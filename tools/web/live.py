#!/usr/bin/env python3
# Live web server (tools/web/serve.sh --live) : serves the web build like serve.sh, rebuilds it (tools/web/build.sh)
# when a source or a game file changes, and the open pages reload themselves after each successful build.
# Watched : src, external (the engine), the game directories packed in the page (src/CMakeLists.txt ASSETS), except the
# saves and the logs. The page gets a small script (not in src/web/shell.html) : it polls /__live and shows the
# build state in a corner. The build log : <build>/game/bin/live-build.log (served : /live-build.log).
import argparse
import http.server
import json
import os
import subprocess
import sys
import threading
import time

ROOT = os.path.realpath(os.path.join(os.path.dirname(__file__), "..", ".."))
WATCHED = ["src", "external", "Data", "Levels", "Sprites", "Sounds", "Music", "Fonts", "Menu", "Resources", "gamesettings.ini"]
SKIPPED_DIRS = {".git", "node_modules", "Saves", "build", "build-web", "tools", "doc", "docs"}
SKIPPED_SUFFIXES = (".log.txt", "Log.txt", "LogError.txt", "~", ".swp", ".tmp")
QUIET = 1.0  # seconds without a change before building

CLIENT = b"""<script>(function () {
	var version = null, box = document.createElement("div");
	box.style.cssText = "position:fixed;left:8px;bottom:8px;z-index:9999;padding:4px 8px;font:12px monospace;" +
		"border-radius:4px;color:#fff;pointer-events:none;display:none";
	document.addEventListener("DOMContentLoaded", function () { document.body.appendChild(box); });
	function show(text, color) { box.textContent = text; box.style.background = color; box.style.display = text ? "block" : "none"; }
	function poll() {
		fetch("/__live", { cache: "no-store" }).then(function (r) { return r.json(); }).then(function (s) {
			if (version === null) { version = s.version; }
			if (s.version !== version) { location.reload(); return; }
			if (s.state === "building") { show("Rebuilding...", "#b07000"); }
			else if (s.state === "failed") { show("Build failed : /live-build.log", "#b00020"); }
			else { show("", ""); }
		}).catch(function () { show("Live server stopped", "#555"); }).then(function () { setTimeout(poll, 1000); });
	}
	poll();
})();</script>"""


class Live:
	def __init__(self, build):
		self.build = build
		self.log = os.path.join(build, "live-build.log")
		self.version = 0
		self.state = "idle"
		self.lock = threading.Lock()

	def status(self):
		with self.lock:
			return {"version": self.version, "state": self.state, "log": self.log}

	def snapshot(self):
		files = {}
		for name in WATCHED:
			top = os.path.join(ROOT, name)
			if os.path.isfile(top):
				files[top] = os.stat(top).st_mtime_ns
				continue
			for directory, dirs, names in os.walk(top):
				dirs[:] = [d for d in dirs if d not in SKIPPED_DIRS and not d.startswith("build")]
				for n in names:
					if n.endswith(SKIPPED_SUFFIXES):
						continue
					path = os.path.join(directory, n)
					try:
						files[path] = os.stat(path).st_mtime_ns
					except OSError:
						pass
		return files

	def rebuild(self, reason):
		with self.lock:
			self.state = "building"
		print(f"[live] {reason} : building...", flush=True)
		start = time.time()
		with open(self.log, "w") as log:
			result = subprocess.run([os.path.join(ROOT, "tools", "web", "build.sh"), os.path.dirname(os.path.dirname(self.build))],
				stdout=log, stderr=subprocess.STDOUT, cwd=ROOT)
		ok = result.returncode == 0 and os.path.isfile(os.path.join(self.build, "index.html"))
		with self.lock:
			if ok:
				self.version += 1
				self.state = "idle"
			else:
				self.state = "failed"
		print(f"[live] build {'done' if ok else 'FAILED (' + self.log + ')'} in {time.time() - start:.0f} s", flush=True)

	def watch(self, build_first):
		if build_first:
			self.rebuild("start")
		files = self.snapshot()
		while True:
			time.sleep(QUIET)
			current = self.snapshot()
			if current == files:
				continue
			# Waits for the end of the changes (an editor saving several files, a git checkout)
			while True:
				time.sleep(QUIET)
				later = self.snapshot()
				if later == current:
					break
				current = later
			changed = [p for p in set(files) | set(current) if files.get(p) != current.get(p)]
			files = current
			self.rebuild(f"{len(changed)} file(s) changed ({os.path.relpath(sorted(changed)[0], ROOT)}...)")


def handler(live, directory):
	class Handler(http.server.SimpleHTTPRequestHandler):
		def __init__(self, *args, **kwargs):
			super().__init__(*args, directory=directory, **kwargs)

		def end_headers(self):
			# Always the last build (index.wasm, index.data)
			self.send_header("Cache-Control", "no-store")
			super().end_headers()

		def do_GET(self):
			path = self.path.split("?")[0]
			if path == "/__live":
				return self.send_bytes(json.dumps(live.status()).encode(), "application/json")
			if path in ("/", "/index.html"):
				try:
					with open(os.path.join(directory, "index.html"), "rb") as f:
						page = f.read()
				except OSError:
					page = b"<html><body>No web build yet : building...</body></html>"
				marker = page.rfind(b"</body>")
				page = page[:marker] + CLIENT + page[marker:] if marker >= 0 else page + CLIENT
				return self.send_bytes(page, "text/html; charset=utf-8")
			return super().do_GET()

		def send_bytes(self, body, content_type):
			self.send_response(200)
			self.send_header("Content-Type", content_type)
			self.send_header("Content-Length", str(len(body)))
			self.end_headers()
			self.wfile.write(body)

		def log_message(self, *args):
			pass

	return Handler


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("build", nargs="?", default=os.path.join(ROOT, "build-web"))
	parser.add_argument("--port", type=int, default=int(os.environ.get("PORT", "8000")))
	parser.add_argument("--bind", default="0.0.0.0")
	parser.add_argument("--no-initial-build", action="store_true", help="serve the current build, build at the first change")
	args = parser.parse_args()
	directory = os.path.join(os.path.realpath(args.build), "game", "bin")
	os.makedirs(directory, exist_ok=True)
	live = Live(directory)
	threading.Thread(target=live.watch, args=(not args.no_initial_build,), daemon=True).start()
	server = http.server.ThreadingHTTPServer((args.bind, args.port), handler(live, directory))
	print(f"[live] serving {directory} on {args.bind}:{args.port}, watching the sources and the game files", flush=True)
	try:
		server.serve_forever()
	except KeyboardInterrupt:
		sys.exit(0)


if __name__ == "__main__":
	main()
