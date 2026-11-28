// The painted objects and animated tiles of a biome (options.biome), in the projection of the map (page.html "oblique" :
// the ground from above, the heights up the screen ; 48 px a block), after the voxel drawings of tools/tilegen (each
// object where the voxel one stands, with its footprint : the collision mask of an object is its drawn pixels,
// tools/worldgen tile_mask). The same geometry in every biome ; the ground colours from TONES, the objects from MATERIALS.
// The look of the approved proposal (scenes/proposal.js) : plank posts and rails, planks with metal hoops, cobbled
// stones, a little tiled roof, dirt furrows and leaf clumps, pale faceted rocks, water in two blues with a lighter sheen
// and thin white ripples ; cast shadows (on the objects themselves : no ground in a keyed tile ; on the ground of the
// opaque tiles), few large shapes, subtle patterns.
//   obj_<name>        the furniture of the villages and the rocks (key) : bench, bin, mailbox, barrel, bush, well, lily
//                     (over the water), boulder, log_seat, sign ; crops (opaque, tiles)
//   lamp              48 x 96 : the street lamp (the generator keeps its bottom row, the post) ; lamphead_<f> its lantern
//   fence, fence_tl, fence_tr, fence_v   a garden fence (rendered with its neighbours : the rails go on across the tiles)
//   flowerpot_<c>, emblem_<c>_<r> (the 2 x 2 round sign on the pavement)
//   animated, 4 frames : water_<f>, flower_<f> (opaque, they tile), campfire_<f>, butterfly_<f> (opaque, on the grass),
//   fountain_<c>_<r>_<f> (2 x 2, key), flowerbed_<c>_<f> (opaque, they tile)
// A block is a world unit, its centre at (x, z) ; a voxel of the drawings is 1/16 of it. A point at the height h is drawn
// 0.8 h higher on the screen than its ground point.
import { TONES, MATERIALS } from "./tones.js";

const OBLIQUE = 0.8;
const U = 1 / 16;
// The ground coordinate of a voxel column / row of a tile (its centre at 8)
const at = (v) => (v - 8) * U;
// The flowers of the voxel palette "flower" (yellow ; blue in the snow, as tools/tilegen BIOMES)
const BLOOM = { meadow: ["#fff07a", "#f8d040", "#d09a20"], autumn: ["#fff07a", "#f8d040", "#d09a20"],
	snow: ["#e2f0ff", "#8cc0f0", "#5a86c8"], desert: ["#fff07a", "#f8d040", "#d09a20"] };
const FRAMES = [0, 1, 2, 3];
// The colours of the proposal : its woods, its pale rock, its stones, its water, its soil
const WOOD = { post: "#9a6a3a", fence: "#b88a56", rail: "#c9995e", bench: "#c08a50", barrel: "#b07a44", stump: "#8a6a48", cut: "#d9b68a" };
const ROCK = "#dcd4c2";
const COBBLE = "#cfc7d6";
const WATER = { base: "#4aa2e6", light: "#64b2ea", deep: "#3f86c8", sheen: "#a8dcff", ripple: "#ffffff" };
const SOIL = { ground: "#7a4e2e", furrow: "#8a5a36" };
const LEAF = ["#86c22c", "#6fb03a"];

export function build(THREE, S, options = {}) {
	const biome = options.biome ?? "meadow";
	const tones = TONES[biome];
	if (!tones) throw new Error(`tiles_objects : no biome ${biome}`);
	const M = { ...MATERIALS, flower: BLOOM[biome] };
	const grass = tones.ground.grass;
	const root = new THREE.Group();
	const parts = {};
	const views = [];
	const soft = (a, b, t) => "#" + new THREE.Color(a).lerp(new THREE.Color(b), t).getHexString();

	// A part of the scene (shown in its views only) ; "animate(view)" : its pose in a frame
	const group = (name, animate = null) => {
		const g = new THREE.Group();
		parts[name] = { group: g, animate };
		root.add(g);
		return g;
	};
	const add = (g, mesh, x = 0, y = 0, z = 0, noInk = false) => {
		mesh.position.set(x, y, z);
		if (noInk) mesh.userData.noInk = true;
		g.add(mesh);
		return mesh;
	};
	// A board of wood (the planks of the proposal)
	const plank = (w, h, d, color, round = 0.3, scale = 4) => S.box(w, h, d, color, round, { steps: 3, pattern: "planks", scale });
	// A faceted clump (leaves, rocks) : few large facets
	const clump = (radius, color, seed, detail = 0) => S.clump(radius, color, S.random(seed), detail);
	// A flat accent on a surface (no outline, no shadow)
	const accent = (mesh) => { mesh.userData.noInk = true; mesh.userData.noShadow = true; return mesh; };
	// A flat ellipse on the ground of an opaque tile (a glow, the ash of a fire), no outline
	const disc = (g, x, z, rx, rz, color, y = 0.002) => {
		const m = S.part(new THREE.CircleGeometry(1, 28), color, { flat: true });
		m.rotation.x = -Math.PI / 2;
		m.scale.set(rx, rz, 1);
		return add(g, accent(m), x, y, z, true);
	};
	// A cylinder (an ellipse across : "squash" its depth)
	const cylinder = (rTop, rBottom, h, color, squash = 1, segments = 20, options = {}) => {
		const m = S.part(new THREE.CylinderGeometry(rTop, rBottom, h, segments), color, options);
		m.scale.z = squash;
		return m;
	};
	// A ground of the 3 x 3 blocks around the block (0, 0) (the opaque tiles : the middle one cropped) : its painted
	// pattern repeats every block (period 1), the grass as the painted grass grounds (scenes/tiles.js)
	const plane = (g, color, pattern = "grass", scale = 1) => {
		const m = S.part(new THREE.PlaneGeometry(3, 3), color, { pattern, scale, period: 1, patches: false, steps: 3 });
		m.rotation.x = -Math.PI / 2;
		m.userData.noShadow = true;
		return add(g, m, 0, 0, 0, true);
	};
	const grassPlane = (g) => plane(g, grass, "grass");
	const each = (g, make) => {
		for (let bx = -1; bx <= 1; bx++) {
			for (let bz = -1; bz <= 1; bz++) {
				const block = new THREE.Group();
				block.position.set(bx, 0, bz);
				make(block, bx, bz);
				g.add(block);
			}
		}
	};
	// The views : an object (key), its shadows on itself ; a field (opaque, it tiles), its shadows on its ground
	const LOOK = { ink: 1.0, supersample: 4 };
	const object = (name, part = name, extra = {}) => views.push({ name, size: [48, 48], oblique: OBLIQUE, target: [0, 0], key: true, wash: 0.4, part, shadows: 2, shadowCenter: [0, 0, 0], ...LOOK, ...extra });
	const field = (name, part, extra = {}) => views.push({ name, size: [144, 144], oblique: OBLIQUE, target: [0, 0], key: false, wash: 0.6, crop: [48, 48, 48, 48], period: 48, part, shadows: 3, shadowCenter: [0, 0, 0], ...LOOK, ...extra });

	// A roof of tiles (the well) : rows of chunky rounded tiles down each slope, the seams between them, a ridge
	const tiledRoof = (parent, w, depth, rise, color, overhang) => {
		const g = new THREE.Group();
		const slope = Math.atan2(rise, depth / 2);
		const length = Math.hypot(rise, depth / 2) + overhang;
		const rows = 2;
		for (const side of [-1, 1]) {
			const half = new THREE.Group();
			half.position.set(0, rise, 0);
			half.rotation.x = side * (Math.PI / 2 - slope);
			for (let r = 0; r < rows; r++) {
				const tile = S.box(w + overhang * 2, 0.06, length / rows + 0.03, M[color][1], 0.45, { steps: 3 });
				tile.position.set(0, -r * 0.012, side * ((r + 0.5) * length / rows));
				half.add(tile);
				for (let k = -w / 2 + 0.12; k < w / 2; k += 0.2) {
					const seam = S.box(0.025, 0.065, length / rows, M[color][2], 0.4, { flat: true });
					seam.position.set(k, 0.008 - r * 0.012, side * ((r + 0.5) * length / rows));
					half.add(accent(seam));
				}
			}
			g.add(half);
		}
		add(g, S.box(w + overhang * 2 + 0.04, 0.09, 0.11, M[color][2], 0.45, { steps: 3 }), 0, rise + 0.02, 0);
		parent.add(g);
		return g;
	};

	// ---------------------------------------------------------------- the furniture

	// The bench : a seat of planks (rows 7 to 10 of the drawing), its back (rows 3 to 5), iron feet
	{
		const g = group("bench");
		add(g, plank(0.88, 0.06, 0.2, WOOD.bench, 0.3), 0, 0.25, 0.235);
		add(g, plank(0.88, 0.17, 0.05, WOOD.bench, 0.3), 0, 0.44, 0.135);
		for (const side of [-1, 1]) {
			add(g, S.box(0.06, 0.24, 0.17, M.metal[1], 0.3, { steps: 3 }), side * 0.34, 0.12, 0.24);
			add(g, S.box(0.05, 0.3, 0.04, M.metal[1], 0.3, { steps: 3 }), side * 0.34, 0.37, 0.135);
		}
		S.ink(g);
		object("obj_bench", "bench");
	}

	// The bin : a green bin, a metal lid and band
	{
		const g = group("bin");
		add(g, cylinder(0.25, 0.23, 0.42, M.roofg[1], 0.45, 20, { steps: 3 }), 0, 0.21, 0.22);
		add(g, cylinder(0.275, 0.275, 0.05, M.metal[1], 0.45, 20, { steps: 3 }), 0, 0.445, 0.22);
		add(g, S.box(0.12, 0.04, 0.035, M.metal[2], 0.4, { steps: 3 }), 0, 0.48, 0.22);
		add(g, accent(cylinder(0.252, 0.25, 0.03, M.roofg[2], 0.46, 20, { flat: true })), 0, 0.2, 0.22, true);
		S.ink(g);
		object("obj_bin", "bin");
	}

	// The mailbox : a red box on a plank post, its flag
	{
		const g = group("mailbox");
		add(g, plank(0.1, 0.52, 0.09, WOOD.post, 0.3), 0, 0.26, 0.3);
		add(g, S.box(0.52, 0.24, 0.3, M.roofr[1], 0.45, { steps: 3 }), 0, 0.6, 0.28);
		add(g, accent(S.box(0.2, 0.03, 0.01, M.roofr[2], 0.3, { flat: true })), -0.06, 0.62, 0.432, true);
		add(g, S.box(0.03, 0.17, 0.03, M.metal[1], 0.3, { steps: 3 }), 0.27, 0.74, 0.33);
		add(g, S.box(0.07, 0.06, 0.02, M.flower[1], 0.3, { steps: 3 }), 0.3, 0.79, 0.33);
		S.ink(g);
		object("obj_mailbox", "mailbox");
	}

	// The barrel : bulging planks, two metal hoops, its lid
	{
		const g = group("barrel");
		const H = 0.52;
		const profile = [];
		for (let i = 0; i <= 10; i++) profile.push(new THREE.Vector2(0.22 + 0.06 * Math.sin(Math.PI * i / 10), H * i / 10));
		const body = S.part(new THREE.LatheGeometry(profile, 24), WOOD.barrel, { steps: 3, pattern: "planks", scale: 5 });
		body.scale.z = 0.45;
		add(g, body, 0, 0, 0.21);
		add(g, cylinder(0.22, 0.22, 0.02, soft(WOOD.barrel, WOOD.cut, 0.5), 0.45, 24, { steps: 3 }), 0, H, 0.21);
		for (const y of [0.11, 0.41]) {
			const r = 0.22 + 0.06 * Math.sin(Math.PI * y / H) + 0.004;
			const hoop = S.part(new THREE.TorusGeometry(r, 0.02, 4, 24), M.metal[1], { flat: true });
			hoop.rotation.x = Math.PI / 2;
			hoop.scale.y = 0.45;
			add(g, accent(hoop), 0, y, 0.21, true);
		}
		S.ink(g);
		object("obj_barrel", "barrel");
	}

	// The bush : a round mass of leaves, two humps on its top (the faceted clumps of the proposal)
	{
		const g = group("bush");
		const [, base, deep] = tones.leaves;
		const main = clump(0.38, deep, 11, 1);
		main.scale.set(1.12, 0.9, 0.8);
		add(g, main, 0, 0.22, 0.14);
		add(g, clump(0.2, base, 12, 1), -0.17, 0.4, 0.08);
		add(g, clump(0.19, base, 13, 1), 0.18, 0.38, 0.06);
		S.ink(g);
		object("obj_bush", "bush");
	}

	// The well : a cobbled ring, water in it, two posts, a little tiled roof
	{
		const g = group("well");
		const z = 0.24;
		add(g, cylinder(0.4, 0.42, 0.22, COBBLE, 0.62, 24, { steps: 3, pattern: "cobble", scale: 3 }), 0, 0.11, z);
		add(g, accent(cylinder(0.3, 0.3, 0.01, WATER.deep, 0.62, 24, { flat: true })), 0, 0.222, z - 0.01, true);
		for (const side of [-1, 1]) add(g, plank(0.06, 0.6, 0.06, WOOD.post, 0.3), side * 0.32, 0.4, z);
		add(g, plank(0.66, 0.04, 0.04, WOOD.post, 0.3), 0, 0.6, z);
		tiledRoof(g, 0.8, 0.42, 0.13, "roofr", 0.07).position.set(0, 0.66, z);
		S.ink(g);
		object("obj_well", "well");
	}

	// The water lilies (over the water : key) : three round pads, a notch in each, a pink flower
	{
		const g = group("lily");
		for (const [cx, cy, r] of [[5, 5, 3.2], [11, 10, 2.6], [4, 12, 2.0]]) {
			const pad = S.part(new THREE.CylinderGeometry(r * U, r * U, 0.03, 28, 1, false, Math.PI / 2 + 0.28, Math.PI * 2 - 0.56), M.roofg[1], { steps: 3 });
			pad.scale.z = 0.82;
			add(g, pad, at(cx), 0.015, at(cy));
		}
		const flower = new THREE.Group();
		flower.position.set(at(5.6), 0.05, at(4.4));
		for (let i = 0; i < 5; i++) {
			const a = i * Math.PI * 2 / 5 + 0.3;
			const petal = S.blob(0.05, 0.03, 0.04, M.roofr[0]);
			petal.position.set(Math.cos(a) * 0.045, 0, Math.sin(a) * 0.04);
			flower.add(petal);
		}
		add(flower, S.blob(0.03, 0.03, 0.03, M.white[0], { flat: true }), 0, 0.025, 0, true);
		g.add(flower);
		S.ink(g, 0.8);
		object("obj_lily", "lily");
	}

	// The boulder : a pale faceted rock
	{
		const g = group("boulder");
		const rock = clump(0.4, ROCK, 5, 0);
		rock.scale.set(1.02, 0.8, 0.62);
		add(g, rock, 0, 0.25, 0.06);
		S.ink(g);
		object("obj_boulder", "boulder");
	}

	// The log to sit on : lying across the block, faceted bark, its cut end lighter
	{
		const g = group("log_seat");
		const log = cylinder(0.15, 0.15, 0.76, WOOD.stump, 1, 7, { facets: true, steps: 3 });
		log.rotation.set(Math.PI / 8, 0, Math.PI / 2);
		add(g, log, 0.02, 0.15, 0.2);
		const end = cylinder(0.125, 0.125, 0.012, WOOD.cut, 1, 7, { flat: true });
		end.rotation.set(Math.PI / 8, 0, Math.PI / 2);
		add(g, accent(end), 0.405, 0.15, 0.2, true);
		add(g, cylinder(0.04, 0.05, 0.08, WOOD.stump, 1, 6, { facets: true, steps: 3 }), -0.14, 0.31, 0.18);
		S.ink(g);
		object("obj_log_seat", "log_seat");
	}

	// The sign : a board of planks on a post
	{
		const g = group("sign");
		add(g, plank(0.1, 0.46, 0.08, WOOD.post, 0.3), 0, 0.23, 0.42);
		add(g, plank(0.76, 0.48, 0.07, WOOD.rail, 0.25), 0, 0.66, 0.42);
		S.ink(g);
		object("obj_sign", "sign");
	}

	// The crops (opaque, they tile) : two dirt furrows a block, three leaf clumps on each
	{
		const g = group("crops");
		plane(g, SOIL.ground, "dirt");
		// The furrows : across the three blocks (no seam between two tiles)
		for (let bz = -1; bz <= 1; bz++) {
			for (const z of [-0.22, 0.28]) add(g, S.box(3.2, 0.06, 0.34, SOIL.furrow, 0.45, { steps: 3, pattern: "dirt", period: 1 }), 0, 0.0, bz + z, true);
		}
		each(g, (block) => {
			for (const z of [-0.22, 0.28]) {
				[-1 / 3, 0, 1 / 3].forEach((x, i) => {
					const plant = clump(0.13, LEAF[(i + (z > 0 ? 1 : 0)) % 2], 60 + i * 7 + (z > 0 ? 3 : 0), 0);
					plant.scale.set(1.1, 0.85, 0.95);
					add(block, plant, x, 0.11, z - 0.04);
				});
			}
		});
		S.ink(g, 0.6);
		field("obj_crops", "crops", { wash: 0.5 });
	}

	// ---------------------------------------------------------------- the street lamp

	// The post on a stone foot (block (0, 0)), the lantern above it (block (0, -1)) : its glass lit by the frame, sparks
	const LIGHTS = ["#fff4a8", "#ffe48a", "#ffef98", "#f6d470"];
	let glass;
	const sparks = [];
	{
		const g = group("lamp", (view) => {
			const f = view.step ?? 0;
			const c = new THREE.Color(LIGHTS[f]);
			for (const k of ["base", "lit", "shade"]) glass.material.uniforms[k].value.copy(c);
			sparks.forEach((spark, i) => { spark.visible = (f === 0 || f === 2) && (i + f / 2) % 2 === 0; });
		});
		const z = 0.3;
		add(g, S.box(0.4, 0.09, 0.2, COBBLE, 0.4, { steps: 3 }), 0, 0.045, z);
		add(g, cylinder(0.06, 0.08, 1.55, M.metal[1], 1, 10, { steps: 3 }), 0, 0.85, z);
		add(g, S.box(0.34, 0.06, 0.22, M.metal[1], 0.4, { steps: 3 }), 0, 1.6, z);
		glass = add(g, S.box(0.28, 0.34, 0.16, LIGHTS[0], 0.25, { flat: true }), 0, 1.8, z);
		glass.userData.noShadow = true;
		for (const side of [-1, 1]) add(g, S.box(0.04, 0.34, 0.18, M.metal[2], 0.3, { steps: 3 }), side * 0.15, 1.8, z);
		add(g, S.box(0.42, 0.07, 0.26, M.metal[2], 0.4, { steps: 3 }), 0, 1.99, z);
		add(g, S.box(0.16, 0.05, 0.12, M.metal[2], 0.4, { steps: 3 }), 0, 2.04, z);
		S.ink(g);
		// The sparks : voxels (2, 4), (14, 6), (3, 9), (13, 2) of the lantern's tile, at the height of the lantern
		for (const [vx, vy] of [[2, 4], [14, 6], [3, 9], [13, 2]]) {
			const spark = S.box(0.05, 0.05, 0.05, LIGHTS[0], 0.45, { flat: true });
			add(g, accent(spark), at(vx + 0.5), 1.8, at(vy + 0.5) - 1 + 0.8 * 1.8, true);
			sparks.push(spark);
		}
		const lampView = { oblique: OBLIQUE, key: true, wash: 0.4, part: "lamp", shadows: 3, shadowCenter: [0, 1, 0], ...LOOK };
		views.push({ ...lampView, name: "lamp", size: [48, 96], target: [0, -0.5], step: 0 });
		for (const f of FRAMES) views.push({ ...lampView, name: `lamphead_${f}`, size: [48, 48], target: [0, -1], step: f });
	}

	// ---------------------------------------------------------------- the fences

	// A garden : the corners (blocks (-1, 0), (1, 0)), the side across between them (0, 0), the sides down from the corners
	// (x -1 and 1, z 1 to 3). Each view is its block cropped from the garden : the rails go on into the neighbours.
	// Plank posts and rails (the proposal)
	{
		const g = group("fence");
		const line = 0.33;                         // the foot of the fence across, in its block
		const post = (x, z, w = 0.1) => add(g, plank(w, 0.74, 0.08, WOOD.fence, 0.3), x, 0.37, z);
		post(-0.34, line); post(0.34, line);
		for (const x of [-1, 1]) post(x, line);
		for (const y of [0.22, 0.52]) add(g, plank(2.0, 0.07, 0.045, WOOD.rail, 0.3), 0, y, line + 0.01);
		for (const x of [-1, 1]) {
			for (let z = line + 0.42; z < 3.6; z += 0.5) post(x, z, 0.16);
			for (const y of [0.22, 0.52]) add(g, plank(0.075, 0.07, 3.3, WOOD.rail, 0.3), x + 0.01, y, line + 1.65);
		}
		S.ink(g);
		for (const [name, target] of [["fence", [0, 0]], ["fence_tl", [-1, 0]], ["fence_tr", [1, 0]], ["fence_v", [-1, 1]]]) {
			views.push({ name, size: [144, 144], oblique: OBLIQUE, target, key: true, wash: 0.4, crop: [48, 48, 48, 48], part: "fence", shadows: 4, shadowCenter: [0, 0, 1.5], ...LOOK });
		}
	}

	// ---------------------------------------------------------------- the flower pots

	const COLOURS = ["roofr", "flower", "roofb", "orange"];
	const blooms = {};
	{
		const g = group("flowerpot", (view) => {
			for (const [c, list] of Object.entries(blooms)) for (const b of list) b.visible = c === view.colour;
		});
		const profile = [new THREE.Vector2(0.16, 0), new THREE.Vector2(0.21, 0.2), new THREE.Vector2(0.235, 0.2), new THREE.Vector2(0.235, 0.25), new THREE.Vector2(0.18, 0.25)];
		const pot = S.part(new THREE.LatheGeometry(profile, 24), M.orange[1], { steps: 3 });
		pot.scale.z = 0.6;
		add(g, pot, 0, 0, 0.27);
		const [, base, deep] = tones.leaves;
		const leaves = clump(0.25, deep, 3, 1);
		leaves.scale.set(1.0, 0.8, 0.7);
		add(g, leaves, 0, 0.42, 0.24);
		add(g, clump(0.13, base, 4, 1), -0.05, 0.56, 0.2);
		S.ink(g);
		for (const c of COLOURS) {
			blooms[c] = [];
			for (const [x, y, z] of [[-0.14, 0.6, 0.27], [0.03, 0.7, 0.2], [0.17, 0.52, 0.3], [-0.05, 0.47, 0.38]]) {
				const flower = new THREE.Group();
				for (let i = 0; i < 4; i++) {
					const a = i * Math.PI / 2 + 0.4;
					add(flower, S.blob(0.038, 0.03, 0.038, M[c][1]), Math.cos(a) * 0.032, 0, Math.sin(a) * 0.032);
				}
				add(flower, S.blob(0.022, 0.02, 0.022, M.white[0], { flat: true }), 0, 0.02, 0, true);
				add(g, flower, x, y, z);
				S.ink(flower, 0.5);
				blooms[c].push(flower);
			}
			object(`flowerpot_${c}`, "flowerpot", { colour: c });
		}
	}

	// ---------------------------------------------------------------- the emblem on the pavement

	// A 2 x 2 round sign painted (white) : centred on the corner (0, 0) of its four blocks ; each view a quarter of it
	{
		const g = group("emblem");
		const paint = (geometry) => {
			const m = S.part(geometry, M.white[1], { flat: true });
			m.rotation.x = -Math.PI / 2;
			return add(g, m, 0, 0.003, 0, true);
		};
		paint(new THREE.RingGeometry(12.4 * U, 14 * U, 72));
		paint(new THREE.RingGeometry(3.0 * U, 4.4 * U, 40));
		for (const side of [-1, 1]) {
			const bar = paint(new THREE.PlaneGeometry(9.6 * U, 1.6 * U));
			bar.position.x = side * 7.85 * U;
		}
		for (let c = 0; c < 2; c++) {
			for (let r = 0; r < 2; r++) {
				views.push({ name: `emblem_${c}_${r}`, size: [96, 96], oblique: OBLIQUE, target: [0, 0], key: true, ink: 1.0, wash: 0.3, supersample: 4, crop: [c * 48, r * 48, 48, 48], part: "emblem" });
			}
		}
	}

	// ---------------------------------------------------------------- the animated tiles

	// The water (opaque, tiles) : the water of the pond of the proposal in two blues (a deeper pool), a lighter sheen,
	// thin white ripples that drift and swell in turn (frames 0 1 2 1 : the loop)
	{
		const anim = [];
		const g = group("water", (view) => {
			const f = view.step ?? 0;
			const k = [0, 1, 2, 1][f];
			for (const a of anim) a.apply(k);
		});
		const water = S.part(new THREE.PlaneGeometry(3, 3), WATER.light, { flat: true });
		water.rotation.x = -Math.PI / 2;
		add(g, water, 0, 0, 0, true);
		// A ripple : an arc of a thin white ring, flattened by the view from above
		const arc = (r, spread) => {
			const m = S.part(new THREE.TorusGeometry(r, 0.011, 4, 20, spread), WATER.ripple, { flat: true });
			m.rotation.x = -Math.PI / 2;
			m.rotation.z = Math.PI / 2 - spread / 2;
			return accent(m);
		};
		each(g, (block) => {
			// The deeper blue : a soft pool across the block ; the sheen : a light lens over it
			const pool = disc(block, at(9), at(11.5), 0.36, 0.05, WATER.base, 0.002);
			const sheen = disc(block, at(5), at(4.5), 0.2, 0.06, WATER.sheen, 0.004);
			anim.push({ apply: (k) => { sheen.scale.x = 0.2 + 0.03 * k; sheen.position.x = at(5) + k * U * 0.5; pool.scale.x = 0.36 + 0.02 * k; } });
			for (const [wx, wy, r] of [[11, 5, 0.1], [4, 11, 0.12], [13, 14, 0.07]]) {
				const ripple = arc(r, Math.PI * 0.75);
				add(block, ripple, at(wx), 0.006, at(wy));
				anim.push({ apply: (k) => { ripple.position.x = at(wx) + k * U; ripple.scale.set(1 + 0.12 * k, 1 + 0.12 * k, 1); } });
			}
		});
		for (const f of FRAMES) field(`water_${f}`, "water", { step: f, wash: 0.45, shadows: undefined });
	}

	// The flowers on the grass (opaque, tiles) : four flowers a block on their stems, swaying in turn
	{
		const stems = [];
		const g = group("flower", (view) => {
			const sway = [0, 1, 0, -1][view.step ?? 0];
			stems.forEach((s, i) => { s.rotation.z = -(i % 2 ? -sway : sway) * 0.32; });
		});
		grassPlane(g);
		each(g, (block) => {
			[[4, 4, "flower"], [11, 7, "white"], [5, 12, "white"], [12, 13, "flower"]].forEach(([fx, fy, m]) => {
				const z = at(fy + 2.5);
				for (const side of [-1, 1]) {
					const leaf = S.blob(0.05, 0.012, 0.022, tones.grass[2], { flat: true });
					leaf.rotation.y = side * 0.4;
					add(block, accent(leaf), at(fx + 0.5) + side * 0.04, 0.012, z, true);
				}
				const stem = new THREE.Group();
				stem.position.set(at(fx + 0.5), 0, z);
				add(stem, S.part(new THREE.CylinderGeometry(0.012, 0.014, 0.2, 5), tones.grass[2], { flat: true }), 0, 0.1, 0, true);
				const head = new THREE.Group();
				head.position.set(0, 0.2, 0);
				for (let k = 0; k < 5; k++) {
					const a = k * Math.PI * 2 / 5 + 0.2;
					add(head, S.blob(0.04, 0.02, 0.04, M[m][1]), Math.cos(a) * 0.04, 0, Math.sin(a) * 0.04);
				}
				add(head, S.blob(0.026, 0.024, 0.026, M.orange[1], { flat: true }), 0, 0.016, 0, true);
				S.ink(head, 0.45);
				stem.add(head);
				block.add(stem);
				stems.push(stem);
			});
		});
		for (const f of FRAMES) field(`flower_${f}`, "flower", { step: f, wash: 0.7 });
	}

	// The campfire (opaque, on the grass) : a ring of pale rocks, crossed logs, flames that flicker
	{
		let flames;
		const g = group("campfire", (view) => {
			const f = view.step ?? 0;
			const tall = [7, 8, 6, 8][f] * U / 0.8 / 0.62;
			flames.scale.set(1, tall, 1);
			flames.rotation.z = -[0, 1, 0, -1][f] * 0.22;
		});
		grassPlane(g);
		const cz = at(10);
		disc(g, 0, cz, 0.42, 0.27, soft(grass, "#ffe08a", 0.3));
		disc(g, 0, cz, 0.26, 0.16, "#5a4232", 0.003);
		for (let i = 0; i < 8; i++) {
			const a = 2 * Math.PI * i / 8;
			const stone = clump(0.085, ROCK, 40 + i, 0);
			stone.scale.set(1, 0.7, 0.85);
			add(g, stone, 5.2 * U * Math.cos(a), 0.04, cz + 3.4 * U * Math.sin(a));
		}
		for (const turn of [-0.45, 0.45]) {
			const log = cylinder(0.045, 0.045, 0.46, WOOD.stump, 1, 7, { facets: true, steps: 3 });
			log.rotation.set(0, turn, Math.PI / 2);
			add(g, log, 0, 0.06, cz);
		}
		S.ink(g);
		// The flames : an orange cone, a yellow one in it, a pale heart (one tone each), from their foot (scaled by the frame)
		flames = new THREE.Group();
		flames.position.set(0, 0.07, cz + 0.02);
		const tongue = (w, h, color, x, z, lean) => {
			const profile = [];
			for (let i = 0; i <= 12; i++) { const t = i / 12; profile.push(new THREE.Vector2(w * Math.sin(Math.PI * Math.sqrt(t)) + 1e-4, h * t)); }
			const m = S.part(new THREE.LatheGeometry(profile, 10), color, { flat: true });
			m.scale.z = 0.6;
			m.position.set(x, 0, z);
			m.rotation.z = lean;
			m.userData.noShadow = true;
			flames.add(m);
			return m;
		};
		tongue(0.17, 0.62, M.orange[1], 0, 0, 0);
		tongue(0.1, 0.4, M.orange[1], -0.1, 0.01, 0.35);
		tongue(0.1, 0.44, M.orange[1], 0.1, 0.01, -0.3);
		tongue(0.11, 0.42, "#ffcf3a", 0.0, 0.07, 0).userData.noInk = true;
		tongue(0.055, 0.22, "#fff6c0", 0.0, 0.12, 0).userData.noInk = true;
		S.ink(flames, 0.7);
		g.add(flames);
		for (const f of FRAMES) field(`campfire_${f}`, "campfire", { step: f, wash: 0.5 });
	}

	// The butterfly over the grass (opaque) : it flutters round, its wings open (frames 0, 2) and closed (1, 3) ; its
	// shadow on the grass under it (5 voxels lower on the screen : the drawing)
	{
		let fly, shadow;
		const wings = [];
		const g = group("butterfly", (view) => {
			const f = view.step ?? 0;
			const a = f * Math.PI / 2;
			const cx = Math.trunc(8 + 3 * Math.cos(a) + 1e-9), cy = Math.trunc(7 + 2 * Math.sin(a) + 1e-9);
			const x = at(cx + 1), z = at(cy + 5.5);
			fly.position.set(x, 5 * U / 0.8, z);
			shadow.position.set(x + 0.01, 0.002, z);
			const open = f % 2 === 0;
			shadow.scale.x = open ? 0.09 : 0.04;
			for (const w of wings) w.pivot.rotation.z = w.side * (open ? 0.25 : 1.3);
		});
		grassPlane(g);
		shadow = disc(g, 0, 0, 0.09, 0.03, soft(tones.shadow, grass, 0.3));
		fly = new THREE.Group();
		add(fly, S.blob(0.022, 0.022, 0.09, "#33301c", { flat: true }), 0, 0, 0);
		for (const side of [-1, 1]) {
			const pivot = new THREE.Group();
			const fore = S.blob(0.1, 0.012, 0.075, M.roofo[1]);
			fore.position.set(side * 0.09, 0, -0.035);
			fore.rotation.y = side * 0.35;
			pivot.add(fore);
			const hind = S.blob(0.06, 0.011, 0.05, M.flower[1]);
			hind.position.set(side * 0.06, 0, 0.05);
			pivot.add(hind);
			const spot = S.blob(0.025, 0.014, 0.02, M.white[0], { flat: true });
			spot.position.set(side * 0.11, 0.002, -0.045);
			pivot.add(accent(spot));
			fly.add(pivot);
			wings.push({ pivot, side });
		}
		fly.traverse((c) => { if (c.isMesh) c.userData.noShadow = true; });
		S.ink(fly, 0.6);
		g.add(fly);
		for (const f of FRAMES) field(`butterfly_${f}`, "butterfly", { step: f, wash: 0.6 });
	}

	// The fountain : 2 x 2 blocks around the corner (0, 0) (key) : a round basin of cobbles, its water in two blues, a
	// pillar, two jets arcing out of its top into the basin, their drops running down the arcs, rings on the water
	{
		const drops = [];
		const rings = [];
		const g = group("fountain", (view) => {
			const f = view.step ?? 0;
			for (const d of drops) d.mesh.position.copy(d.curve.getPoint((d.k + f / 4) / 4 % 1));
			rings.forEach((r, i) => { const s = r.base + ((f + i * 2) % 4) * r.grow; r.mesh.scale.set(s, s * r.squash, 1); });
		});
		const cz = 3 * U;
		const squash = 0.76;
		add(g, cylinder(0.9, 0.92, 0.17, COBBLE, squash, 40, { steps: 3, pattern: "cobble", scale: 3 }), 0, 0.085, cz);
		add(g, accent(cylinder(0.76, 0.76, 0.01, WATER.base, squash, 40, { flat: true })), 0, 0.172, cz + 0.01, true);
		add(g, accent(cylinder(0.5, 0.5, 0.01, soft(WATER.deep, WATER.base, 0.25), squash, 32, { flat: true })), 0.08, 0.174, cz + 0.06, true);
		const sheen = S.part(new THREE.CircleGeometry(1, 24), WATER.sheen, { flat: true });
		sheen.rotation.x = -Math.PI / 2;
		sheen.scale.set(0.24, 0.07, 1);
		add(g, accent(sheen), -0.38, 0.176, cz - 0.3, true);
		add(g, cylinder(0.12, 0.15, 0.86, COBBLE, 1, 14, { steps: 3 }), 0, 0.6, cz);
		add(g, S.part(new THREE.CylinderGeometry(0.26, 0.12, 0.1, 18), COBBLE, { steps: 3 }), 0, 1.0, cz);
		add(g, accent(cylinder(0.2, 0.2, 0.01, WATER.sheen, 1, 18, { flat: true })), 0, 1.051, cz, true);
		add(g, S.blob(0.05, 0.08, 0.05, WATER.sheen), 0, 1.09, cz);
		S.ink(g);
		// The jets : the arcs of the drawing (x 16 +- (1.5 + 9 t), the screen row 4 - 6 sin(pi t) + 12 t of 32 voxels)
		for (const side of [-1, 1]) {
			const points = [];
			for (let i = 0; i <= 12; i++) {
				const t = i / 12;
				const screen = (4 - 6 * Math.sin(t * Math.PI) + 12 * t - 16) * U;
				points.push(new THREE.Vector3(side * (1.5 + 9 * t) * U, (cz - screen) / OBLIQUE, cz));
			}
			const curve = new THREE.CatmullRomCurve3(points);
			const jet = S.part(new THREE.TubeGeometry(curve, 24, 0.03, 6), WATER.sheen, { flat: true });
			g.add(accent(jet));
			for (let k = 0; k < 4; k++) {
				const drop = S.blob(0.05, 0.05, 0.05, M.white[0], { flat: true });
				g.add(accent(drop));
				drops.push({ mesh: drop, curve, k });
			}
			const splash = S.part(new THREE.RingGeometry(0.75, 1, 20), M.white[0], { flat: true });
			splash.rotation.x = -Math.PI / 2;
			add(g, accent(splash), side * 10.5 * U, 0.18, cz + 0.02, true);
			rings.push({ mesh: splash, base: 0.05, grow: 0.015, squash: 0.6 });
		}
		const ripple = S.part(new THREE.RingGeometry(0.95, 1, 40), M.white[0], { flat: true });
		ripple.rotation.x = -Math.PI / 2;
		add(g, accent(ripple), 0, 0.178, cz + 0.01, true);
		rings.push({ mesh: ripple, base: 0.25, grow: 0.1, squash });
		for (let c = 0; c < 2; c++) {
			for (let r = 0; r < 2; r++) {
				for (const f of FRAMES) {
					views.push({ name: `fountain_${c}_${r}_${f}`, size: [96, 96], oblique: OBLIQUE, target: [0, 0], key: true, wash: 0.4, crop: [c * 48, r * 48, 48, 48], part: "fountain", step: f, shadows: 3, shadowCenter: [0, 0, 0], ...LOOK });
				}
			}
		}
	}

	// The flower beds (opaque, they tile) : leafy mounds on the grass, nine blossoms of a colour, some swaying in turn
	{
		const heads = {};
		const g = group("flowerbed", (view) => {
			const f = view.step ?? 0;
			for (const [c, list] of Object.entries(heads)) {
				for (const h of list) {
					h.mesh.visible = c === view.colour;
					const sway = h.i % 2 ? [0, 1, 0, -1][(f + h.i) % 4] : 0;
					h.mesh.position.x = h.x + sway * U;
				}
			}
		});
		grassPlane(g);
		const SPOTS = [[5, 3], [12, 13], [3, 13], [7, 4], [10, 8], [11, 2], [4, 2], [2, 10], [2, 8]];
		const deep = tones.leaves[2];
		each(g, (block) => {
			SPOTS.forEach(([cx, cy], i) => {
				const leaf = clump(0.13, deep, 90 + i, 0);
				leaf.scale.set(1.25, 0.45, 0.95);
				add(block, leaf, at(cx + 1), 0.02, at(cy + 1.4), true);
			});
			for (const c of COLOURS) {
				heads[c] ??= [];
				SPOTS.forEach(([cx, cy], i) => {
					const head = new THREE.Group();
					for (let k = 0; k < 4; k++) {
						const a = k * Math.PI / 2 + 0.6;
						add(head, S.blob(0.034, 0.024, 0.032, M[c][1]), Math.cos(a) * 0.03, 0, Math.sin(a) * 0.026);
					}
					add(head, S.blob(0.02, 0.02, 0.02, M.white[0], { flat: true }), 0, 0.016, 0, true);
					const x = at(cx + 1);
					add(block, head, x, 0.07, at(cy + 1.2));
					S.ink(head, 0.45);
					heads[c].push({ mesh: head, x, i });
				});
			}
		});
		for (const c of COLOURS) for (const f of FRAMES) field(`flowerbed_${c}_${f}`, "flowerbed", { step: f, colour: c, wash: 0.6 });
	}

	return {
		root,
		views,
		pose(view) {
			for (const [name, p] of Object.entries(parts)) {
				p.group.visible = name === view.part;
				if (p.group.visible && p.animate) p.animate(view);
			}
		},
	};
}
