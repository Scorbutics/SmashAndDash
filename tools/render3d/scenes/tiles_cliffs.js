// The cliffs of the generator in 3D (the design the user chose : pale faceted boulders under an overhanging grass slab,
// chunky stone steps), a view per key of tools/worldgen : `cliff_<low>_<stairs>_<foot>` (low : the 8 bits N E S W NE
// SE SW NW of the lower neighbours, as mask8 keeps them) and `foot_<ground>_<west>_<east>_<stairs>` (the block under a
// face). Each view renders the 3 x 3 blocks around its block and keeps the middle one : neighbouring tiles join.
//
// The layout of the voxel cliffs is kept (their collision masks are computed from it, tools/worldgen tile_mask) : the
// top of a plateau is drawn on its own blocks, its face starts 18 px into its last row and ends 33 px into the block
// under it (1.3125 blocks of screen : a height of 1.64 in the oblique projection, 0.8). So a plateau is modelled with its
// footprint moved south by its projected height, its top lands on its blocks. The rocks are the same on every block (a
// face continues from a tile to the next).
import { TONES } from "./tones.js";

const OBLIQUE = 0.8;
const HEIGHT = 1.64;
const DROP = HEIGHT * OBLIQUE;       // 1.3125 : the screen height of a face
const TOP_SOUTH = 18 / 48;           // the face starts there in the last row of a plateau
const SIDE = 18 / 48;                // the rock of a side : the west or east 18 px of the block
const RIM = 4 / 48;                  // the rim at the back of a plateau
// D8 of tools/worldgen : N E S W NE SE SW NW
const D8 = [[0, -1], [1, 0], [0, 1], [-1, 0], [1, -1], [1, 1], [-1, 1], [-1, -1]];

// The masks of mask8 (a diagonal only when its two sides are not set), but the empty one
function masks() {
	const out = new Map();
	for (let m = 0; m < 256; m++) {
		const bits = [];
		for (let k = 0; k < 8; k++) bits.push(((m >> k) & 1) === 1);
		for (const [k, a, b] of [[4, 0, 1], [5, 2, 1], [6, 2, 3], [7, 0, 3]]) if (bits[a] || bits[b]) bits[k] = false;
		if (bits.some((b) => b)) out.set(bits.map((b) => (b ? 1 : 0)).join(""), bits);
	}
	return [...out.values()];
}

export function build(THREE, S, options = {}) {
	const tones = TONES[options.biome ?? "meadow"];
	const root = new THREE.Group();
	const rockColor = tones.ground.rock;
	// (the body of a plateau, where it shows between the rocks : mossy earth, not a grey wall)
	const bodyColor = new THREE.Color(rockColor).lerp(new THREE.Color(tones.grass[2]), 0.5).multiplyScalar(0.92).getStyle();
	const grassTop = (shift) => S.toon(tones.ground.grass, { pattern: "grass", period: 1, patches: false, steps: 3, shift });
	const lipColor = tones.grass[2];
	const groundMaterial = (name) => {
		const pattern = name === "grass" ? "grass" : (name === "pave" ? "cobble" : "dirt");
		const scale = name === "sand" ? 2 : 1;
		return S.toon(tones.ground[name], { pattern, scale, period: 1, patches: false, steps: 3 });
	};
	// The rocks : 6 shapes, the same on every block (a seed per slot)
	const rockShapes = [];
	for (let i = 0; i < 6; i++) rockShapes.push(S.clump(1, rockColor, S.random(101 + i * 17), 0).geometry);
	// The lowest rocks : mossy, they fade into the grass
	const mossColor = new THREE.Color(rockColor).lerp(new THREE.Color(tones.ground.grass), 0.35).getStyle();
	const rock = (parent, slot, r, x, y, z, squash = 0.85, color = rockColor) => {
		const m = new THREE.Mesh(rockShapes[slot % rockShapes.length], S.toon(color, { facets: true }));
		m.scale.set(r * 1.15, r * squash, r);
		m.rotation.y = slot * 1.3;
		m.position.set(x, y, z);
		parent.add(m);
		return m;
	};

	// A clump of grass blades (the foot of a face, its lip) : "hang" the blades bend down over the edge
	const bladeGeometry = new THREE.ConeGeometry(0.035, 1, 4);
	bladeGeometry.translate(0, 0.5, 0);
	const clump = (parent, x, y, z, size, hang = false, slot = 0) => {
		const group = new THREE.Group();
		group.position.set(x, y, z);
		const blades = [[-0.5, 0.85, 0], [-0.2, 1.0, 1], [0.15, 0.95, 2], [0.45, 0.8, 1]];
		blades.forEach(([angle, length, tone], i) => {
			const blade = new THREE.Mesh(bladeGeometry, S.toon(tones.grass[tone === 2 ? 0 : tone === 1 ? 1 : 2], { facets: true, steps: 3 }));
			blade.scale.set(1, length * size, 1);
			blade.rotation.z = angle * (slot % 2 ? -1 : 1) + (hang ? Math.PI * 0.82 : 0);
			blade.rotation.x = hang ? -0.4 : 0.2;
			blade.position.x = (i - 1.5) * 0.04;
			group.add(blade);
		});
		parent.add(group);
		return group;
	};

	// A tile : "high" the 3 x 3 blocks (true : the plateau), "stairs" the block whose face has the stairs ([dx, dz],
	// null : none), "ground" the ground
	const tile = (offset, offsetZ, high, stairs, ground) => {
		const hasStairs = (dx, dz) => stairs !== null && stairs[0] === dx && stairs[1] === dz;
		const g = new THREE.Group();
		g.position.set(offset, 0, offsetZ);
		root.add(g);
		const plane = new THREE.Mesh(new THREE.PlaneGeometry(9, 9), groundMaterial(ground));
		plane.rotation.x = -Math.PI / 2;
		plane.position.set(0.5, 0, 0.5);
		plane.userData.noInk = true;
		plane.userData.noShadow = true;
		g.add(plane);
		const isHigh = (dx, dz) => (dx < -1 || dx > 1 || dz < -1 || dz > 1) ? null : high[dz + 1][dx + 1];
		for (let dz = -1; dz <= 1; dz++) {
			for (let dx = -1; dx <= 1; dx++) {
				if (!high[dz + 1][dx + 1]) continue;
				// Its neighbours lower (unknown out of the 3 x 3 : the plateau goes on)
				const low = (ox, oz) => isHigh(dx + ox, dz + oz) === false;
				const x0 = dx + (low(-1, 0) ? SIDE : 0) - (dx === -1 && !low(-1, 0) ? 1 : 0);
				const x1 = dx + 1 - (low(1, 0) ? SIDE : 0) + (dx === 1 && !low(1, 0) ? 1 : 0);
				const z0 = dz + (low(0, -1) ? RIM : 0) - (dz === -1 && !low(0, -1) ? 1 : 0);
				const z1 = dz + (low(0, 1) ? TOP_SOUTH : 1) + (dz === 1 && !low(0, 1) ? 1 : 0);
				// The plateau : its footprint moved south by its projected height (its top lands on its blocks)
				const w = x1 - x0, d = z1 - z0;
				const body = new THREE.Mesh(new THREE.BoxGeometry(w, HEIGHT - 0.14, d), S.toon(bodyColor, { facets: true }));
				body.position.set((x0 + x1) / 2, (HEIGHT - 0.14) / 2, (z0 + z1) / 2 + DROP);
				body.userData.noInk = true;
				g.add(body);
				const top = new THREE.Mesh(new THREE.BoxGeometry(w + 0.002, 0.14, d + 0.002), grassTop([0, -DROP]));
				top.position.set((x0 + x1) / 2, HEIGHT - 0.07, (z0 + z1) / 2 + DROP);
				top.userData.noInk = true;
				g.add(top);
				// The south face : two rows of boulders (2 a block), a grass lip over it ; the stairs in the middle block
				if (low(0, 1)) {
					const lip = S.box(w + 0.04, 0.12, 0.16, lipColor, 0.45, { steps: 3 });
					lip.position.set((x0 + x1) / 2, HEIGHT - 0.05, z1 + DROP + 0.02);
					g.add(lip);
					// At its foot : a band of moss on the ground (the face fades into the grass)
					const soft = new THREE.Color(tones.grass[2]).lerp(new THREE.Color(tones.ground.grass), 0.5).getStyle();
					const moss = new THREE.Mesh(new THREE.PlaneGeometry(w, 0.26), S.toon(soft, { flat: true }));
					moss.rotation.x = -Math.PI / 2;
					moss.position.set((x0 + x1) / 2, 0.004, z1 + DROP + 0.16);
					moss.userData.noInk = true;
					moss.userData.noShadow = true;
					g.add(moss);
					for (let k = Math.floor(x0 * 2); k < x1 * 2; k++) {
						const x = (k + 0.5) / 2;
						if (x < x0 + 0.05 || x > x1 - 0.05) continue;
						const slot = ((k % 2) + 2) % 2;
						const inStairs = hasStairs(dx, dz) && x > dx + 0.2 && x < dx + 0.8;
						if (inStairs) continue;
						// (three rows, out of the face : they cover it ; the lowest mossy, in tufts of grass)
						rock(g, slot, 0.4, x, 0.36, z1 + DROP + 0.04, 0.95, mossColor);
						clump(g, x - 0.18, 0, z1 + DROP + 0.34, 0.48, false, slot);
						clump(g, x + 0.14, 0, z1 + DROP + 0.4, 0.38, false, slot + 1);
						clump(g, x + 0.06, HEIGHT - 0.06, z1 + DROP + 0.02, 0.26, true, slot);
						rock(g, slot + 2, 0.34, x + 0.25, HEIGHT * 0.58, z1 + DROP - 0.02, 0.9);
						rock(g, slot + 4, 0.28, x, HEIGHT * 0.86, z1 + DROP - 0.08, 0.8);
					}
					if (hasStairs(dx, dz)) {
						// Chunky stone steps carved in the face, a cheek of rock on each side
						const steps = 5;
						for (let i = 0; i < steps; i++) {
							const rise = HEIGHT * (steps - i) / steps;
							const step = S.box(0.5, rise, 0.09, i % 2 ? tones.ground.stone : new THREE.Color(tones.ground.stone).multiplyScalar(0.92).getStyle(), 0.12, { steps: 3 });
							// (out of the face : the treads show)
							step.position.set(dx + 0.5, rise / 2, z1 + DROP + 0.02 + i * 0.09);
							g.add(step);
						}
						rock(g, 4, 0.22, dx + 0.2, 0.3, z1 + DROP + 0.12, 0.9);
						rock(g, 5, 0.22, dx + 0.8, 0.3, z1 + DROP + 0.12, 0.9);
					}
				}
				// The sides : boulders along a lower west or east, the length of the plateau
				for (const [ox, xe] of [[-1, x0], [1, x1]]) {
					if (!low(ox, 0)) continue;
					for (let k = Math.floor((z0 + DROP) * 2); k < (z1 + DROP) * 2; k++) {
						const z = (k + 0.5) / 2;
						rock(g, 2 + (((k % 2) + 2) % 2), 0.32, xe - ox * 0.06, HEIGHT * 0.5, z, 0.95);
						clump(g, xe + ox * 0.3, 0, z + 0.1, 0.3, false, k);
					}
				}
				// The rim at the back : a darker lip of grass along a lower north
				if (low(0, -1)) {
					const rim = S.box(w, 0.1, 0.1, lipColor, 0.45, { steps: 3 });
					rim.position.set((x0 + x1) / 2, HEIGHT - 0.02, z0 + DROP + 0.04);
					g.add(rim);
				}
				// An inner corner : a lower diagonal (its two sides high) : a rock in that corner
				for (const [ox, oz] of [[1, -1], [1, 1], [-1, 1], [-1, -1]]) {
					if (low(ox, oz) && !low(ox, 0) && !low(0, oz)) {
						const cx = dx + (ox > 0 ? 1 : 0), cz = dz + (oz > 0 ? 1 : 0);
						rock(g, 3, 0.2, cx, HEIGHT - 0.06, cz + DROP, 0.6);
					}
				}
			}
		}
		S.ink(g);
		return g;
	};

	const views = [];
	const groups = [];
	// (the tiles on a grid 12 units apart, near the origin : far away the renderer loses its precision)
	const view = (name, high, stairs, ground) => {
		const i = groups.length;
		const offset = (i % 13 - 6) * 12, offsetZ = (Math.floor(i / 13) - 6) * 12;
		groups.push(tile(offset, offsetZ, high, stairs, ground));
		views.push({
			name, size: [144, 144], oblique: OBLIQUE, target: [offset + 0.5, offsetZ + 0.5], key: false, ink: 1.0, wash: 0.4, supersample: 4,
			crop: [48, 48, 48, 48], period: 48, shadows: 4, shadowCenter: [offset + 0.5, 0, offsetZ + 1.5], group: i,
		});
	};
	// The cliffs : the middle block high, its neighbours lower by the mask
	for (const bits of masks()) {
		const high = [[true, true, true], [true, true, true], [true, true, true]];
		D8.forEach(([dx, dz], k) => { if (bits[k]) high[dz + 1][dx + 1] = false; });
		// A diagonal is high only if its two sides are (mask8 dropped the diagonals next to a lower side)
		for (const [dx, dz] of [[1, -1], [1, 1], [-1, 1], [-1, -1]]) {
			if (!high[1][dx + 1] || !high[dz + 1][1]) high[dz + 1][dx + 1] = false;
		}
		const code = bits.map((b) => (b ? 1 : 0)).join("");
		for (const stairs of [false, true]) {
			for (const foot of bits[2] ? [false, true] : [false]) {
				view(`cliff_${code}_${stairs ? 1 : 0}_${foot ? 1 : 0}`, high, stairs ? [0, 0] : null, "grass");
			}
		}
	}
	// The feet : the block under a face (its ground), the face going on to the west or the east
	for (const ground of ["sand", "path", "grass", "pave"]) {
		for (const west of [false, true]) {
			for (const east of [false, true]) {
				for (const stairs of [false, true]) {
					const high = [[west, true, east], [false, false, false], [false, false, false]];
					// The face above is a plateau's last row : seen from the foot, its block is the middle of the top row
					view(`foot_${ground}_${west ? 1 : 0}_${east ? 1 : 0}_${stairs ? 1 : 0}`, high, stairs ? [0, -1] : null, ground);
				}
			}
		}
	}
	return {
		root,
		views,
		pose(view) {
			groups.forEach((group, i) => { group.visible = i === view.group; });
		},
	};
}
