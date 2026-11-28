// The painted tiles of a biome (options.biome), in the projection of the map (page.html "oblique" : the ground from above,
// the heights up the screen ; 48 px a block), on the colour key of the chipsets. Written in
// Resources/Worldgen/painted/<biome> by tools/worldgen/paint.sh ; the generator uses them instead of the voxel drawings
// (tools/tilegen painted()). The same geometry in every biome, only the colours change (as the voxel drawings : the
// blocks between two biomes blend the two, pixel by pixel).
//   tree_round   the broadleaf tree (tools/worldgen TREE_SHAPES "round") : 3 x 4 tiles, its canopy over the rows 0 to 2,
//                its trunk in the middle column of the row 3, where the voxel tree stands (its collision mask)
//   tall_<f>     the tall grass, 4 frames (the blades sway) : a tile that makes one field with its neighbours
import { build as tree } from "./tree.js";
import { TONES } from "./tones.js";

const OBLIQUE = 0.8;


// A tuft of tall grass on a block, as the clumps of the proposal : a few broad blades fanning out from one root, each
// folded along its vein (its two halves lit differently : the toon tones), bent outwards at its tip. The blades lie
// across the screen (x, y), a little apart in depth so they overlap cleanly. "tones" : light, base, deep
// The tuft of the tall grass : 6 broad blades, a softer fold (calm : few lines, two tones)
const TALL = [
	[-58, 0.78, 0.17, 2, -0.04], [56, 0.8, 0.17, 2, -0.03], [-30, 1.02, 0.19, 1, -0.01], [28, 1.05, 0.19, 1, 0.0],
	[-6, 1.18, 0.2, 1, 0.01], [12, 0.96, 0.18, 0, 0.03],
];

function tuft(THREE, S, tones, given = null, fold = 0.7) {
	const specs = given ?? [
		// angle (degrees from up, + to the right), length, width, tone, depth
		[-62, 0.66, 0.12, 2, -0.06], [62, 0.7, 0.12, 2, -0.05], [-36, 0.95, 0.135, 1, -0.02], [34, 0.98, 0.135, 1, -0.01],
		[-12, 1.15, 0.14, 1, 0.0], [10, 1.08, 0.14, 0, 0.02], [-50, 0.74, 0.12, 1, 0.04], [48, 0.78, 0.12, 2, 0.05],
	];
	const blades = [];
	for (const [angle, length, width, tone, depth] of specs) {
		const a = angle * Math.PI / 180;
		const rows = 6;
		const positions = [];
		const index = [];
		for (let r = 0; r <= rows; r++) {
			const t = r / rows;
			// Along a curve : up the angle, the tip bending further out
			const bendAngle = a * (1 + 0.55 * t);
			const cx = Math.sin(a) * length * t * 0.9 + Math.sin(bendAngle) * length * t * t * 0.25;
			const cy = Math.cos(a) * length * t * 0.95 - (Math.abs(a) > 0.5 ? 0.12 * t * t * length : 0);
			const w = width * Math.sin(Math.PI * Math.min(0.97, 0.12 + t * 0.88)) * (1 - 0.35 * t);
			const nx = Math.cos(bendAngle), ny = -Math.sin(bendAngle);
			// Left edge, the vein (raised towards the camera), right edge
			positions.push(cx - nx * w, cy - ny * w, 0, cx, cy, w * fold, cx + nx * w, cy + ny * w, 0);
			if (r < rows) {
				const i = r * 3;
				index.push(i, i + 1, i + 3, i + 1, i + 4, i + 3, i + 1, i + 2, i + 4, i + 2, i + 5, i + 4);
			}
		}
		const geometry = new THREE.BufferGeometry();
		geometry.setAttribute("position", new THREE.Float32BufferAttribute(positions, 3));
		geometry.setIndex(index);
		geometry.computeVertexNormals();
		blades.push({ geometry, color: tones[tone], depth, length });
	}
	return blades;
}

export function build(THREE, S, options = {}) {
	const root = new THREE.Group();
	const tones = TONES[options.biome ?? "meadow"];
	if (!tones) throw new Error(`tiles : no biome ${options.biome}`);

	// The tree : on its block (0, 0), its shadow on the grass around its foot
	const treeGroup = new THREE.Group();
	const built = tree(THREE, S, { seed: Number(options.seed ?? 21), height: 3.1, leaves: tones.leaves });
	built.root.traverse((child) => {
		if (child.isMesh && !child.userData.hull && child.geometry.type === "CylinderGeometry") child.material = S.toon(tones.trunk);
	});
	const t = built.root;
	// Its canopy centred over its trunk (the trunk stays on its block : its collision mask)
	const box = new THREE.Box3().setFromObject(built.canopy);
	built.canopy.position.x -= (box.min.x + box.max.x) / 2;
	// Its canopy spread over the 3 columns of its tiles (the broadleaf of the old games), a little lower
	t.scale.set(1.15, 0.8, 1.05);
	treeGroup.add(t);
	const shadow = S.part(new THREE.CircleGeometry(1, 24), tones.shadow, { flat: true });
	shadow.rotation.x = -Math.PI / 2;
	shadow.scale.set(1.15, 0.42, 1);
	shadow.position.set(0.05, 0.002, 0.05);
	shadow.userData.noInk = true;
	treeGroup.add(shadow);
	root.add(treeGroup);

	// The tall grass : the same tuft on the 3 x 3 blocks around the block (0, 10) : the middle block, cropped, tiles
	const field = new THREE.Group();
	field.position.z = 10;
	const blades = tuft(THREE, S, tones.grass, TALL, 0.35);
	const swaying = [];
	for (let bx = -1; bx <= 1; bx++) {
		for (let bz = -1; bz <= 1; bz++) {
			// A tuft a block, broad : it reaches its neighbours (a field, calm)
			for (const [tx, tz, scale, mirror] of [[0, 0.12, 1.0, 1]]) {
				// Its root : a little shadow on the ground
				// (half way between the ground and the shadow : a soft one, no dark band along the rows)
				const soft = new THREE.Color(tones.shadow).lerp(new THREE.Color(tones.ground.grass), 0.5);
				const shade = S.part(new THREE.CircleGeometry(1, 20), soft, { flat: true });
				shade.rotation.x = -Math.PI / 2;
				shade.scale.set(0.36 * scale, 0.11 * scale, 1);
				shade.position.set(bx + tx, 0.001, bz + tz + 0.04);
				shade.userData.noInk = true;
				field.add(shade);
				for (const b of blades) {
					const pivot = new THREE.Group();
					pivot.position.set(bx + tx, 0, bz + tz + b.depth);
					pivot.scale.set(scale * mirror, scale, scale);
					const mesh = S.part(b.geometry, b.color, { facets: true, side: THREE.DoubleSide });
					pivot.add(mesh);
					pivot.userData.length = b.length * scale;
					field.add(pivot);
					swaying.push(pivot);
				}
			}
		}
	}
	S.ink(field, 0.5);

	// The pines and the cactus (tools/worldgen TREE_SHAPES : 2 tiles wide, 3 rows : the tip, the canopy, the trunk) : on
	// the block (0, 30), their trunk on the line between their two columns, where the voxel ones stand (their collision)
	const conifers = {};
	const conifer = (name) => {
		const group = new THREE.Group();
		group.position.set(0, 0, 30);
		const shade = S.part(new THREE.CircleGeometry(1, 24), tones.shadow, { flat: true });
		shade.rotation.x = -Math.PI / 2;
		shade.scale.set(0.62, 0.24, 1);
		shade.position.set(0.02, 0.002, 0.04);
		shade.userData.noInk = true;
		group.add(shade);
		conifers[name] = group;
		root.add(group);
		return group;
	};
	const [light, base, deep] = tones.conifer;
	for (const snowy of [false, true]) {
		const pine = conifer(snowy ? "snowpine" : "pine");
		const trunk = S.part(new THREE.CylinderGeometry(0.08, 0.11, 0.6, 7), tones.trunk);
		trunk.position.y = 0.3;
		pine.add(trunk);
		// The tiers : faceted cones, smaller upwards, a little turned each
		const tiers = [[0.58, 0.88, 0.56], [0.48, 0.78, 0.98], [0.37, 0.68, 1.4], [0.25, 0.6, 1.8]];
		tiers.forEach(([radius, height, y], i) => {
			const cone = S.part(new THREE.ConeGeometry(radius, height, 7), i === 3 ? base : (i % 2 ? base : deep), { facets: true, steps: 3 });
			cone.position.y = y + height / 2;
			cone.rotation.y = i * 0.45;
			pine.add(cone);
			if (snowy) {
				// Its snow : a white cap on the upper part of the tier
				const cap = S.part(new THREE.ConeGeometry(radius * 0.86, height * 0.78, 7), "#f4f8fc", { facets: true, steps: 3 });
				cap.position.y = y + height - height * 0.78 / 2 + 0.012;
				cap.rotation.y = i * 0.45;
				pine.add(cap);
			}
		});
	}
	{
		const cactus = conifer("cactus");
		const column = (radius, height) => S.part(new THREE.CylinderGeometry(radius, radius, height, 8), base, { facets: true, steps: 3 });
		const top = (radius) => S.part(new THREE.SphereGeometry(radius, 8, 5, 0, Math.PI * 2, 0, Math.PI / 2), base, { facets: true });
		const body = column(0.2, 1.55);
		body.position.y = 0.78;
		cactus.add(body);
		const cap = top(0.2);
		cap.position.y = 1.55;
		cactus.add(cap);
		// The arms : out, then up (the left one lower)
		for (const [side, y, up] of [[-1, 0.62, 0.42], [1, 0.9, 0.36]]) {
			const out = S.part(new THREE.CylinderGeometry(0.1, 0.1, 0.3, 8), base, { facets: true });
			out.rotation.z = Math.PI / 2;
			out.position.set(side * 0.3, y, 0);
			cactus.add(out);
			const arm = column(0.1, up);
			arm.position.set(side * 0.42, y + up / 2 - 0.05, 0);
			cactus.add(arm);
			const end = top(0.1);
			end.position.set(side * 0.42, y + up - 0.05, 0);
			cactus.add(end);
		}
		// A small flower on its top, the ribs darker (a stripe on its front)
		const flower = S.box(0.12, 0.06, 0.12, "#f4728c", 0.45);
		flower.position.y = 1.72;
		cactus.add(flower);
		const rib = S.box(0.03, 1.2, 0.02, deep, 0.4, { flat: true });
		rib.position.set(0.06, 0.8, 0.2);
		rib.userData.noInk = true;
		cactus.add(rib);
	}
	for (const group of Object.values(conifers)) S.ink(group);

	// The grounds : on the 3 x 3 blocks around the block (0, 20), the same on each (they tile) ; the middle block cropped.
	// The grass in 4 variants (as tools/tilegen GRASS_VARIANTS : plain, two tufts, a tuft, a tuft elsewhere), the path
	// worn, the sand, the paving in cobbles
	const grounds = {};
	// (painted patterns, seamless : they repeat every block ; the big patches of the grass are painted by the game
	// across the map, Shaders/paper)
	// (shift : the pattern of a variant moved, the variants of the grass differ)
	const ground = (name, color, pattern = "none", scale = 1, shift = [0, 0]) => {
		const group = new THREE.Group();
		group.position.z = 20;
		const plane = S.part(new THREE.PlaneGeometry(3, 3), color, pattern === "none" ? { flat: true } : { pattern, scale, period: 1, patches: false, steps: 3, shift });
		plane.rotation.x = -Math.PI / 2;
		plane.userData.noInk = true;
		group.add(plane);
		grounds[name] = group;
		root.add(group);
		return group;
	};
	const each = (group, make) => {
		for (let bx = -1; bx <= 1; bx++) {
			for (let bz = -1; bz <= 1; bz++) {
				const part = make();
				part.position.x += bx;
				part.position.z += bz;
				group.add(part);
			}
		}
	};
	// A small tuft : the tuft of the tall grass, small (no ink : it is part of the ground)
	const smallBlades = tuft(THREE, S, tones.grass);
	const smallTuft = (x, z, flip) => {
		const group = new THREE.Group();
		group.position.set(x, 0, z);
		group.scale.set(flip ? -0.26 : 0.26, 0.26, 0.26);
		for (const b of smallBlades) {
			const mesh = S.part(b.geometry, b.color, { facets: true, side: THREE.DoubleSide });
			mesh.position.z = b.depth;
			mesh.userData.noInk = true;
			group.add(mesh);
		}
		return group;
	};
	const tufts = [[], [[-0.25, -0.15, false]], [], [[0.15, 0.2, true]]];
	tufts.forEach((list, variant) => {
		const g = ground(`grass_${variant}`, tones.ground.grass, "grass", 1, [variant * 0.4, variant * 0.6]);
		each(g, () => {
			const block = new THREE.Group();
			for (const [x, z, flip] of list) block.add(smallTuft(x, z, flip));
			return block;
		});
	});
	const pebbles = (group) => each(group, () => {
		const block = new THREE.Group();
		for (const [x, z, r] of [[-0.28, -0.3, 0.05], [0.22, 0.05, 0.04], [-0.05, 0.32, 0.035]]) {
			const stone = S.clump(r, tones.ground.pebble, S.random(Math.round((x + 1) * 100)), 0);
			stone.position.set(x, r * 0.4, z);
			stone.userData.noInk = true;
			block.add(stone);
		}
		return block;
	});
	ground("path", tones.ground.path, "dirt");
	ground("sand", tones.ground.sand, "dirt", 2);
	// The rock and the stone (the textures of the voxel drawings repainted : tools/worldgen repaint)
	ground("rock", tones.ground.rock, "cobble", 2);
	ground("stone", tones.ground.stone, "cobble", 2);
	ground("pave", tones.ground.pave, "cobble");
	root.add(field);
	S.ink(treeGroup);

	const views = [
		{ name: "tree_round", size: [144, 192], oblique: OBLIQUE, target: [0, -1.5], key: true, ink: 1.0, wash: 0.8, supersample: 4, part: "tree" },
	];
	// (96 x 144 : the 3 rows placed ; the trunk base 30 px into the bottom row, on the middle line : the ground point
	// (0, 30) 54 px under the middle of the picture)
	for (const name of ["pine", "snowpine", "cactus"]) {
		views.push({ name: `tree_${name}`, size: [96, 144], oblique: OBLIQUE, target: [0, 30 - 54 / 48], key: true, ink: 1.0, wash: 0.5, supersample: 4, part: "conifer", conifer: name });
	}
	for (const name of ["grass_0", "grass_1", "grass_2", "grass_3", "path", "sand", "pave", "rock", "stone"]) {
		views.push({ name: `ground_${name}`, size: [144, 144], oblique: OBLIQUE, target: [0, 20], key: false, ink: 0.8, supersample: 4, crop: [48, 48, 48, 48], period: 48, part: "ground", ground: name, wash: name.startsWith("grass") ? 0.7 : 0.6 });
	}
	for (let f = 0; f < 4; f++) {
		views.push({ name: `tall_${f}`, size: [144, 144], oblique: OBLIQUE, target: [0, 10], key: true, ink: 1.0, wash: 0.15, supersample: 4, crop: [48, 48, 48, 48], part: "grass", step: f });
	}
	return {
		root,
		views,
		pose(view) {
			treeGroup.visible = view.part === "tree";
			field.visible = view.part === "grass";
			for (const [name, group] of Object.entries(grounds)) group.visible = view.part === "ground" && view.ground === name;
			for (const [name, group] of Object.entries(conifers)) group.visible = view.part === "conifer" && view.conifer === name;
			// The tips sway with the frames (0, 1, 0, -1), the rows the other way in turn
			const sway = [0, 1, 0, -1][view.step ?? 0];
			swaying.forEach((pivot) => { pivot.rotation.z = -sway * 0.09 * pivot.userData.length; });
		},
	};
}
