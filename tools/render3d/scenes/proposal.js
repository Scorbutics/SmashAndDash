// The design proposal of the tiles not reviewed yet (the grounds, the cliffs and their stairs, the water and its banks,
// the buildings, the objects, a room), after the references of the user : cel shaded painted grounds (sun and shade
// patches, brush strokes), a worn dirt path, pale faceted rocks, brick walls, chunky rows of roof tiles, cast shadows.
// Views : the map (the camera of the game), a diorama (3/4, the same place), then a plate for each family of tiles.
import { build as tree } from "./tree.js";
import { tuft } from "./grass.js";
import { build as hero } from "./hero.js";
import { build as pikachu } from "./pikachu.js";
import { TONES, MATERIALS } from "./tones.js";

const OBLIQUE = 0.8;

export function build(THREE, S, options = {}) {
	const root = new THREE.Group();
	const rand = S.random(17);
	const M = MATERIALS;
	const T = TONES.meadow;
	const mat = (color, o = {}) => S.toon(color, o);
	const mesh = (geometry, color, o = {}) => new THREE.Mesh(geometry, mat(color, o));
	const add = (parent, m, x = 0, y = 0, z = 0) => { m.position.set(x, y, z); parent.add(m); return m; };

	// ------------------------------------------------------------------ materials of the proposal
	const GRASS = "#86c234";
	const grassMat = () => mat(GRASS, { pattern: "grass" });
	const dirtMat = () => mat("#d0844e", { pattern: "dirt" });
	const ROCK = "#dcd4c2";

	// A flat shape on the ground (a path, a square), at a little height over it
	const flat = (parent, shape, material, y = 0.004) => {
		// (the shape turned onto the ground faces down : seen from both sides)
		material.side = THREE.DoubleSide;
		const m = new THREE.Mesh(new THREE.ShapeGeometry(shape, 24), material);
		m.rotation.x = Math.PI / 2;
		m.position.y = y;
		m.userData.noInk = true;
		m.userData.noShadow = true;
		parent.add(m);
		return m;
	};
	const ground = (parent, w, d, material) => {
		const m = new THREE.Mesh(new THREE.PlaneGeometry(w, d), material);
		m.rotation.x = -Math.PI / 2;
		m.userData.noInk = true;
		m.userData.noShadow = true;
		parent.add(m);
		return m;
	};

	// A pale faceted rock (the boulders of the references)
	const rock = (parent, r, x, y, z, squash = 0.75, color = ROCK) => {
		const c = S.clump(r, color, rand, 0);
		c.scale.set(1.15, squash, 1);
		c.rotation.y = rand() * 6;
		add(parent, c, x, y, z);
		return c;
	};

	// A tree stump (the references)
	const stump = (parent, x, z, r = 0.16) => {
		const s = mesh(new THREE.CylinderGeometry(r * 0.9, r * 1.1, r * 1.6, 7), "#8a6a48", { facets: true });
		add(parent, s, x, r * 0.8, z);
		const top = mesh(new THREE.CylinderGeometry(r * 0.88, r * 0.88, 0.02, 7), "#d9b68a", { flat: true });
		top.userData.noInk = true;
		add(parent, top, x, r * 1.6, z);
	};

	// A cliff : a plateau "w" x "d" blocks, "h" high, its front face of pale faceted rocks under an overhanging grass slab ;
	// "stairs" : the column of its stairs (-1 : none)
	const cliff = (parent, x0, z0, w, d, h, stairs = -1) => {
		const g = new THREE.Group();
		g.position.set(x0, 0, z0);
		const top = mesh(new THREE.BoxGeometry(w, 0.16, d), GRASS, { pattern: "grass" });
		add(g, top, w / 2, h - 0.04, d / 2);
		// The lip : a rounded edge of grass hanging over the face
		const lip = S.box(w + 0.06, 0.16, 0.22, "#6fa82c", 0.45, { steps: 3 });
		add(g, lip, w / 2, h - 0.06, d + 0.03);
		// The body behind the face
		const body = mesh(new THREE.BoxGeometry(w - 0.1, h - 0.1, d - 0.1), "#b9ab92", { facets: true });
		body.userData.noInk = true;
		add(g, body, w / 2, (h - 0.1) / 2, d / 2);
		// The face : rows of faceted rocks, big at the foot
		for (let x = 0.3; x < w; x += 0.62) {
			if (stairs >= 0 && x > stairs - 0.2 && x < stairs + 1.2) continue;
			rock(g, 0.5 + rand() * 0.12, x + (rand() - 0.5) * 0.12, 0.42, d - 0.06, 0.9);
			rock(g, 0.4 + rand() * 0.1, x + 0.3 + (rand() - 0.5) * 0.12, h * 0.66, d - 0.14, 0.85);
		}
		// The sides
		for (let z = 0.3; z < d; z += 0.5) {
			rock(g, 0.3, -0.02, 0.3, z, 0.9);
			rock(g, 0.3, w + 0.02, 0.3, z, 0.9);
		}
		if (stairs >= 0) {
			// Chunky stone steps down the face, a cheek of rocks on each side
			const steps = 4;
			for (let i = 0; i < steps; i++) {
				const step = S.box(0.9, h / steps, 0.3, i % 2 ? "#cfc7d6" : "#c4bccd", 0.18, { steps: 3, pattern: "cobble", scale: 2 });
				add(g, step, stairs + 0.5, (h / steps) * (i + 0.5), d - 0.15 + (steps - 1 - i) * 0.3 * -1 + (steps - 1) * 0.3 - 0.0);
				step.position.z = d + 0.6 - (i) * 0.3;
			}
			rock(g, 0.26, stairs - 0.05, 0.25, d + 0.4, 0.9);
			rock(g, 0.26, stairs + 1.05, 0.25, d + 0.4, 0.9);
		}
		parent.add(g);
		return g;
	};

	// A pond : still water, cel bands, a few white ripples, a foam rim ; rocks on its bank
	const pond = (parent, cx, cz, rx, rz) => {
		const shape = new THREE.Shape();
		for (let i = 0; i <= 32; i++) {
			const a = (i / 32) * Math.PI * 2;
			const r = 1 + 0.12 * Math.sin(a * 3) + 0.06 * Math.cos(a * 5);
			const px = cx + Math.cos(a) * rx * r, pz = cz + Math.sin(a) * rz * r;
			if (i === 0) shape.moveTo(px, pz); else shape.lineTo(px, pz);
		}
		const bank = flat(parent, shape, mat("#c79a62", { pattern: "dirt" }), 0.005);
		bank.scale.set(1, 1, 1);
		const inner = shape.clone();
		const water = flat(parent, new THREE.Shape(shape.getPoints(32).map((p) => new THREE.Vector2(cx + (p.x - cx) * 0.86, cz + (p.y - cz) * 0.86))), mat("#4aa2e6", { flat: false }), 0.01);
		void inner;
		// The light on the water : a lighter band, ripples
		const sheen = flat(parent, new THREE.Shape(shape.getPoints(32).map((p) => new THREE.Vector2(cx - rx * 0.15 + (p.x - cx) * 0.45, cz - rz * 0.2 + (p.y - cz) * 0.35))), mat("#a8dcff", { flat: true }), 0.012);
		void sheen;
		for (let i = 0; i < 5; i++) {
			const ripple = mesh(new THREE.TorusGeometry(0.12 + rand() * 0.1, 0.012, 4, 20, Math.PI * 0.8), "#ffffff", { flat: true });
			ripple.rotation.x = -Math.PI / 2;
			ripple.userData.noInk = true;
			add(parent, ripple, cx + (rand() - 0.5) * rx, 0.016, cz + (rand() - 0.5) * rz);
		}
		for (let i = 0; i < 9; i++) {
			const a = (i / 9) * Math.PI * 2 + rand() * 0.4;
			rock(parent, 0.14 + rand() * 0.1, cx + Math.cos(a) * rx * 1.02, 0.06, cz + Math.sin(a) * rz * 1.02, 0.6);
		}
		// A lily pad, reeds
		const lily = mesh(new THREE.CylinderGeometry(0.16, 0.16, 0.02, 12, 1, false, 0.4, Math.PI * 1.8), "#5aa83a", { flat: true });
		add(parent, lily, cx + rx * 0.35, 0.02, cz + rz * 0.1);
		for (let i = 0; i < 6; i++) {
			const reed = mesh(new THREE.ConeGeometry(0.025, 0.5 + rand() * 0.3, 4), "#5f9a2c", { flat: true });
			reed.rotation.z = (rand() - 0.5) * 0.4;
			add(parent, reed, cx - rx * 0.8 + rand() * 0.3, 0.3, cz - rz * 0.3 + rand() * 0.4);
		}
	};

	// A roof of tiles : rows of chunky rounded tiles down the slope (the reference with walls), a ridge
	const tiledRoof = (parent, w, depth, rise, color, overhang = 0.18, snowy = false) => {
		const g = new THREE.Group();
		const slope = Math.atan2(rise, depth / 2);
		const length = Math.hypot(rise, depth / 2) + overhang;
		const rows = Math.max(3, Math.round(length / 0.22));
		for (const side of [-1, 1]) {
			const half = new THREE.Group();
			half.position.set(0, rise, 0);
			half.rotation.x = side * (Math.PI / 2 - slope);
			for (let r = 0; r < rows; r++) {
				const shade = r % 2 ? M[color][1] : M[color][2] === undefined ? M[color][1] : M[color][1];
				const tile = S.box(w + overhang * 2, 0.07, length / rows + 0.04, shade, 0.45, { steps: 3 });
				tile.position.set(0, -r * 0.012, side * ((r + 0.5) * length / rows));
				half.add(tile);
				// The seams between the tiles of a row
				for (let k = -w / 2; k <= w / 2 + 0.01; k += 0.25) {
					const seam = S.box(0.03, 0.075, length / rows, M[color][2], 0.4, { flat: true });
					seam.userData.noInk = true;
					seam.position.set(k, 0.01 - r * 0.012, side * ((r + 0.5) * length / rows));
					half.add(seam);
				}
			}
			g.add(half);
		}
		const ridge = S.box(w + overhang * 2 + 0.06, 0.14, 0.18, M[color][2], 0.45, { steps: 3 });
		add(g, ridge, 0, rise + 0.04, 0);
		if (snowy) {
			// Snow on each slope, its edge hanging a little over the tiles
			for (const side of [-1, 1]) {
				const snow = S.box(w + overhang * 2, 0.09, length * 0.62, "#f4f8fc", 0.5, { steps: 3 });
				snow.rotation.x = side * (Math.PI / 2 - slope);
				const along = length * 0.31;
				snow.position.set(0, rise - Math.sin(slope) * along + 0.06, side * Math.cos(slope) * along);
				g.add(snow);
			}
		}
		parent.add(g);
		return g;
	};

	// A window : a frame, panes, shutters, a flower box
	const windowAt = (parent, x, y, z, w = 0.42, h = 0.36, shutters = M.roofr[1]) => {
		const frame = S.box(w + 0.08, h + 0.08, 0.06, "#fff8ec", 0.3, { steps: 3 });
		add(parent, frame, x, y, z);
		const pane = S.box(w, h, 0.04, M.glass[1], 0.2, { steps: 3 });
		pane.userData.noInk = true;
		add(parent, pane, x, y, z + 0.02);
		const bar = S.box(0.03, h, 0.02, "#fff8ec", 0.3, { flat: true });
		bar.userData.noInk = true;
		add(parent, bar, x, y, z + 0.045);
		for (const side of [-1, 1]) {
			const shutter = S.box(0.13, h + 0.06, 0.04, shutters, 0.3, { steps: 3, pattern: "planks", scale: 3 });
			add(parent, shutter, x + side * (w / 2 + 0.11), y, z + 0.01);
		}
		const box = S.box(w + 0.04, 0.08, 0.1, "#a8703f", 0.3, { steps: 3 });
		add(parent, box, x, y - h / 2 - 0.07, z + 0.05);
		for (let i = 0; i < 4; i++) {
			const f = S.blob(0.04, 0.04, 0.04, i % 2 ? "#ff6a7a" : "#ffd24a", { flat: true });
			f.userData.noInk = true;
			add(parent, f, x - w / 2 + 0.07 + i * (w - 0.1) / 3, y - h / 2 - 0.02, z + 0.08);
		}
	};

	// A house (4 x 4 blocks in the game) : brick walls on a stone base, a tiled roof, a chimney, a plank door
	const house = (parent, x0, z0, roof = "roofr", style = "meadow") => {
		const g = new THREE.Group();
		g.position.set(x0, 0, z0);
		const w = 3.4, d = 2.2, h = 1.35;
		const wallColor = style === "desert" ? "#e8c89a" : (style === "autumn" ? "#b87a4c" : "#e9d2b0");
		const pattern = style === "desert" ? "plaster" : (style === "autumn" ? "planks" : "brick");
		const walls = mesh(new THREE.BoxGeometry(w, h, d), wallColor, { pattern, scale: style === "autumn" ? 1.3 : 1 });
		add(g, walls, 0, h / 2, 0);
		const base = mesh(new THREE.BoxGeometry(w + 0.08, 0.18, d + 0.08), "#b8b0c4", { pattern: "cobble", scale: 2.5 });
		add(g, base, 0, 0.09, 0);
		tiledRoof(g, w, d, 0.85, roof, 0.22, style === "snow").position.y = h;
		const chimney = mesh(new THREE.BoxGeometry(0.28, 0.75, 0.28), "#c99a7a", { pattern: "brick", scale: 1.6 });
		add(g, chimney, w * 0.28, h + 0.85, -0.25);
		const door = S.box(0.46, 0.8, 0.08, "#9a5f34", 0.2, { steps: 3, pattern: "planks", scale: 2.5 });
		add(g, door, 0.35, 0.48, d / 2 + 0.02);
		const awning = S.box(0.62, 0.06, 0.24, M[roof][1], 0.4, { steps: 3 });
		add(g, awning, 0.35, 0.95, d / 2 + 0.1);
		const knob = S.blob(0.03, 0.03, 0.03, "#ffd24a", { flat: true });
		knob.userData.noInk = true;
		add(g, knob, 0.5, 0.48, d / 2 + 0.08);
		windowAt(g, -0.95, 0.72, d / 2 + 0.02, 0.5, 0.36, M[roof][1]);
		windowAt(g, 1.25, 0.82, d / 2 + 0.02, 0.24, 0.24, M[roof][1]);
		const step = S.box(0.7, 0.08, 0.3, "#c4bccd", 0.3, { steps: 3 });
		add(g, step, 0.35, 0.04, d / 2 + 0.2);
		parent.add(g);
		return g;
	};

	// The Pokémon Center (5 x 5 blocks) : white plaster, a teal tiled roof, the sign of the Center, a wide glass door
	const center = (parent, x0, z0) => {
		const g = new THREE.Group();
		g.position.set(x0, 0, z0);
		const w = 4.4, d = 2.8, h = 1.7;
		const walls = mesh(new THREE.BoxGeometry(w, h, d), "#f6f4ee", { pattern: "plaster" });
		add(g, walls, 0, h / 2, 0);
		const band = S.box(w + 0.04, 0.16, d + 0.04, M.teal[1], 0.3, { steps: 3 });
		add(g, band, 0, 0.3, 0);
		tiledRoof(g, w, d, 1.0, "teal", 0.24).position.y = h;
		const sign = mesh(new THREE.CylinderGeometry(0.32, 0.32, 0.08, 20), M.orange[1], { steps: 3 });
		sign.rotation.x = Math.PI / 2;
		add(g, sign, 0, h + 0.5, d / 2 + 0.25);
		const cross1 = S.box(0.34, 0.1, 0.04, "#ffffff", 0.3, { flat: true });
		const cross2 = S.box(0.1, 0.34, 0.04, "#ffffff", 0.3, { flat: true });
		add(g, cross1, 0, h + 0.5, d / 2 + 0.3);
		add(g, cross2, 0, h + 0.5, d / 2 + 0.3);
		for (const side of [-1, 1]) {
			const door = S.box(0.42, 0.95, 0.06, M.glass[1], 0.15, { steps: 3 });
			add(g, door, side * 0.23, 0.55, d / 2 + 0.03);
			windowAt(g, side * 1.45, 1.0, d / 2 + 0.02, 0.8, 0.5, M.teal[1]);
		}
		const awning = new THREE.Group();
		for (let i = 0; i < 6; i++) {
			const stripe = S.box(0.2, 0.06, 0.34, i % 2 ? "#ffffff" : M.orange[1], 0.3, { steps: 3 });
			stripe.position.set(-0.5 + i * 0.2, 0, 0);
			awning.add(stripe);
		}
		add(g, awning, 0, 1.12, d / 2 + 0.16);
		parent.add(g);
		return g;
	};

	// A tent (2 x 2) : striped canvas, poles
	const tent = (parent, x0, z0, canvas = "roofo") => {
		const g = new THREE.Group();
		g.position.set(x0, 0, z0);
		const shape = new THREE.Shape([new THREE.Vector2(-0.8, 0), new THREE.Vector2(0.8, 0), new THREE.Vector2(0, 1.1)]);
		const body = mesh(new THREE.ExtrudeGeometry(shape, { depth: 1.2, bevelEnabled: false }), M[canvas][1], { facets: true, steps: 3 });
		add(g, body, 0, 0, -0.6);
		const opening = new THREE.Shape([new THREE.Vector2(-0.3, 0), new THREE.Vector2(0.3, 0), new THREE.Vector2(0, 0.62)]);
		const hole = mesh(new THREE.ShapeGeometry(opening), "#4a2e22", { flat: true });
		hole.userData.noInk = true;
		add(g, hole, 0, 0.001, 0.61);
		parent.add(g);
		return g;
	};

	// A pillar (1 x 2) : stacked stone drums, broken or whole
	const pillar = (parent, x0, z0, broken = false) => {
		const g = new THREE.Group();
		g.position.set(x0, 0, z0);
		add(g, S.box(0.6, 0.16, 0.6, "#d8d0e2", 0.25, { steps: 3 }), 0, 0.08, 0);
		const drums = broken ? 2 : 4;
		for (let i = 0; i < drums; i++) {
			add(g, mesh(new THREE.CylinderGeometry(0.21, 0.23, 0.36, 10), i % 2 ? "#e2dcea" : "#d6cfe0", { facets: true }), 0, 0.34 + i * 0.36, 0);
		}
		if (broken) rock(g, 0.16, 0.35, 0.08, 0.2, 0.6, "#d6cfe0");
		else add(g, S.box(0.56, 0.14, 0.56, "#d8d0e2", 0.25, { steps: 3 }), 0, 1.66, 0);
		parent.add(g);
		return g;
	};

	// The objects : a fence run, a bench, a barrel, a lamp, a sign, a well, a mailbox, a bin, crops
	const fence = (parent, x0, z0, n) => {
		for (let i = 0; i <= n; i++) add(parent, S.box(0.09, 0.5, 0.09, "#b88a56", 0.3, { steps: 3, pattern: "planks", scale: 4 }), x0 + i * 0.5, 0.25, z0);
		for (const y of [0.18, 0.38]) add(parent, S.box(n * 0.5, 0.06, 0.05, "#c9995e", 0.35, { steps: 3, pattern: "planks", scale: 4 }), x0 + n * 0.25, y, z0);
	};
	const bench = (parent, x, z) => {
		add(parent, S.box(0.9, 0.06, 0.3, "#c08a50", 0.3, { steps: 3, pattern: "planks", scale: 4 }), x, 0.26, z);
		add(parent, S.box(0.9, 0.2, 0.05, "#c08a50", 0.3, { steps: 3, pattern: "planks", scale: 4 }), x, 0.42, z - 0.13);
		for (const s of [-1, 1]) add(parent, S.box(0.06, 0.26, 0.26, M.metal[1], 0.3, { steps: 3 }), x + s * 0.38, 0.13, z);
	};
	const barrel = (parent, x, z) => {
		add(parent, mesh(new THREE.CylinderGeometry(0.2, 0.2, 0.5, 12), "#b07a44", { pattern: "planks", scale: 5 }), x, 0.25, z);
		for (const y of [0.1, 0.4]) {
			const hoop = mesh(new THREE.TorusGeometry(0.205, 0.02, 4, 16), M.metal[1], { flat: true });
			hoop.rotation.x = Math.PI / 2;
			hoop.userData.noInk = true;
			add(parent, hoop, x, y, z);
		}
	};
	const lamp = (parent, x, z) => {
		add(parent, mesh(new THREE.CylinderGeometry(0.04, 0.06, 1.3, 8), M.metal[1], { steps: 3 }), x, 0.65, z);
		add(parent, S.box(0.2, 0.24, 0.2, "#ffe48a", 0.3, { flat: true }), x, 1.36, z);
		add(parent, S.box(0.26, 0.06, 0.26, M.metal[2], 0.3, { steps: 3 }), x, 1.5, z);
	};
	const sign = (parent, x, z) => {
		add(parent, S.box(0.08, 0.5, 0.08, "#9a6a3a", 0.3, { steps: 3 }), x, 0.25, z);
		add(parent, S.box(0.6, 0.32, 0.06, "#c9995e", 0.25, { steps: 3, pattern: "planks", scale: 4 }), x, 0.56, z + 0.03);
	};
	const well = (parent, x, z) => {
		add(parent, mesh(new THREE.CylinderGeometry(0.42, 0.45, 0.42, 14), "#cfc7d6", { pattern: "cobble", scale: 3 }), x, 0.21, z);
		const water = mesh(new THREE.CylinderGeometry(0.34, 0.34, 0.02, 14), "#3f86c8", { flat: true });
		water.userData.noInk = true;
		add(parent, water, x, 0.4, z);
		for (const s of [-1, 1]) add(parent, S.box(0.07, 0.8, 0.07, "#9a6a3a", 0.3, { steps: 3 }), x + s * 0.38, 0.7, z);
		const roofW = new THREE.Group();
		tiledRoof(roofW, 0.9, 0.7, 0.3, "roofr", 0.08);
		add(parent, roofW, x, 1.05, z);
	};
	const crops = (parent, x0, z0) => {
		for (let r = 0; r < 3; r++) {
			add(parent, S.box(1.6, 0.08, 0.26, "#8a5a36", 0.4, { steps: 3, pattern: "dirt" }), x0 + 0.8, 0.04, z0 + r * 0.38);
			for (let i = 0; i < 6; i++) {
				const leaf = S.clump(0.09, i % 2 ? "#6fb03a" : "#86c22c", rand, 0);
				add(parent, leaf, x0 + 0.15 + i * 0.26, 0.14, z0 + r * 0.38);
			}
		}
	};
	const grassTuft = (parent, x, z, s = 1) => {
		const t = tuft(THREE, S, rand, 7);
		t.scale.setScalar(s);
		add(parent, t, x, 0, z);
	};
	const treeAt = (parent, x, z, seed, height = 3.4, kind = undefined) => {
		const t = tree(THREE, S, { seed, height, kind }).root;
		add(parent, t, x, 0, z);
		t.rotation.y = rand() * 6;
	};

	// ------------------------------------------------------------------ the village corner (the map, the diorama)
	const village = new THREE.Group();
	root.add(village);
	ground(village, 30, 24, grassMat()).position.set(0, 0, 0);
	// The path : from the stairs down to the square
	const path = new THREE.Shape();
	path.moveTo(-3.6, -3.4);
	path.bezierCurveTo(-3.2, -1.5, -1.0, -1.4, -0.6, 0.4);
	path.bezierCurveTo(-0.3, 1.6, 0.2, 2.4, 0.6, 5.0);
	path.lineTo(1.5, 5.0);
	path.bezierCurveTo(1.1, 2.4, 0.6, 1.4, 0.3, 0.2);
	path.bezierCurveTo(-0.1, -1.6, -2.3, -1.9, -2.6, -3.4);
	path.closePath();
	flat(village, path, dirtMat());
	// The square in front of the Center : cobbles
	const square = new THREE.Shape();
	square.moveTo(1.2, -0.6);
	square.lineTo(6.6, -0.6);
	square.lineTo(6.6, 2.0);
	square.lineTo(1.2, 2.0);
	square.closePath();
	flat(village, square, mat("#d2cddd", { pattern: "cobble" }), 0.006);
	// The plateau at the back, its stairs down to the path
	cliff(village, -9, -7.2, 7.4, 3.6, 1.6, 4.6);
	cliff(village, 6.4, -7.2, 4, 2.6, 1.6);
	center(village, 3.9, -2.2);
	house(village, -4.6, 1.6, "roofr");
	pond(village, -6.6, 4.8, 2.2, 1.3);
	fence(village, 2.2, 4.2, 5);
	bench(village, 2.2, 1.4);
	lamp(village, 1.4, 0.2);
	lamp(village, 6.4, 0.2);
	sign(village, -0.8, 3.0);
	barrel(village, -2.6, 2.8);
	barrel(village, -2.2, 3.0);
	well(village, 5.6, 3.0);
	crops(village, 2.6, 5.0);
	treeAt(village, -9.4, -1.0, 3, 3.6);
	treeAt(village, -8.2, 1.0, 8, 3.2, "gold");
	treeAt(village, 9.0, 0.6, 12, 3.6);
	treeAt(village, 8.4, 4.4, 21, 3.4);
	treeAt(village, -7.6, -5.0, 31, 3.8);
	treeAt(village, 0.4, -6.6, 44, 3.8, "gold");
	stump(village, -1.8, 4.8);
	stump(village, -3.4, 5.6, 0.13);
	stump(village, 7.6, 2.4, 0.12);
	rock(village, 0.45, -9.0, 0.2, 4.8, 0.6);
	rock(village, 0.3, 9.2, 0.12, 2.6, 0.6);
	for (let i = 0; i < 26; i++) {
		const x = -9 + rand() * 18, z = 2.2 + rand() * 4;
		if (x > -1.2 && x < 2.2) continue;
		if (x > -8.6 && x < -4.6 && z > 3.4) continue;
		grassTuft(village, x, z, 0.8 + rand() * 0.6);
	}
	const h = hero(THREE, S);
	h.pose({ row: 2, column: 1 });
	add(village, h.root, 0.1, 0, 2.6);
	const p = pikachu(THREE, S);
	p.pose({ row: 6, column: 1 });
	add(village, p.root, 0.9, 0, 3.2);
	p.root.rotation.y = -0.4;

	// ------------------------------------------------------------------ the plates (one family of tiles each), far away
	const plates = new THREE.Group();
	plates.position.set(100, 0, 0);
	root.add(plates);
	const plate = (x, z, w, d, material = grassMat()) => {
		const g = new THREE.Group();
		g.position.set(x, 0, z);
		ground(g, w, d, material);
		plates.add(g);
		return g;
	};
	// Grounds in the 4 biomes (grass, path, sand, cobbles)
	const biomes = ["meadow", "autumn", "snow", "desert"];
	biomes.forEach((b, i) => {
		const t = TONES[b];
		const grass = plate(i * 4, 0, 3, 3, mat(t.ground.grass, { pattern: "grass" }));
		const strip = new THREE.Shape();
		strip.moveTo(-1.5, -0.4); strip.bezierCurveTo(-0.5, -0.8, 0.5, 0.2, 1.5, -0.2); strip.lineTo(1.5, 0.5); strip.bezierCurveTo(0.5, 0.9, -0.5, 0.0, -1.5, 0.4); strip.closePath();
		flat(grass, strip, mat(b === "snow" ? "#b8aca4" : t.ground.path, { pattern: "dirt" }));
		const sand = new THREE.Shape();
		sand.moveTo(0.4, 0.8); sand.lineTo(1.5, 0.8); sand.lineTo(1.5, 1.5); sand.lineTo(0.2, 1.5); sand.closePath();
		flat(grass, sand, mat(t.ground.sand, { pattern: "dirt" }), 0.006);
		const cob = new THREE.Shape();
		cob.moveTo(-1.5, 0.8); cob.lineTo(-0.3, 0.8); cob.lineTo(-0.3, 1.5); cob.lineTo(-1.5, 1.5); cob.closePath();
		flat(grass, cob, mat(t.ground.pave, { pattern: "cobble" }), 0.006);
		grassTuft(grass, 0.8, -1.0, 0.6);
		stump(grass, -1.0, -1.1, 0.12);
	});
	// The cliff : a corner, a face, stairs
	{
		const g = plate(4, 8, 6, 5);
		cliff(g, -2.5, -2.4, 4, 2, 1.6, 1.5);
		cliff(g, -2.5 + 4, -2.4, 1, 1.2, 1.6);
	}
	// The pond and its banks
	pond(plate(12, 8, 5, 4), 0, 0, 1.5, 0.9);
	// The buildings
	house(plate(0, 16, 5, 4), 0, -0.2, "roofb");
	house(plate(6, 16, 5, 4), 0, -0.2, "roofg", "snow");
	house(plate(12, 16, 5, 4), 0, -0.2, "roofo", "desert");
	house(plate(18, 16, 5, 4), 0, -0.2, "roofr", "autumn");
	center(plate(4, 24, 6, 5), 0, -0.3);
	{
		const g = plate(12, 24, 5, 4);
		tent(g, -1.0, 0, "roofo");
		tent(g, 1.2, 0.2, "roofb");
	}
	{
		const g = plate(18, 24, 4, 4);
		pillar(g, -0.8, 0);
		pillar(g, 0.8, 0, true);
	}
	// The objects
	{
		const g = plate(6, 31, 12, 3.4);
		fence(g, -5.6, -0.8, 4);
		bench(g, -2.2, 0.2);
		barrel(g, -0.8, 0.0);
		lamp(g, 0.2, 0.0);
		sign(g, 1.2, 0.2);
		well(g, 2.8, 0.1);
		crops(g, 3.8, -0.6);
		rock(g, 0.3, -4.6, 0.12, 0.9, 0.6);
		stump(g, -1.6, 1.0);
	}
	// A room : planks, a plaster wall with a wainscot, a window, a bed, a table, a bookshelf, a rug
	{
		const g = new THREE.Group();
		g.position.set(18, 0, 32);
		plates.add(g);
		const floor = mesh(new THREE.PlaneGeometry(5, 4), "#d8a466", { pattern: "planks", scale: 1.2 });
		floor.rotation.x = -Math.PI / 2;
		floor.userData.noInk = true;
		floor.userData.noShadow = true;
		g.add(floor);
		const wall = mesh(new THREE.BoxGeometry(5, 1.4, 0.12), "#f2e2c4", { pattern: "plaster" });
		add(g, wall, 0, 0.7, -2);
		const wainscot = mesh(new THREE.BoxGeometry(5, 0.45, 0.16), "#b98652", { pattern: "planks", scale: 2 });
		add(g, wainscot, 0, 0.22, -1.97);
		windowAt(g, -1.2, 0.95, -1.92, 0.6, 0.4, M.roofb[1]);
		const rug = mesh(new THREE.BoxGeometry(2.0, 0.02, 1.3), M.roofr[1], { steps: 3 });
		add(g, rug, 0.4, 0.01, 0.4);
		const border = mesh(new THREE.BoxGeometry(1.8, 0.025, 1.1), "#f6c860", { flat: true });
		border.userData.noInk = true;
		add(g, border, 0.4, 0.012, 0.4);
		const inner = mesh(new THREE.BoxGeometry(1.7, 0.03, 1.0), M.roofr[1], { steps: 3 });
		inner.userData.noInk = true;
		add(g, inner, 0.4, 0.014, 0.4);
		// Bed
		add(g, S.box(0.9, 0.3, 1.7, "#a8703f", 0.25, { steps: 3, pattern: "planks", scale: 4 }), -1.9, 0.15, -0.9);
		add(g, S.box(0.8, 0.12, 1.2, M.roofb[1], 0.4, { steps: 3 }), -1.9, 0.36, -0.6);
		add(g, S.box(0.6, 0.12, 0.3, "#ffffff", 0.45, { steps: 3 }), -1.9, 0.38, -1.5);
		// Table, stools
		add(g, S.box(1.0, 0.08, 0.6, "#c08a50", 0.3, { steps: 3, pattern: "planks", scale: 4 }), 1.3, 0.5, -0.6);
		for (const s of [-1, 1]) add(g, S.box(0.07, 0.48, 0.5, "#9a6a3a", 0.3, { steps: 3 }), 1.3 + s * 0.42, 0.24, -0.6);
		for (const s of [-1, 1]) add(g, mesh(new THREE.CylinderGeometry(0.16, 0.16, 0.3, 10), "#c08a50", { steps: 3 }), 1.3 + s * 0.75, 0.15, -0.3);
		// Bookshelf against the wall
		add(g, S.box(0.9, 1.3, 0.35, "#9a6a3a", 0.15, { steps: 3, pattern: "planks", scale: 3 }), 2.0, 0.65, -1.75);
		for (let r = 0; r < 3; r++) {
			for (let i = 0; i < 6; i++) {
				const book = S.box(0.1, 0.26, 0.04, [M.roofr[1], M.roofb[1], M.roofg[1], M.orange[1]][(r + i) % 4], 0.2, { steps: 3 });
				book.userData.noInk = true;
				add(g, book, 1.67 + i * 0.13, 0.3 + r * 0.38, -1.56);
			}
		}
		// A plant
		add(g, mesh(new THREE.CylinderGeometry(0.16, 0.12, 0.3, 10), M.orange[1], { steps: 3 }), -0.4, 0.15, -1.6);
		const leaves = S.clump(0.3, "#6fb03a", rand, 1);
		add(g, leaves, -0.4, 0.55, -1.6);
	}

	S.ink(root);

	const views = [
		{ name: "map", size: [960, 672], oblique: OBLIQUE, target: [0, -0.8], key: false, background: GRASS, ink: 1.1, wash: 0.5, supersample: 3, shadows: 16, shadowCenter: [0, 0, 0] },
		{ name: "diorama", size: [960, 600], perspective: true, fov: 34, target: [-0.5, 0.6, 0.2], elevation: 34, yaw: 24, distance: 21, background: "#bfe0f4", ink: 1.2, wash: 0.5, supersample: 3, shadows: 16, shadowCenter: [0, 0, 0] },
	];
	// The same, with the brush of Okami (strokes that swell and taper, sumi ink, parchment)
	views.push({ ...views[0], name: "map_okami", brush: 1, ink: 1.6 });
	views.push({ ...views[1], name: "diorama_okami", brush: 1, ink: 1.7, background: "#f0dfb8" });
	const plateView = (name, cx, cz, w, h, extra = {}) => views.push({ name, size: [w, h], oblique: OBLIQUE, target: [100 + cx, cz], key: false, background: GRASS, ink: 1.0, wash: 0.45, supersample: 3, shadows: 8, shadowCenter: [100 + cx, 0, cz], ...extra });
	plateView("plate_grounds", 6, 0.1, 15 * 48, 3 * 48);
	plateView("plate_cliff", 4, 7.2, 6 * 48, 6 * 48);
	plateView("plate_pond", 12, 8, 5 * 48, 4 * 48);
	plateView("plate_houses", 9, 14.6, 23 * 48, 5 * 48);
	plateView("plate_center", 4, 22.9, 6 * 48, 6 * 48);
	plateView("plate_tent_pillar", 15, 23.6, 11 * 48, 4 * 48);
	plateView("plate_objects", 6, 30.7, 12 * 48, 4 * 48);
	plateView("plate_room", 18, 31.8, 5 * 48, 5 * 48, { background: "#d8a466" });
	return { root, views };
}
