// The painted structures of the villages (tools/worldgen stamp_makers : World.stamp), in the projection of the map
// (page.html "oblique" : the ground from above, the heights up the screen ; 48 px a block), on the colour key of the
// chipsets. The same layout as the voxel drawings of tools/tilegen (their rectangle, their door, their top row over the
// characters) : the collision of their tiles comes from the drawn pixels (worldgen tile_mask).
//   stamp_center_<style>          the Pokémon Center, 5 x 5 tiles, its sliding door in the column 2 of the row 4
//   stamp_house_<roof>_<style>    a house, 4 x 4 tiles, its door in the column 2 of the row 3
//   stamp_tent_<canvas>           a tent, 2 x 2 tiles
//   stamp_pillar_<broken>         a pillar of a ruin, 1 x 2 tiles (True : snapped)
// <style> : the style of the building (tilegen.styled) : brick walls (meadow), plank walls (autumn), plaster walls
// (desert), snow on the roofs (snow : the brick of the meadow). The look of the approved proposal (scenes/proposal.js :
// chunky rows of roof tiles, brick on a cobbled base, plank doors under an awning, shuttered windows with a flower box) ;
// the shadows fall on the structure only (no ground in a keyed tile : a shadow there would be solid pixels).
//
// The projection : a ground point (x, z) at the height y is drawn at the pixel (W / 2 + x * 48, py0 + z * 48 - y * 38.4)
// (py0 : the pixel row of the ground point (0, 0)). A slope steeper than 1.25 facing away is hidden behind its ridge :
// the back slopes of the roofs are steep, only their front slope shows (as the voxel drawings).
import { MATERIALS } from "./tones.js";

const OBLIQUE = 0.8;
const U = 48;

// The colours of the buildings (base tones : the toon lights and shades them)
const C = {
	brick: "#e9d2b0", planks: "#b87a4c", plaster: "#e8c89a", centerWall: "#f6f4ee",
	base: "#b8b0c4", step: "#c4bccd", chimney: "#c99a7a", door: "#9a5f34",
	frame: "#fff8ec", glass: MATERIALS.glass[1], flowerBox: "#a8703f", flowerA: "#ff6a7a", flowerB: "#ffd24a",
	orange: MATERIALS.orange[1], snow: "#f4f8fc", opening: "#4a2e22",
};

// A solid of triangles (flat faces), each face turned outwards (a convex solid : away from its centroid)
function solid(THREE, points, faces) {
	const c = new THREE.Vector3();
	points.forEach((p) => c.add(new THREE.Vector3(...p)));
	c.divideScalar(points.length);
	const positions = [];
	for (const face of faces) {
		for (let i = 1; i + 1 < face.length; i++) {
			let tri = [face[0], face[i], face[i + 1]].map((k) => new THREE.Vector3(...points[k]));
			const n = new THREE.Vector3().subVectors(tri[1], tri[0]).cross(new THREE.Vector3().subVectors(tri[2], tri[0]));
			const m = new THREE.Vector3().add(tri[0]).add(tri[1]).add(tri[2]).divideScalar(3);
			if (n.dot(m.sub(c)) < 0) tri = [tri[0], tri[2], tri[1]];
			for (const v of tri) positions.push(v.x, v.y, v.z);
		}
	}
	const geometry = new THREE.BufferGeometry();
	geometry.setAttribute("position", new THREE.Float32BufferAttribute(positions, 3));
	geometry.computeVertexNormals();
	return geometry;
}

export function build(THREE, S, options = {}) {
	const root = new THREE.Group();
	if (!["meadow", "autumn", "snow", "desert"].includes(options.biome ?? "meadow")) throw new Error(`tiles_village : no biome ${options.biome}`);

	// A rounded box between two corners [x0, y0, z0] .. [x1, y1, z1]
	const block = (parent, [x0, y0, z0], [x1, y1, z1], color, round = 0.2, opts = {}) => {
		const mesh = S.box(x1 - x0, y1 - y0, z1 - z0, color, round, { steps: 3, ...opts });
		mesh.position.set((x0 + x1) / 2, (y0 + y1) / 2, (z0 + z1) / 2);
		if (opts.noInk || opts.flat) mesh.userData.noInk = true;
		parent.add(mesh);
		return mesh;
	};
	// A slab on a slope : from its low edge (z0, y0) up to its high edge (z1, y1), across x0 .. x1 ; "thick" under the
	// line ; "lift" : over it (a layer on a roof)
	const slope = (parent, x0, x1, [z0, y0], [z1, y1], thick, color, { round = 0.25, lift = 0, opts = {} } = {}) => {
		const dz = z0 - z1, dy = y1 - y0;
		const length = Math.hypot(dz, dy);
		const n = [dz / length, dy / length];
		const g = new THREE.Group();
		const mesh = S.box(x1 - x0, thick, length, color, round, { steps: 3, ...opts });
		g.add(mesh);
		g.rotation.x = Math.atan2(dy, dz);
		g.position.set((x0 + x1) / 2, (y0 + y1) / 2 + n[0] * (lift - thick / 2), (z0 + z1) / 2 + n[1] * (lift - thick / 2));
		if (opts.noInk || opts.flat) mesh.userData.noInk = true;
		parent.add(g);
		return g;
	};
	// The snow : white, its shade cool (not the warm light of the toon)
	const snowy = (mesh) => {
		const u = mesh.material.uniforms;
		u.lit.value = new THREE.Color("#ffffff");
		u.base.value = new THREE.Color("#e6eef8");
		u.shade.value = new THREE.Color("#b4c6dc");
		return mesh;
	};
	// A roof of tiles on a front slope from its eaves to its ridge ([z, y] each) : rows of chunky rounded tiles, the row
	// above lying over the one below, the seams between the tiles of a row in the deep tone (scenes/proposal.js tiledRoof).
	// "width(t)" : the half width of the roof at the fraction t of the slope (a hip narrows it)
	// The tiles of a roof in their own tones : the light less washed to yellow (a blue roof stays blue), the shade deep
	const roofTone = (mesh, [light, base, deep]) => {
		const u = mesh.material.uniforms;
		u.lit.value = new THREE.Color(base).lerp(new THREE.Color(light), 0.4);
		u.base.value = new THREE.Color(base);
		u.shade.value = new THREE.Color(base).lerp(new THREE.Color(deep), 0.7);
		return mesh;
	};
	// A roof of tiles on a front slope from its eaves to its ridge ([z, y] each) : rows of chunky tiles, the row above
	// lying over the one below (its edge casts a thin shadow), the seams between the tiles of a row in the deep tone
	// (scenes/proposal.js tiledRoof). "width(t)" : the half width of the roof at the fraction t of the slope (a hip
	// narrows it : the rows are trapezoids then). Returns the height of the top row over the slope
	const STEP = 0.03;
	const tiledRoof = (parent, eave, ridge, color, width, rows = 7) => {
		const tones = MATERIALS[color];
		const at = (t) => [eave[0] + (ridge[0] - eave[0]) * t, eave[1] + (ridge[1] - eave[1]) * t];
		const dz = eave[0] - ridge[0], dy = ridge[1] - eave[1], L = Math.hypot(dz, dy);
		const n = [dy === 0 ? 0 : dz / L, dy / L];
		const hipped = width(0) !== width(1);
		for (let r = 0; r < rows; r++) {
			const t0 = r / rows, t1 = Math.min(1, (r + 1) / rows + 0.04);
			const lift = 0.07 + r * STEP;
			if (hipped) {
				// A trapezoid : its bottom face on the row below, its top face "lift" over the slope
				const p = [];
				for (const [t, k] of [[t0, lift - 0.08], [t1, lift - 0.08], [t0, lift], [t1, lift]]) {
					const [z, y] = at(t);
					for (const side of [-1, 1]) p.push([side * width(t), y + n[0] * k, z + n[1] * k]);
				}
				parent.add(roofTone(S.part(solid(THREE, p, [[0, 1, 3, 2], [4, 5, 7, 6], [0, 1, 5, 4], [2, 3, 7, 6], [0, 2, 6, 4], [1, 3, 7, 5]]), tones[1]), tones));
			} else {
				roofTone(slope(parent, -width(0), width(0), at(t0), at(t1), 0.08, tones[1], { lift, round: 0.45 }).children[0], tones);
			}
			const half = Math.min(width(t0), width(t1));
			for (let x = -Math.floor(half / 0.25) * 0.25; x <= half - 0.1; x += 0.25) {
				if (Math.abs(x) > half - 0.08) continue;
				slope(parent, x - 0.016, x + 0.016, at(t0 + 0.01), at(t1 - 0.01), 0.03, tones[2], { lift: lift + 0.008, round: 0.4, opts: { flat: true } });
			}
		}
		return 0.07 + (rows - 1) * STEP;
	};
	// A window (scenes/proposal.js windowAt) : a frame, its panes, a bar, plank shutters, a flower box. Its middle (x, y)
	// on the wall z
	const windowAt = (parent, x, y, z, w, h, shutters) => {
		block(parent, [x - w / 2 - 0.04, y - h / 2 - 0.04, z - 0.02], [x + w / 2 + 0.04, y + h / 2 + 0.04, z + 0.04], C.frame, 0.3);
		block(parent, [x - w / 2, y - h / 2, z], [x + w / 2, y + h / 2, z + 0.05], C.glass, 0.2, { noInk: true });
		block(parent, [x - 0.015, y - h / 2, z + 0.04], [x + 0.015, y + h / 2, z + 0.06], C.frame, 0.3, { flat: true });
		for (const side of [-1, 1]) {
			const sx = x + side * (w / 2 + 0.11);
			block(parent, [sx - 0.065, y - h / 2 - 0.03, z - 0.01], [sx + 0.065, y + h / 2 + 0.03, z + 0.03], shutters, 0.3, { pattern: "planks", scale: 3 });
		}
		const by = y - h / 2 - 0.07;
		block(parent, [x - w / 2 - 0.02, by - 0.04, z], [x + w / 2 + 0.02, by + 0.04, z + 0.1], C.flowerBox, 0.3);
		for (let i = 0; i < 4; i++) {
			const f = S.blob(0.04, 0.04, 0.04, i % 2 ? C.flowerA : C.flowerB, { flat: true });
			f.userData.noInk = true;
			f.position.set(x - w / 2 + 0.07 + i * (w - 0.14) / 3, by + 0.05, z + 0.07);
			parent.add(f);
		}
	};

	// ---------------------------------------------------------------- the house (4 x 4 tiles, 192 px)
	// Its front wall on z = 0, from x -1.75 to 1.75 (the pixels 12 .. 180) ; the ground (0, 0) on the row 185
	const houses = {};
	const house = (roofName, style) => {
		const g = new THREE.Group();
		const [, roof, roofDeep] = MATERIALS[roofName];
		const wallColor = style === "autumn" ? C.planks : style === "desert" ? C.plaster : C.brick;
		const pattern = style === "autumn" ? "planks" : style === "desert" ? "plaster" : "brick";
		// The cobbled base, the walls
		block(g, [-1.72, 0, -0.12], [1.77, 0.16, 0.06], C.base, 0.3, { pattern: "cobble", scale: 2.5 });
		block(g, [-1.72, 0.1, -1.4], [1.75, 2.05, 0], wallColor, style === "desert" ? 0.1 : 0.04, { pattern, scale: style === "autumn" ? 1.3 : 1 });

		// The roof : the front slope from its eaves (z 0.22, y 2.0 : the row 118) to its ridge (the row 26), in rows of
		// tiles ; the back slope steep (hidden), its ridge
		const eave = [0.22, 2.0], ridge = [-0.72, 3.1];
		slope(g, -1.9, 1.9, eave, ridge, 0.1, roofDeep, { round: 0.3 });
		slope(g, -1.9, 1.9, [-1.25, 2.4], ridge, 0.14, roofDeep, { round: 0.3 });
		const top = tiledRoof(g, eave, ridge, roofName, () => 1.9);
		roofTone(block(g, [-1.94, ridge[1] - 0.02, ridge[0] - 0.1], [1.94, ridge[1] + 0.1, ridge[0] + 0.08], roofDeep, 0.45), MATERIALS[roofName]);
		// The brick chimney behind the ridge (the pixels 138 .. 159)
		block(g, [0.88, 2.6, -0.77], [1.29, 3.86, -0.53], C.chimney, 0.15, { pattern: "brick", scale: 1.6 });

		// The window and its shutters (the roof colour), its flower box (the columns 0 - 1) ; a small one (the column 3)
		windowAt(g, -0.86, 0.98, 0, 0.56, 0.48, roof);
		windowAt(g, 1.33, 1.06, 0, 0.26, 0.28, roof);

		// The plank door (the pixels 105 .. 135 : the column 2), its knob, its awning, its stone step
		block(g, [0.19, 0.12, -0.02], [0.81, 1.24, 0.06], C.door, 0.2, { pattern: "planks", scale: 2.5 });
		const knob = S.blob(0.035, 0.035, 0.03, C.flowerB, { flat: true });
		knob.position.set(0.7, 0.62, 0.08);
		knob.userData.noInk = true;
		g.add(knob);
		roofTone(slope(g, 0.1, 0.9, [0.26, 1.36], [0, 1.46], 0.07, roof, { round: 0.4 }).children[0], MATERIALS[roofName]);
		block(g, [0.15, 0, -0.04], [0.85, 0.1, 0.22], C.step, 0.3);

		if (style === "snow") {
			// The snow on the top of the slope, its edge hanging a little over the tiles ; on the ridge, the chimney
			const at = (t) => [eave[0] + (ridge[0] - eave[0]) * t, eave[1] + (ridge[1] - eave[1]) * t];
			snowy(slope(g, -1.94, 1.94, at(0.45), at(1.0), 0.12, C.snow, { lift: top + 0.06, round: 0.5 }).children[0]);
			snowy(block(g, [-1.95, ridge[1], ridge[0] - 0.08], [1.95, ridge[1] + 0.12, ridge[0] + 0.12], C.snow, 0.5));
			snowy(block(g, [0.85, 3.84, -0.79], [1.31, 3.92, -0.51], C.snow, 0.45));
		}
		S.ink(g);
		houses[`${roofName}_${style}`] = g;
		root.add(g);
	};

	// ---------------------------------------------------------------- the Pokémon Center (5 x 5 tiles, 240 px)
	// Its front wall on z = 0, from x -2.25 to 2.25 (the pixels 12 .. 228) ; the ground (0, 0) on the row 236
	const centers = {};
	const center = (style) => {
		const g = new THREE.Group();
		const [roofLight, roof, roofDeep] = MATERIALS.teal;
		block(g, [-2.22, 0, -0.12], [2.28, 0.16, 0.06], C.base, 0.3, { pattern: "cobble", scale: 2.5 });
		block(g, [-2.22, 0.1, -0.95], [2.25, 2.95, 0], C.centerWall, 0.06, { pattern: "plaster" });
		// The teal band
		block(g, [-2.22, 0.22, -0.02], [2.25, 0.4, 0.035], roof, 0.3);
		// The windows (the pixels 30 .. 84, 156 .. 210), teal shutters
		windowAt(g, -1.31, 1.66, 0, 0.74, 0.6, roof);
		windowAt(g, 1.31, 1.66, 0, 0.74, 0.6, roof);
		// The glass doors in a teal frame (the column 2), the step
		block(g, [-0.53, 0.1, -0.03], [0.53, 1.66, 0.03], roof, 0.2);
		for (const side of [-1, 1]) block(g, [side < 0 ? -0.46 : 0.02, 0.12, -0.01], [side < 0 ? -0.02 : 0.46, 1.6, 0.05], C.glass, 0.15);
		block(g, [-0.52, 1.6, -0.02], [0.52, 1.7, 0.05], roof, 0.3);
		block(g, [-0.56, 0, -0.04], [0.56, 0.1, 0.22], C.step, 0.3);
		// The striped awning over the door (the rows 156 .. 171)
		for (let k = 0; k < 6; k++) {
			const x = -0.69 + k * 0.23;
			slope(g, x, x + 0.23, [0.28, 1.92], [0, 2.1], 0.07, k % 2 ? "#ffffff" : C.orange, { round: 0.3 });
		}

		// The hipped roof : its eaves at y 2.85 (the row 136), its ridge at the row 24, from x -1.625 to 1.625 ; the back
		// slope steep and short (the hips cut as the voxel roof : full width below the row 72) ; rows of tiles on its front
		const hw = 2.41, rh = 1.62, ye = 2.85, zf = 0.22, zb = -1.03, yr = 4.26, zr = -0.86;
		const FL = [-hw, ye, zf], FR = [hw, ye, zf], BL = [-hw, ye, zb], BR = [hw, ye, zb], RL = [-rh, yr, zr], RR = [rh, yr, zr];
		g.add(S.part(solid(THREE, [FL, FR, BL, BR, RL, RR], [[0, 1, 5, 4], [2, 3, 5, 4], [0, 2, 4], [1, 3, 5], [0, 1, 3, 2]]), roofDeep));
		// The west hip in the light, the east one in the shade (the voxel roof)
		const hipSide = (side, color) => {
			const pts = [[side * hw, ye, zf], [side * hw, ye, zb], [side * rh, yr, zr]].map(([x, y, z]) => new THREE.Vector3(x + side * 0.004, y + 0.004, z));
			const geometry = new THREE.BufferGeometry().setFromPoints(pts);
			geometry.computeVertexNormals();
			const mesh = S.part(geometry, color, { side: THREE.DoubleSide });
			mesh.userData.noInk = true;
			g.add(mesh);
		};
		hipSide(-1, roofLight);
		hipSide(1, roof);
		const top = tiledRoof(g, [zf, ye], [zr, yr], "teal", (t) => hw + (rh - hw) * t - 0.02, 8);
		roofTone(block(g, [-rh - 0.04, yr - 0.02, zr - 0.1], [rh + 0.04, yr + 0.1, zr + 0.08], roofDeep, 0.45), MATERIALS.teal);

		// The sign of the Center on the roof (the pixels 103 .. 137, the rows 90 .. 123) : an orange disc leaning back,
		// a white cross
		const sign = new THREE.Group();
		sign.position.set(0, 3.5, -0.08);
		sign.rotation.x = -0.45;
		g.add(sign);
		const disc = S.part(new THREE.CylinderGeometry(0.36, 0.36, 0.08, 28), C.orange);
		disc.rotation.x = Math.PI / 2;
		sign.add(disc);
		for (const [w, h] of [[0.36, 0.11], [0.11, 0.36]]) {
			const bar = S.box(w, h, 0.03, "#ffffff", 0.3, { flat: true });
			bar.position.z = 0.045;
			bar.userData.noInk = true;
			sign.add(bar);
		}

		if (style === "snow") {
			// The snow : the top of the roof (its hips too)
			// (lifted over the tiles along the normal of the front slope)
			const t = 0.5, dz = zf - zr, dy = yr - ye, len = Math.hypot(dz, dy), lift = top + 0.08;
			const L = (a, b) => a.map((v, i) => v + (b[i] - v) * t);
			const p = [L(FL, RL), L(FR, RR), L(BL, RL), L(BR, RR), RL, RR].map(([x, y, z]) => [x * 1.02, y + lift * dz / len, z + lift * dy / len]);
			g.add(snowy(S.part(solid(THREE, p, [[0, 1, 5, 4], [2, 3, 5, 4], [0, 2, 4], [1, 3, 5], [0, 1, 3, 2]]), C.snow)));
		}
		S.ink(g);
		centers[style] = g;
		root.add(g);
	};

	// ---------------------------------------------------------------- the tent (2 x 2 tiles, 96 px)
	// An A-frame seen from its front : its front corners on z = 0 (x +- 0.95), its back a point : the triangle of the
	// voxel tent (the rows 9 .. 87) ; the canvas folded down its middle (the west half lit, the east one in the shade), its
	// dark opening at the bottom
	const tents = {};
	const tent = (canvas) => {
		const g = new THREE.Group();
		const [light, base, deep] = MATERIALS[canvas];
		const h = 1.62, az = -0.32;
		const P = [[-0.96, 0, 0], [0.96, 0, 0], [0, 0, 0.14], [0, h, az], [0, 0, -0.75]];
		g.add(S.part(solid(THREE, P, [[0, 2, 3], [2, 1, 3], [0, 3, 4], [1, 3, 4], [0, 2, 1, 4]]), base, { facets: true }));
		const half = (side, color) => {
			const pts = [[side * 0.96, 0, 0], [0, 0, 0.14], [0, h, az]].map(([x, y, z]) => new THREE.Vector3(x, y, z + 0.004));
			const geometry = new THREE.BufferGeometry().setFromPoints(pts);
			geometry.computeVertexNormals();
			const mesh = S.part(geometry, color, { flat: true, side: THREE.DoubleSide });
			mesh.userData.noInk = true;
			g.add(mesh);
		};
		half(-1, new THREE.Color(base).lerp(new THREE.Color(light), 0.35));
		half(1, new THREE.Color(base).lerp(new THREE.Color(deep), 0.35));
		// The opening : a dark triangle at the bottom of the fold
		const apex = new THREE.Vector3(0, 0, 0.14).lerp(new THREE.Vector3(0, h, az), 0.38).add(new THREE.Vector3(0, 0, 0.012));
		const span = 0.32, zs = 0.14 * (1 - span / 0.96) + 0.012;
		const geometry = new THREE.BufferGeometry().setFromPoints([
			new THREE.Vector3(-span, 0.005, zs), new THREE.Vector3(0, 0.005, 0.152), apex,
			new THREE.Vector3(0, 0.005, 0.152), new THREE.Vector3(span, 0.005, zs), apex,
		]);
		geometry.computeVertexNormals();
		const opening = S.part(geometry, C.opening, { flat: true, side: THREE.DoubleSide });
		opening.userData.noInk = true;
		g.add(opening);
		S.ink(g);
		tents[canvas] = g;
		root.add(g);
	};

	// ---------------------------------------------------------------- the pillar (1 x 2 tiles, 48 x 96 px)
	// Its column x +- 0.22 (the pixels 12 .. 36), its capital and its base x +- 0.35 ; the ground (0, 0) on the row 88.
	// Stacked stone drums (the proposal), the broken one : two drums, a snapped top, its fallen piece at its foot
	const pillars = {};
	const DRUM = ["#e4dfec", "#cbc3d8"], SLAB = "#d8d0e2";
	const pillar = (broken) => {
		const g = new THREE.Group();
		const zc = -0.1;
		block(g, [-0.38, 0, -0.04], [0.38, 0.1, 0.1], SLAB, 0.3);
		const drums = broken ? 2 : 4;
		const tall = broken ? 0.42 : 0.435;
		for (let i = 0; i < drums; i++) {
			const drum = S.part(new THREE.CylinderGeometry(0.2, 0.22, tall, 12), DRUM[i % 2], { facets: true });
			drum.position.set(0, 0.1 + tall * (i + 0.5), zc);
			g.add(drum);
		}
		if (!broken) {
			block(g, [-0.35, 1.84, zc - 0.12], [0.35, 1.99, zc + 0.14], SLAB, 0.25);
		} else {
			// The snapped top : a broken stone on the last drum, its fallen piece
			const rand = S.random(11);
			const top = S.clump(0.17, DRUM[0], rand, 0);
			top.scale.set(1.1, 0.7, 1);
			top.position.set(-0.03, 1.02, zc);
			g.add(top);
			const piece = S.clump(0.12, DRUM[1], rand, 0);
			piece.scale.set(1.15, 0.6, 1);
			piece.position.set(0.3, 0.07, 0.06);
			g.add(piece);
		}
		S.ink(g);
		pillars[broken ? "True" : "False"] = g;
		root.add(g);
	};

	const STYLES = ["meadow", "autumn", "snow", "desert"];
	const ROOFS = ["roofr", "roofb", "roofg", "roofo"];
	for (const style of STYLES) {
		center(style);
		for (const roof of ROOFS) house(roof, style);
	}
	for (const canvas of ["roofo", "roofg", "roofb", "roofr"]) tent(canvas);
	pillar(false);
	pillar(true);

	// The target : the ground point at the middle of the picture, the ground (0, 0) on the row py0 ; the shadows of the
	// structure on itself (the roof on the walls, the awning on the door)
	const view = (name, w, h, py0, part, key) => ({
		name, size: [w, h], oblique: OBLIQUE, target: [0, -(py0 - h / 2) / U], key: true, ink: 1.2, wash: 0.4, supersample: 4, part, partKey: key,
		shadows: 4, shadowCenter: [0, 1.5, -0.4],
	});
	const views = [];
	for (const style of STYLES) views.push(view(`stamp_center_${style}`, 240, 240, 236, "center", style));
	for (const roof of ROOFS) for (const style of STYLES) views.push(view(`stamp_house_${roof}_${style}`, 192, 192, 185, "house", `${roof}_${style}`));
	for (const canvas of ["roofo", "roofg", "roofb", "roofr"]) views.push(view(`stamp_tent_${canvas}`, 96, 96, 86, "tent", canvas));
	for (const broken of ["False", "True"]) views.push(view(`stamp_pillar_${broken}`, 48, 96, 88, "pillar", broken));

	const groups = { center: centers, house: houses, tent: tents, pillar: pillars };
	return {
		root,
		views,
		pose(v) {
			for (const [part, map] of Object.entries(groups)) {
				for (const [key, group] of Object.entries(map)) group.visible = v.part === part && v.partKey === key;
			}
		},
	};
}
