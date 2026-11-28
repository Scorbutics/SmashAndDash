// The painted tiles of the rooms (the inside of the houses, of the rest houses and of the Pokémon Centers : tools/worldgen
// rooms.py, Resources/Worldgen/rooms), in the projection of the map (page.html "oblique" : the ground from above, the
// heights up the screen ; 48 px a block). The rooms always use the meadow palette : every biome renders the same views.
// The look of the approved proposal (scenes/proposal.js, its room) : a plank floor, a plaster wall over a wooden
// wainscot, a window with shutters, a rug with a border, the furniture in planks ; cast shadows on the furniture.
//   room_floor_<kind>            the floors (house, rest : broad planks ; center : a checker of light stone slabs), seamless
//   room_wall_top                the top of the back wall (dark, its wooden cap at the bottom)
//   room_wall_<item>_<paper>     the face of the back wall (item None, W a window, F a picture ; paper white, wall)
//   room_rug_<n><e><s><w>        a block of the rug on the planks (1 : the rug goes on that way ; 0 : its border)
//   room_mat                     the exit mat on the planks
//   furniture_<c>_<w>_<th>_<x>   a piece of furniture (Resources/Worldgen/shapes.txt), w x th blocks on the colour key,
//                                where the voxel one stands (its collision : the drawn pixels, worldgen.py tile_mask) ;
//                                the wall pieces (S, K) one block taller, over the wall face ; no floor under it : its
//                                shadows fall on itself only
//   furniture_H_1_1_None         the healing machine on its counter (the block (2, 0) of the counter : it replaces it)
// Each model stands with the bottom of its picture on the line z = 0 (its front), its picture from z = -th to 0.
// The seamless tiles (floors, walls, rugs, mat) : their patterns repeat every block (toon "period" 1, the view "period" 48).
import { MATERIALS } from "./tones.js";

const OBLIQUE = 0.8;
// The height of the back wall : its face fills one block of the screen (1 / OBLIQUE)
const WALL = 1 / OBLIQUE;
const M = MATERIALS;

// The colours of the rooms (as the proposal)
const C = {
	floor: "#d8a466",
	slab: { light: "#eceef6", base: "#d2d3e4", joint: "#a6a4c0" },
	plaster: { white: "#f6f4ee", wall: "#f2e2c4" },
	wainscot: { white: "#9fb4c4", wall: "#b98652" },
	dark: "#3a3226",
	// The wood of the furniture : deeper and redder than the floor (they stand out of it)
	wood: "#a8703f", table: "#c08a50", deep: "#8a5a32",
	rug: M.roofr[1], border: "#f6c860",
	books: [M.roofr[1], M.roofb[1], M.roofg[1], M.orange[1]],
};

export function build(THREE, S, options = {}) {
	const root = new THREE.Group();
	const groups = {};
	const add = (name) => {
		const group = new THREE.Group();
		groups[name] = group;
		root.add(group);
		return group;
	};
	// A rounded block between two corners (x0, y0, z0) .. (x1, y1, z1)
	const block = (x0, y0, z0, x1, y1, z1, color, round = 0.25, opts = { steps: 3 }) => {
		const mesh = S.box(x1 - x0, y1 - y0, z1 - z0, color, round, opts);
		mesh.position.set((x0 + x1) / 2, (y0 + y1) / 2, (z0 + z1) / 2);
		return mesh;
	};
	const detail = (mesh) => { mesh.userData.noInk = true; return mesh; };
	const flat = { flat: true };
	// Wood in planks (the furniture : "scale" the boards a world unit / 5)
	const planked = (scale = 3) => ({ steps: 3, pattern: "planks", scale });
	const plane = (x0, z0, x1, z1, y, color, opts = flat) => {
		const mesh = S.part(new THREE.PlaneGeometry(x1 - x0, z1 - z0), color, opts);
		mesh.rotation.x = -Math.PI / 2;
		mesh.position.set((x0 + x1) / 2, y, (z0 + z1) / 2);
		mesh.userData.noInk = true;
		mesh.userData.noShadow = true;
		return mesh;
	};

	// ------------------------------------------------------------ the floors (a 3 x 3 field, the middle block kept)
	// The planks : the painted pattern of the proposal (5 broad boards a block across the screen), repeating every block
	const planks = (group, x0 = -1.5, x1 = 1.5, z0 = -1.5, z1 = 1.5) => {
		group.add(plane(x0, z0, x1, z1, 0, C.floor, { pattern: "planks", scale: 1, period: 1 }));
	};
	for (const kind of ["house", "rest"]) planks(add(`floor_${kind}`));
	{
		// The centre : 2 x 2 stone slabs a block, a checker of two light tones, their edges a little uneven (the cobbles
		// of the proposal, big and calm) ; the same 4 slabs in each block (it tiles)
		const group = add("floor_center");
		group.add(plane(-1.5, -1.5, 1.5, 1.5, 0, C.slab.joint));
		const rand = S.random(23);
		const jitter = [0, 1, 2, 3].map(() => [0, 1, 2, 3].map(() => (rand() - 0.5) * 0.025));
		for (let x = -3; x < 3; x++) {
			for (let z = -3; z < 3; z++) {
				const k = (((x % 2) + 2) % 2) * 2 + (((z % 2) + 2) % 2);
				const [a, b, c, d] = jitter[k];
				const x0 = x / 2, z0 = z / 2;
				const tone = k === 0 || k === 3 ? C.slab.light : C.slab.base;
				const slab = block(x0 + 0.022 + a, 0, z0 + 0.022 + b, x0 + 0.478 + c, 0.04, z0 + 0.478 + d, tone, 0.45,
					{ steps: 3 });
				slab.userData.noShadow = true;
				group.add(detail(slab));
			}
		}
	}

	// ------------------------------------------------------------ the back wall
	// Its face on the line z = 0.5 (the face block : the screen rows -0.5 .. 0.5, its top block -1.5 .. -0.5), a field of
	// 3 blocks across ; "item" on the middle block only. Plaster over a wooden wainscot (the proposal)
	const windowAt = (group, x, y, z, w, h, shutters) => {
		group.add(block(x - w / 2 - 0.04, y - h / 2 - 0.04, z - 0.03, x + w / 2 + 0.04, y + h / 2 + 0.04, z + 0.03, "#fff8ec", 0.3));
		group.add(detail(block(x - w / 2, y - h / 2, z - 0.02, x + w / 2, y + h / 2, z + 0.04, M.glass[1], 0.2)));
		// The light on the panes, the bar
		group.add(detail(block(x - w / 2 + 0.03, y + 0.02, z + 0.035, x - 0.04, y + h / 2 - 0.03, z + 0.045, M.glass[0], 0.3, flat)));
		group.add(detail(block(x - 0.015, y - h / 2, z + 0.03, x + 0.015, y + h / 2, z + 0.055, "#fff8ec", 0.3, flat)));
		for (const side of [-1, 1]) {
			group.add(block(x + side * (w / 2 + 0.11) - 0.065, y - h / 2 - 0.03, z - 0.01, x + side * (w / 2 + 0.11) + 0.065, y + h / 2 + 0.03, z + 0.03, shutters, 0.3, planked(3)));
		}
		// The flower box
		group.add(block(x - w / 2 - 0.02, y - h / 2 - 0.11, z, x + w / 2 + 0.02, y - h / 2 - 0.03, z + 0.1, "#a8703f", 0.3));
		for (let i = 0; i < 4; i++) {
			const f = S.blob(0.04, 0.04, 0.04, i % 2 ? "#ff6a7a" : "#ffd24a", flat);
			f.position.set(x - w / 2 + 0.07 + i * (w - 0.14) / 3, y - h / 2 - 0.02, z + 0.08);
			group.add(detail(f));
		}
	};
	const wall = (name, paper, item) => {
		const group = add(name);
		const front = 0.5;
		const plaster = block(-1.6, 0, front - 0.1, 1.6, WALL, front, C.plaster[paper], 0.02, { pattern: "plaster", scale: 1, period: 1 });
		plaster.userData.noInk = true;
		group.add(plaster);
		// The wainscot : boards, its rail on top
		const rail = 0.42;
		group.add(detail(block(-1.6, 0, front - 0.06, 1.6, rail, front + 0.04, C.wainscot[paper], 0.04, { pattern: "planks", scale: 2, period: 1 })));
		group.add(block(-1.6, rail - 0.01, front - 0.05, 1.6, rail + 0.06, front + 0.07, C.wood, 0.35));
		group.add(block(-1.6, 0, front - 0.05, 1.6, 0.07, front + 0.06, C.deep, 0.3));
		// The shade under the cap
		group.add(detail(block(-1.6, WALL - 0.1, front - 0.06, 1.6, WALL - 0.02, front + 0.006, C.plaster[paper], 0.05, { steps: 3 })));
		// The top : dark (the inside of the wall), the wooden cap along its edge
		group.add(plane(-1.6, -3, 1.6, front - 0.08, WALL - 0.01, C.dark));
		group.add(block(-1.6, WALL - 0.04, front - 0.08, 1.6, WALL + 0.04, front + 0.04, C.wood, 0.3));
		// The floor in front of it (the wash sees it at the bottom edge)
		planks(group, -1.6, 1.6, front, 2);
		if (item === "W") {
			windowAt(group, 0, 0.84, front + 0.02, 0.46, 0.4, paper === "white" ? M.teal[1] : M.roofb[1]);
		} else if (item === "F") {
			// A picture : a wooden frame, a landscape (sky, a green hill, the sun)
			const y0 = 0.58, y1 = 1.06, x = 0.25;
			group.add(block(-x, y0, front - 0.02, x, y1, front + 0.05, C.wood, 0.25));
			group.add(detail(block(-x + 0.05, y0 + 0.05, front + 0.03, x - 0.05, y1 - 0.05, front + 0.06, "#9fd6f2", 0.05, flat)));
			const hill = S.part(new THREE.CircleGeometry(0.19, 20, 0, Math.PI), M.roofg[1], flat);
			hill.scale.set(1.1, 0.55, 1);
			hill.position.set(0.04, y0 + 0.05, front + 0.065);
			group.add(detail(hill));
			const sun = S.part(new THREE.CircleGeometry(0.04, 14), "#fff2a0", flat);
			sun.position.set(-0.11, y1 - 0.14, front + 0.065);
			group.add(detail(sun));
		}
		S.ink(group, 0.8);
	};
	for (const paper of ["white", "wall"]) {
		for (const item of [null, "W", "F"]) wall(`wall_${item}_${paper}`, paper, item);
	}

	// ------------------------------------------------------------ the rug and the mat (on the planks of the house)
	const rugs = {};
	for (let m = 0; m < 16; m++) {
		const mask = [(m >> 3) & 1, (m >> 2) & 1, (m >> 1) & 1, m & 1];
		const name = `rug_${mask.join("")}`;
		const group = add(name);
		planks(group);
		// Its edges : the edge of the block (0.5) where it ends, a little inside ; on through the neighbour where it goes on
		const [n, e, s, w] = mask;
		const inset = 0.015;
		const x0 = w ? -1.5 : -0.5 + inset, x1 = e ? 1.5 : 0.5 - inset;
		const z0 = n ? -1.5 : -0.5 + inset, z1 = s ? 1.5 : 0.5 - inset;
		const rug = block(x0, 0, z0, x1, 0.03, z1, C.rug, 0.3);
		group.add(rug);
		// The border : a gold line a little inside its edges
		const band = 0.1, line = 0.045;
		group.add(detail(block(x0 + (w ? 0 : band), 0.026, z0 + (n ? 0 : band), x1 - (e ? 0 : band), 0.034, z1 - (s ? 0 : band), C.border, 0.2, flat)));
		group.add(detail(block(x0 + (w ? 0 : band + line), 0.03, z0 + (n ? 0 : band + line), x1 - (e ? 0 : band + line), 0.038, z1 - (s ? 0 : band + line), C.rug, 0.2, flat)));
		S.ink(rug, 0.8);
		rugs[name] = group;
	}
	{
		const group = add("mat");
		planks(group);
		const mat = block(-0.44, 0, -0.3, 0.44, 0.03, 0.3, M.orange[1], 0.3);
		group.add(mat);
		group.add(detail(block(-0.37, 0.026, -0.23, 0.37, 0.034, 0.23, M.orange[0], 0.2, flat)));
		group.add(detail(block(-0.33, 0.03, -0.19, 0.33, 0.038, 0.19, M.orange[1], 0.2, flat)));
		S.ink(mat, 0.8);
	}

	// ------------------------------------------------------------ the furniture (its picture : z from -th to 0)
	// The bed : its head board against the wall (top), the frame, a pillow, the blanket (its colour), a low foot board
	for (const blanket of ["roofb", "roofg", "roofr"]) {
		const group = add(`B_${blanket}`);
		group.add(block(-0.44, 0, -1.5, 0.44, 0.3, -0.06, C.wood, 0.2, planked()));
		group.add(block(-0.44, 0, -1.52, 0.44, 0.52, -1.38, C.wood, 0.3, planked()));
		group.add(detail(block(-0.4, 0.26, -1.42, 0.4, 0.37, -0.12, "#fff8ec", 0.35)));
		group.add(block(-0.31, 0.34, -1.36, 0.31, 0.47, -1.06, "#ffffff", 0.45));
		group.add(block(-0.42, 0.3, -1.0, 0.42, 0.43, -0.1, M[blanket][1], 0.4));
		group.add(detail(block(-0.425, 0.34, -1.02, 0.425, 0.44, -0.9, "#fff8ec", 0.4)));
		group.add(block(-0.46, 0, -0.14, 0.46, 0.4, -0.02, C.wood, 0.3, planked()));
		S.ink(group);
	}
	// The table : its top in planks, four legs
	const table = (name, w, th) => {
		const group = add(name);
		const x = w / 2 - 0.08, back = -th + 0.42, front = -0.06;
		for (const lx of [-x + 0.08, x - 0.08]) {
			for (const lz of [back + 0.08, front - 0.06]) group.add(block(lx - 0.05, 0, lz - 0.05, lx + 0.05, 0.3, lz + 0.05, C.deep, 0.3));
		}
		group.add(block(-x, 0.28, back, x, 0.38, front, C.table, 0.25, planked(2)));
		S.ink(group);
	};
	table("T_2_1", 2, 1);
	table("T_2_2", 2, 2);
	{
		// The stool : a round seat on three legs
		const group = add("c");
		const z = -0.26;
		for (const [lx, lz] of [[-0.13, z + 0.08], [0.13, z + 0.08], [0, z - 0.13]]) group.add(block(lx - 0.045, 0, lz - 0.045, lx + 0.045, 0.27, lz + 0.045, C.deep, 0.3));
		const seat = S.part(new THREE.CylinderGeometry(0.24, 0.22, 0.1, 24), C.table, { steps: 3 });
		seat.position.set(0, 0.3, z);
		group.add(seat);
		S.ink(group);
	}
	{
		// The plant : a broad round of leaves over an orange pot
		const group = add("P");
		const z = -0.25;
		const pot = S.part(new THREE.CylinderGeometry(0.22, 0.17, 0.3, 20), M.orange[1], { steps: 3 });
		pot.position.set(0, 0.15, z);
		group.add(pot);
		const rim = S.part(new THREE.CylinderGeometry(0.245, 0.235, 0.07, 20), M.orange[0], { steps: 3 });
		rim.position.set(0, 0.29, z);
		group.add(rim);
		const leaves = S.clump(0.3, "#6fb03a", S.random(5), 1);
		leaves.position.set(-0.05, 0.42, z);
		leaves.scale.set(1.45, 0.75, 1.0);
		group.add(leaves);
		const small = S.clump(0.17, "#86c22c", S.random(9), 1);
		small.position.set(-0.1, 0.58, z + 0.08);
		group.add(small);
		S.ink(group);
	}
	// The bookshelf : one block deep at the front of its row, tall (its top over the wall face), three shelves of books
	const shelf = (name, w) => {
		const group = add(name);
		const x = w / 2 - 0.06, back = -0.44, front = -0.03, height = 1.78;
		const carcass = block(-x, 0, back, x, height, front, C.deep, 0.08, planked(3));
		group.add(carcass);
		const rand = S.random(11 + w);
		for (const y of [0.12, 0.66, 1.2]) {
			// The recess (dark), its books
			group.add(detail(block(-x + 0.07, y, front - 0.02, x - 0.07, y + 0.46, front + 0.005, "#5a3a22", 0.05, flat)));
			let bx = -x + 0.1;
			let i = Math.floor(rand() * 4);
			while (bx < x - 0.16) {
				const bw = Math.min(x - 0.1 - bx, 0.1 + rand() * 0.04);
				const bh = 0.32 + rand() * 0.06;
				group.add(detail(block(bx, y, front - 0.12, bx + bw - 0.012, y + bh, front + 0.01, C.books[i % C.books.length], 0.2)));
				bx += bw;
				i += 1;
			}
		}
		S.ink(carcass);
	};
	shelf("S_1_2", 1);
	shelf("S_2_2", 2);
	// The kitchen : the counter (a stone top, its doors ; the oven at its right end) and the cupboards on the wall above
	const kitchen = (name, w) => {
		const group = add(name);
		const x = w / 2 - 0.03;
		const counter = block(-x + 0.02, 0, -0.42, x - 0.02, 0.8, -0.03, C.wood, 0.1, planked(3));
		group.add(counter);
		const top = block(-x, 0.76, -0.46, x, 0.86, 0.0, M.stone[1], 0.3, { steps: 3, pattern: "cobble", scale: 1.5 });
		group.add(top);
		const cupboard = block(-x + 0.02, 1.22, -0.44, x - 0.02, 1.86, -0.2, C.wood, 0.12, planked(3));
		group.add(cupboard);
		for (let d = 0; d < w * 2; d++) {
			const dx = -w / 2 + d * 0.5;
			const oven = d === w * 2 - 1;
			// The doors : panels, a knob
			group.add(detail(block(dx + 0.06, 0.08, -0.04, dx + 0.44, 0.7, -0.015, oven ? M.metal[1] : C.table, 0.15, oven ? { steps: 3 } : flat)));
			if (oven) group.add(detail(block(dx + 0.11, 0.2, -0.02, dx + 0.39, 0.5, -0.005, M.glass[2], 0.2, flat)));
			else group.add(detail(block(dx + (d % 2 ? 0.08 : 0.36), 0.52, -0.02, dx + (d % 2 ? 0.14 : 0.42), 0.58, 0.0, "#ffd24a", 0.4, flat)));
			group.add(detail(block(dx + 0.06, 1.28, -0.21, dx + 0.44, 1.8, -0.19, C.table, 0.15, flat)));
		}
		// The sink : a basin in the stone top, on the left block
		group.add(detail(block(-w / 2 + 0.16, 0.85, -0.36, -w / 2 + 0.56, 0.865, -0.1, M.metal[0], 0.3, flat)));
		S.ink(counter);
		S.ink(cupboard);
		S.ink(top);
	};
	kitchen("K_1_2", 1);
	kitchen("K_2_2", 2);
	// The healing counter (5 x 2 blocks : its top over the back row, its front over the front row) : white plaster, a
	// teal band (the roof of the Center), an orange line
	const healing = (group, machine) => {
		const height = 1.55, back = -0.6, front = -0.04;
		const counter = block(-2.46, 0, back, 2.46, height, front, "#f6f4ee", 0.06);
		group.add(counter);
		group.add(block(-2.48, height - 0.08, back - 0.02, 2.48, height + 0.02, front + 0.03, M.white[0], 0.3));
		group.add(detail(block(-2.44, 0.3, front - 0.02, 2.44, 0.5, front + 0.012, M.teal[1], 0.1, flat)));
		group.add(detail(block(-2.44, 0.5, front - 0.02, 2.44, 0.54, front + 0.014, M.orange[1], 0.1, flat)));
		group.add(detail(block(-2.44, 0.0, front - 0.02, 2.44, 0.07, front + 0.004, M.teal[2], 0.1, flat)));
		S.ink(counter);
		if (machine) {
			// The machine : a white base, the glass dome of the pokéballs, an orange light
			const z = -0.32;
			const base = block(-0.34, height, z - 0.19, 0.34, height + 0.14, z + 0.19, M.white[0], 0.35);
			group.add(base);
			const dome = S.part(new THREE.SphereGeometry(0.21, 24, 14, 0, Math.PI * 2, 0, Math.PI / 2), "#6fb4e8", { steps: 3 });
			dome.scale.set(1, 0.9, 0.8);
			dome.position.set(0, height + 0.14, z - 0.04);
			group.add(dome);
			group.add(detail(block(-0.06, height + 0.05, z + 0.19, 0.06, height + 0.11, z + 0.215, M.orange[1], 0.4, flat)));
			for (const bx of [-0.22, 0.22]) {
				const ball = S.blob(0.07, 0.07, 0.07, "#d8322a");
				ball.position.set(bx, height + 0.15, z + 0.12);
				group.add(detail(ball));
			}
			S.ink(base);
			S.ink(dome);
		}
	};
	healing(add("C_5_2"), false);
	healing(add("H"), true);

	// ------------------------------------------------------------ the views
	const views = [];
	const tile = { oblique: OBLIQUE, supersample: 4 };
	const ground = (name, part, wash = 0.4) => views.push({ name, size: [144, 144], ...tile, target: [0, 0], key: false, ink: 0.8, crop: [48, 48, 48, 48], period: 48, wash, part });
	for (const kind of ["house", "center", "rest"]) ground(`room_floor_${kind}`, `floor_${kind}`, 0.35);
	views.push({ name: "room_wall_top", size: [144, 144], ...tile, target: [0, -1], key: false, ink: 0.8, crop: [48, 48, 48, 48], period: 48, wash: 0.3, part: "wall_null_wall" });
	for (const item of [null, "W", "F"]) {
		for (const paper of ["white", "wall"]) {
			views.push({ name: `room_wall_${item === null ? "None" : item}_${paper}`, size: [144, 144], ...tile, target: [0, 0], key: false, ink: 0.8, crop: [48, 48, 48, 48], period: 48, wash: 0.35, shadows: 3, shadowCenter: [0, 0.6, 0.5], part: `wall_${item}_${paper}` });
		}
	}
	for (const name of Object.keys(rugs)) ground(`room_${name}`, name);
	ground("room_mat", "mat");
	// The furniture : its own cast shadows (no floor : they fall on itself only)
	const furniture = (name, part, w, th, x = 0, z = -th / 2) => views.push({ name, size: [w * 48, th * 48], ...tile, target: [x, z], key: true, ink: 1.5, wash: 0.4, shadows: 4, shadowCenter: [x, 0.5, z], part });
	for (const blanket of ["roofb", "roofg", "roofr"]) furniture(`furniture_B_1_2_${blanket}`, `B_${blanket}`, 1, 2);
	furniture("furniture_C_5_2_None", "C_5_2", 5, 2);
	furniture("furniture_K_1_2_None", "K_1_2", 1, 2);
	furniture("furniture_K_2_2_None", "K_2_2", 2, 2);
	furniture("furniture_P_1_1_None", "P", 1, 1);
	furniture("furniture_S_1_2_None", "S_1_2", 1, 2);
	furniture("furniture_S_2_2_None", "S_2_2", 2, 2);
	furniture("furniture_T_2_1_None", "T_2_1", 2, 1);
	furniture("furniture_T_2_2_None", "T_2_2", 2, 2);
	furniture("furniture_c_1_1_None", "c", 1, 1);
	// The machine : the block (2, 0) of the counter (its middle column, its top row)
	furniture("furniture_H_1_1_None", "H", 1, 1, 0, -1.5);
	return {
		root,
		views,
		pose(view) {
			for (const [name, group] of Object.entries(groups)) group.visible = name === view.part;
		},
	};
}
