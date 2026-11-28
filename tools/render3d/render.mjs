#!/usr/bin/env node
// tools/render3d/render.mjs : renders a 3D scene of tools/render3d/scenes in headless Chromium (three.js, the look of
// style.js), its views as PNGs.
//   node tools/render3d/render.mjs <scene> [--out DIR] [--option key=value ...]
// A scene is a module scenes/<scene>.js : build(THREE, S, options) -> { root, views: [{ name, size: [w, h], ... }], pose(view) }
// (see page.html for the fields of a view). "--sheet" assembles the views named "<row>_<column>" into <scene>.png, the
// layout of the character sheets of the game (Graphic/CharsetSheet.h : 72 px frames, 8 rows, the columns of the walk).
import { chromium } from "playwright";
import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

const here = path.dirname(fileURLToPath(import.meta.url));
// Chromium's temporary files (its profile) in the build folder : /tmp may be full (a quota per user)
if (!process.env.RENDER3D_KEEP_TMPDIR) {
	process.env.TMPDIR = path.join(here, "..", "..", "build", "tmp");
	fs.mkdirSync(process.env.TMPDIR, { recursive: true });
}
const args = process.argv.slice(2);
const scene = args.shift();
if (!scene) {
	console.error("usage : render.mjs <scene> [--out DIR] [--option key=value]");
	process.exit(2);
}
let out = path.join(here, "out");
const options = {};
for (let i = 0; i < args.length; i++) {
	if (args[i] === "--out") out = path.resolve(args[++i]);
	else if (args[i] === "--option") { const [k, ...v] = args[++i].split("="); options[k] = v.join("="); }
	else { console.error(`unknown option ${args[i]}`); process.exit(2); }
}
fs.mkdirSync(out, { recursive: true });

const types = { ".html": "text/html", ".js": "text/javascript", ".mjs": "text/javascript", ".json": "application/json" };
const browser = await chromium.launch({ headless: true, args: ["--use-angle=swiftshader", "--enable-unsafe-swiftshader", "--ignore-gpu-blocklist"] });
try {
	const page = await browser.newPage();
	page.on("console", (message) => { if (message.type() === "error") console.error(`page : ${message.text()}`); });
	page.on("pageerror", (error) => console.error(`page : ${error.message}`));
	// The files : the page, the style, the scenes, three.js (node_modules)
	await page.route("http://render3d.local/**", async (route) => {
		const url = new URL(route.request().url());
		const file = url.pathname.startsWith("/three/") ? path.join(here, "node_modules", url.pathname.slice(1)) : path.join(here, url.pathname.slice(1));
		if (!fs.existsSync(file)) return route.fulfill({ status: 404, body: "" });
		await route.fulfill({ status: 200, contentType: types[path.extname(file)] ?? "application/octet-stream", body: fs.readFileSync(file) });
	});
	await page.goto("http://render3d.local/page.html");
	await page.waitForFunction(() => window.ready === true, null, { timeout: 30000 });
	const results = await page.evaluate(([name, opts]) => window.renderScene(name, opts), [scene, options]);
	for (const result of results) {
		const file = path.join(out, `${result.name}.png`);
		fs.writeFileSync(file, Buffer.from(result.data.split(",")[1], "base64"));
	}
	console.log(`${results.length} pictures of ${scene} in ${out}`);
} finally {
	await browser.close();
}
