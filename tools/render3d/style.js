// The look of the pre-rendered assets, shared by every scene : the place to change the identity of the game.
//   toon   3 flat tones per material (light, base, shade) and a paper highlight : a watercolour wash, not a gradient.
//          The light picks the tone : lit from the top left, in front (as the tiles : tools/tilegen). The shadows are
//          cool (the colour times a blue grey), the lights warm and washed towards the paper.
//   ink    an outline drawn around each object (its back faces pushed out, a constant width in pixels), warm dark
//   wash   a pass over the whole picture : the edges of the colours wobble a little (a brush), the pigment pools at
//          the edges of the shapes (darker), the grain of the paper, the whites lifted (the light of the references)
import * as THREE from "three";
import { mergeVertices } from "three/addons/utils/BufferGeometryUtils.js";
import { RoundedBoxGeometry } from "three/addons/geometries/RoundedBoxGeometry.js";

// The colours are written as they are shown (no conversion to linear light : a painting, not a photo)
THREE.ColorManagement.enabled = false;

// The world : x right, y up, z towards the camera of the front view. The light comes from the top left, in front
export const LIGHT = new THREE.Vector3(-0.5, 0.85, 0.55).normalize();
export const INK = new THREE.Color("#33301c");
export const PAPER = new THREE.Color("#fdfaee");
// The tint of the shadows (multiplied) and of the lights (mixed towards it)
// (After Cube World : the shadows deep and still saturated, towards a blue green, not a grey ; the lights towards a
// sunny lime yellow)
const SHADE = new THREE.Color(0.56, 0.66, 0.74);
const WARM = new THREE.Color("#fff3a0");

// The palette of the references (pastel, washed : yellow greens, creams, a pale sky)
export const PAL = {
	leaf: "#86c22c", leafDeep: "#4e8f1c", leafPale: "#c8e03c",
	// The other canopies of the forests : golden, orange (a tree has one of them)
	gold: "#f0c830", goldDeep: "#c99418", goldPale: "#f8e468",
	orange: "#ec9430", orangeDeep: "#c0601c", orangePale: "#f6be5c",
	grass: "#92c832", grassDeep: "#5a9620",
	ground: "#a8d23e", groundPale: "#c6e252", groundDeep: "#86b82c", path: "#eab45a", dirt: "#c98a44",
	trunk: "#7c5a3a", bark: "#5e4430",
	sky: "#78b6ec",
	yellow: "#f8d94c", yellowDeep: "#e2b52f", red: "#e8463c", cheek: "#ee5a46", black: "#2c2a26", white: "#fbf8ef",
	skin: "#f7d9bd", hair: "#4a3a32", blue: "#5a86c8", denim: "#4f74b5", shoe: "#d9423b", jacket: "#e04a3f",
};

const vertex = /* glsl */`
	#include <common>
	#include <shadowmap_pars_vertex>
	varying vec3 vNormal;
	varying vec3 vWorld;
	void main() {
		#include <beginnormal_vertex>
		#include <defaultnormal_vertex>
		#include <begin_vertex>
		#include <project_vertex>
		#include <worldpos_vertex>
		#include <shadowmap_vertex>
		vec4 world = modelMatrix * vec4(position, 1.0);
		vWorld = world.xyz;
		vNormal = normalize(mat3(modelMatrix) * normal);
	}`;

// The painted patterns of a material (world space, "pattern") : what the references paint on their surfaces
export const PATTERN = { none: 0, grass: 1, dirt: 2, brick: 3, plaster: 4, planks: 5, cobble: 6 };

const fragment = /* glsl */`
	#include <common>
	#include <packing>
	#include <lights_pars_begin>
	#include <shadowmap_pars_fragment>
	#include <shadowmask_pars_fragment>
	uniform vec3 base;
	uniform vec3 lit;
	uniform vec3 shade;
	uniform vec3 paper;
	uniform vec3 light;
	uniform float facets;
	uniform float steps;
	uniform float pattern;
	uniform float scale;
	// The pattern repeats every "period" world units (0 : never ; 1 : a tile that tiles), "patches" : the big sun and
	// shade patches of the grass (off on a tile : the shader of the game paints them across the map, Shaders/paper)
	uniform float period;
	uniform float patches;
	// The pattern moved (world units) : a surface drawn away from where it lies (the top of a cliff, shifted by its height)
	uniform vec2 shift;
	varying vec3 vNormal;
	varying vec3 vWorld;

	float h21(vec2 p) { return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453); }
	float vnoise(vec2 p) {
		vec2 i = floor(p), f = fract(p);
		vec2 u = f * f * (3.0 - 2.0 * f);
		return mix(mix(h21(i), h21(i + vec2(1, 0)), u.x), mix(h21(i + vec2(0, 1)), h21(i + vec2(1, 1)), u.x), u.y);
	}
	float fbm(vec2 p) { return vnoise(p) * 0.6 + vnoise(p * 2.1 + 7.3) * 0.3 + vnoise(p * 4.3 + 2.1) * 0.1; }
	// The same, periodic : "f" the cells of a world unit (whole), their lattice wraps every "period" units
	float cells(float f) { return period > 0.0 ? f * period * scale : 0.0; }
	float hw(vec2 c, float n) { return h21(n > 0.5 ? mod(c, n) : c); }
	float vnoiseW(vec2 p, float n) {
		vec2 i = floor(p), f = fract(p);
		vec2 u = f * f * (3.0 - 2.0 * f);
		return mix(mix(hw(i, n), hw(i + vec2(1, 0), n), u.x), mix(hw(i + vec2(0, 1), n), hw(i + vec2(1, 1), n), u.x), u.y);
	}
	float fbmW(vec2 p, float n) { return vnoiseW(p, n) * 0.6 + vnoiseW(p * 2.0 + 7.0, n * 2.0) * 0.3 + vnoiseW(p * 4.0 + 2.0, n * 4.0) * 0.1; }

	// The tone of a light (d : the light on the surface, -1 .. 1) : 0 shade, 1 base, 2 lit
	float band(float d) { return d > 0.55 ? 2.0 : (d > 0.05 ? 1.0 : 0.0); }
	vec3 toneOf(float t) { return t >= 1.5 ? lit : (t >= 0.5 ? base : shade); }

	void main() {
		// The side of the triangle seen (a double sided part seen from behind : its normal the other way), whatever the
		// projection (the oblique one of the map does not look along its camera position) ; facets (foliage, rocks) : the
		// normal of the triangle, oriented as the smooth one
		vec3 n = normalize(vNormal);
		if (!gl_FrontFacing) n = -n;
		if (facets > 0.5) {
			vec3 f = normalize(cross(dFdx(vWorld), dFdy(vWorld)));
			n = dot(f, n) < 0.0 ? -f : f;
		}
		float d = dot(n, light);
		// A cast shadow (a light in the scene : page.html, view.shadows) : the shade tone
		float lightness = getShadowMask();
		if (lightness < 0.5) d = min(d, 0.0);
		float t = band(d);
		vec3 tint = vec3(1.0);
		vec2 g = (vWorld.xz + shift) * scale;
		if (pattern > 0.5 && pattern < 1.5) {
			// Grass : big patches of sun and of shade (a band up or down), short brush strokes of a lighter or darker green
			if (patches > 0.5) {
				float sunny = fbmW(g, cells(1.0));
				if (sunny > 0.66) t = min(2.0, t + 1.0);
				if (sunny < 0.28) t = max(0.0, t - 1.0);
			}
			vec2 cell = floor(g * 5.0);
			vec2 local = fract(g * 5.0) - 0.5;
			float r = hw(cell, cells(5.0));
			vec2 offset = vec2(hw(cell + 3.0, cells(5.0)), hw(cell + 5.0, cells(5.0))) - 0.5;
			vec2 q = local - offset * 0.5;
			vec2 dir = normalize(vec2(0.35 + r * 0.3, 1.0));
			float along = dot(q, dir), across = dot(q, vec2(dir.y, -dir.x));
			// (a stroke : tapered, wider at its root)
			float width = 0.075 * (1.0 - (along + 0.3) / 0.6 * 0.7);
			if (r > 0.3 && abs(across) < width && along > -0.3 && along < 0.3) tint = r > 0.68 ? vec3(1.18, 1.14, 0.9) : vec3(0.78, 0.86, 0.84);
		} else if (pattern > 1.5 && pattern < 2.5) {
			// Dirt : soft worn bands, a few darker pebbles
			float worn = fbmW(g, cells(1.0));
			tint = vec3(0.94 + 0.12 * step(0.55, worn));
			vec2 cell = floor(g * 9.0);
			if (hw(cell, cells(9.0)) > 0.9 && length(fract(g * 9.0) - 0.5) < 0.18) tint = vec3(0.78, 0.74, 0.72);
		} else if (pattern > 2.5 && pattern < 3.5) {
			// Bricks on a wall : rows (the height), staggered, each its tone ; the mortar in the shade
			float along = (abs(n.x) > abs(n.z) ? vWorld.z : vWorld.x) * scale;
			float row = floor(vWorld.y * scale / 0.13);
			float x = along / 0.28 + mod(row, 2.0) * 0.5;
			vec2 brick = vec2(floor(x), row);
			float mortar = step(fract(vWorld.y * scale / 0.13), 0.14) + step(fract(x), 0.06);
			float k = h21(brick);
			tint = k < 0.25 ? vec3(0.88, 0.9, 0.96) : (k > 0.8 ? vec3(1.1, 1.06, 0.98) : vec3(1.0));
			if (mortar > 0.5) { t = max(0.0, t - 1.0); tint = vec3(0.92); }
		} else if (pattern > 3.5 && pattern < 4.5) {
			// Plaster : soft blotches, a few bricks showing through
			vec2 w = vec2((abs(n.x) > abs(n.z) ? vWorld.z : vWorld.x), vWorld.y) * scale;
			float blot = fbmW(w * 2.0, cells(2.0));
			tint = vec3(0.95 + 0.08 * blot);
			if (fbmW(w + 4.0, cells(1.0)) > 0.72) tint *= vec3(0.9, 0.84, 0.8);
		} else if (pattern > 4.5 && pattern < 5.5) {
			// Planks : boards across, a dark joint, a grain
			float across = (abs(n.y) > 0.5 ? vWorld.z : vWorld.y) * scale;
			float alongP = (abs(n.x) > abs(n.z) && abs(n.y) < 0.5 ? vWorld.z : vWorld.x) * scale;
			float board = floor(across / 0.2);
			float joint = step(fract(across / 0.2), 0.07);
			float grain = vnoiseW(vec2(alongP * 2.0, across * 30.0 + board * 3.0), cells(2.0));
			tint = vec3(0.94 + 0.1 * grain) * (0.94 + 0.1 * hw(vec2(board, 1.0), cells(5.0)));
			if (joint > 0.5) { t = max(0.0, t - 1.0); }
		} else if (pattern > 5.5 && pattern < 6.5) {
			// Cobbles : irregular stones (the nearest of jittered points), their joints in the shade
			vec2 p = g * 3.0;
			vec2 i = floor(p);
			float nc = cells(3.0);
			float best = 9.0, second = 9.0;
			float id = 0.0;
			for (int y = -1; y <= 1; y++) for (int x = -1; x <= 1; x++) {
				vec2 c = i + vec2(x, y);
				vec2 pt = c + 0.2 + 0.6 * vec2(hw(c, nc), hw(c + 9.0, nc));
				float dist = length(p - pt);
				if (dist < best) { second = best; best = dist; id = hw(c + 2.0, nc); } else if (dist < second) { second = dist; }
			}
			tint = vec3(0.92 + 0.14 * id);
			if (second - best < 0.09) { t = max(0.0, t - 1.0); tint = vec3(0.9); }
		}
		vec3 c = toneOf(t) * tint;
		if (steps > 3.5 && d > 0.9 && lightness > 0.5) c = mix(lit, paper, 0.4);
		gl_FragColor = vec4(c, 1.0);
	}`;

// A toon material : "facets" for the faceted shapes (the foliage of the references), "steps" 4 : the paper highlight
// (the paper highlight only on the facets : on a smooth shape it reads as a shine of plastic)
// "flat" : one tone (the small accents : eyes, cheeks, stripes stay their colour, the light would wash a black eye grey)
// "pattern" : a painted pattern (PATTERN), "scale" : its size (world units ; 1 a block)
// It receives the shadows of a scene with a light (page.html, view.shadows) ; without one it is lit by LIGHT only
// "period" : the pattern repeats every so many world units (1 : a tile that tiles), "patches" : the big patches of the grass
export function toon(color, { facets = false, steps = facets ? 4 : 3, side = THREE.FrontSide, flat = false, pattern = "none", scale = 1, period = 0, patches = true, shift = [0, 0] } = {}) {
	const base = new THREE.Color(color);
	const lit = flat ? base.clone() : base.clone().lerp(WARM, 0.36);
	const shade = flat ? base.clone() : base.clone().multiply(SHADE);
	return new THREE.ShaderMaterial({
		vertexShader: vertex,
		fragmentShader: fragment,
		side,
		lights: true,
		uniforms: THREE.UniformsUtils.merge([THREE.UniformsLib.lights, {
			base: { value: base }, lit: { value: lit }, shade: { value: shade }, paper: { value: PAPER },
			light: { value: LIGHT }, facets: { value: facets ? 1 : 0 }, steps: { value: steps },
			pattern: { value: PATTERN[pattern] ?? 0 }, scale: { value: scale }, period: { value: period }, patches: { value: patches ? 1 : 0 }, shift: { value: new THREE.Vector2(...shift) },
		}]),
	});
}

const inkVertex = /* glsl */`
	uniform float width;
	uniform vec2 resolution;
	uniform float brush;
	float h31(vec3 p) { return fract(sin(dot(p, vec3(127.1, 311.7, 74.7))) * 43758.5453); }
	float n3(vec3 p) {
		vec3 i = floor(p), f = fract(p);
		vec3 u = f * f * (3.0 - 2.0 * f);
		return mix(mix(mix(h31(i), h31(i + vec3(1, 0, 0)), u.x), mix(h31(i + vec3(0, 1, 0)), h31(i + vec3(1, 1, 0)), u.x), u.y),
			mix(mix(h31(i + vec3(0, 0, 1)), h31(i + vec3(1, 0, 1)), u.x), mix(h31(i + vec3(0, 1, 1)), h31(i + vec3(1, 1, 1)), u.x), u.y), u.z);
	}
	void main() {
		vec4 clip = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
		vec3 viewNormal = normalize(normalMatrix * normal);
		vec2 direction = (projectionMatrix * vec4(viewNormal, 0.0)).xy;
		float length2 = dot(direction, direction);
		if (length2 > 1e-8) direction /= sqrt(length2);
		// The brush (Okami, sumi-e) : the stroke swells and tapers along the shape
		vec3 world = (modelMatrix * vec4(position, 1.0)).xyz;
		float stroke = mix(1.0, 0.12 + 2.6 * smoothstep(0.3, 0.85, n3(world * 1.7)), brush);
		clip.xy += direction * width * stroke * 2.0 / resolution * clip.w;
		gl_Position = clip;
	}`;
const inkFragment = /* glsl */`
	uniform vec3 color;
	uniform vec3 sumi;
	uniform float brush;
	void main() { gl_FragColor = vec4(mix(color, sumi, brush), 1.0); }`;

export const inkUniforms = { width: { value: 2.0 }, resolution: { value: new THREE.Vector2(512, 512) }, brush: { value: 0 }, sumi: { value: new THREE.Color("#1b1712") } };

// The outline of every mesh of an object : a hull of its back faces pushed out (smooth normals : no crack at the facets).
// "scale" : its width relative to the others (thin for the grass)
export function ink(object, scale = 1) {
	const meshes = [];
	object.traverse((child) => { if (child.isMesh && !child.userData.hull && !child.userData.noInk) meshes.push(child); });
	for (const mesh of meshes) {
		let geometry = mesh.geometry.clone();
		geometry.deleteAttribute("uv");
		geometry = mergeVertices(geometry, 1e-4);
		geometry.computeVertexNormals();
		const material = new THREE.ShaderMaterial({
			vertexShader: inkVertex, fragmentShader: inkFragment, side: THREE.BackSide,
			uniforms: { width: { value: inkUniforms.width.value * scale }, resolution: inkUniforms.resolution, color: { value: INK }, brush: inkUniforms.brush, sumi: inkUniforms.sumi },
		});
		material.userData.scale = scale;
		const hull = new THREE.Mesh(geometry, material);
		hull.userData.hull = true;
		mesh.add(hull);
	}
	return object;
}

// The width of the outlines in pixels of the picture rendered (the supersampling multiplies it)
export function setInkWidth(object, pixels, resolution) {
	inkUniforms.resolution.value.copy(resolution);
	object.traverse((child) => {
		if (child.isMesh && child.userData.hull) {
			child.material.uniforms.width.value = pixels * (child.material.userData.scale ?? 1);
		}
	});
}

const washFragment = /* glsl */`
	uniform sampler2D picture;
	uniform vec2 resolution;
	uniform float strength;
	uniform float seed;
	// The wash repeats every "period" pixels (0 : never) : a tile that tiles (a ground) has no seam
	uniform float period;
	// The parchment of Okami (0 .. 1) : warm aged paper, the pigment pooling more
	uniform float brush;
	uniform vec3 paper;
	varying vec2 vUv;
	float hash(vec2 p) { return fract(sin(dot(p, vec2(127.1, 311.7)) + seed) * 43758.5453); }
	float lattice(vec2 i, float cells) { return hash(cells > 0.0 ? mod(i, cells) : i); }
	// A noise of the pixels at a scale (pixels a cell) ; periodic : the scale fitted to a whole number of cells a period
	float pnoise(vec2 px, float scale, float offset) {
		float cells = 0.0;
		if (period > 0.0) {
			cells = max(1.0, floor(period / scale + 0.5));
			scale = period / cells;
		}
		vec2 p = px / scale + offset;
		vec2 i = floor(p), f = fract(p);
		vec2 u = f * f * (3.0 - 2.0 * f);
		return mix(mix(lattice(i, cells), lattice(i + vec2(1, 0), cells), u.x), mix(lattice(i + vec2(0, 1), cells), lattice(i + vec2(1, 1), cells), u.x), u.y);
	}
	void main() {
		vec2 px = vUv * resolution;
		// The brush : the picture wobbles a little, slowly
		vec2 wobble = vec2(pnoise(px, 38.0, 0.0), pnoise(px, 38.0, 17.0)) - 0.5;
		vec2 uv = vUv + wobble * 3.0 * strength / resolution;
		vec4 c = texture2D(picture, uv);
		// The pigment pools at the edges of the shapes : darker where the colour differs from around it
		vec3 around = vec3(0.0);
		float alphaAround = 0.0;
		for (int i = 0; i < 8; i++) {
			float a = float(i) * 0.785398;
			vec4 s = texture2D(picture, uv + vec2(cos(a), sin(a)) * 4.0 / resolution);
			around += s.rgb;
			alphaAround += s.a;
		}
		around /= 8.0;
		float edge = clamp(length(c.rgb - around) * 1.6, 0.0, 1.0);
		c.rgb *= 1.0 - (0.16 + 0.14 * brush) * edge * strength;
		// The grain of the paper, the uneven wash
		float grain = pnoise(px, 2.2, 0.0) * 0.5 + pnoise(px, 9.0, 0.0) * 0.5;
		float wash = pnoise(px, 90.0, 0.0);
		c.rgb *= 1.0 + (grain - 0.5) * 0.09 * strength + (wash - 0.5) * 0.08 * strength;
		// The light of the references : the whites lifted, the colours washed a little towards the paper
		c.rgb = mix(c.rgb, paper, 0.05 * strength);
		// Parchment : the colours steeped in a warm paper, its fibres, its stains
		vec3 parchment = vec3(0.96, 0.89, 0.74);
		float stain = pnoise(px, 160.0, 31.0);
		c.rgb = mix(c.rgb, c.rgb * parchment * (1.04 - 0.1 * stain), brush * 0.55);
		c.rgb = mix(c.rgb, parchment, brush * 0.06);
		gl_FragColor = vec4(clamp(c.rgb, 0.0, 1.0), c.a);
	}`;

// The wash over a rendered picture (a render target) : drawn on the screen of the renderer
export function makeWash() {
	const material = new THREE.ShaderMaterial({
		vertexShader: "varying vec2 vUv; void main() { vUv = uv; gl_Position = vec4(position.xy, 0.0, 1.0); }",
		fragmentShader: washFragment,
		uniforms: { picture: { value: null }, resolution: { value: new THREE.Vector2() }, strength: { value: 1 }, seed: { value: 3.1 }, paper: { value: PAPER }, period: { value: 0 }, brush: { value: 0 } },
		transparent: true,
		depthTest: false,
	});
	const quad = new THREE.Mesh(new THREE.PlaneGeometry(2, 2), material);
	const scene = new THREE.Scene();
	scene.add(quad);
	return { scene, material, camera: new THREE.Camera() };
}

// A seeded random (the same asset each time)
export function random(seed) {
	let a = seed >>> 0;
	return () => {
		a = (a + 0x6d2b79f5) >>> 0;
		let t = a;
		t = Math.imul(t ^ (t >>> 15), t | 1);
		t ^= t + Math.imul(t ^ (t >>> 7), t | 61);
		return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
	};
}

// A mesh of a geometry and a colour
export function part(geometry, color, options = {}) {
	return new THREE.Mesh(geometry, toon(color, options));
}

// An ellipsoid (a sphere scaled)
export function blob(rx, ry, rz, color, options = {}) {
	const mesh = part(new THREE.SphereGeometry(1, options.facets ? 8 : 28, options.facets ? 6 : 20), color, options);
	mesh.scale.set(rx, ry, rz);
	return mesh;
}

// A rock of foliage : an icosahedron, its vertices pushed in and out (the faceted clumps of the references)
export function clump(radius, color, rand, detail = 1) {
	const geometry = new THREE.IcosahedronGeometry(radius, detail);
	const position = geometry.attributes.position;
	const moved = new Map();
	for (let i = 0; i < position.count; i++) {
		const key = `${position.getX(i).toFixed(4)},${position.getY(i).toFixed(4)},${position.getZ(i).toFixed(4)}`;
		if (!moved.has(key)) moved.set(key, 0.82 + rand() * 0.32);
		const k = moved.get(key);
		position.setXYZ(i, position.getX(i) * k, position.getY(i) * k * 0.86, position.getZ(i) * k);
	}
	geometry.computeVertexNormals();
	return part(geometry, color, { facets: true });
}

// The views of a character sheet of the game (Graphic/CharsetSheet.h, tools/tilegen/character.py) : frames of 72 px, 8
// rows (0 back, 1 right, 2 front, 3 left, 4 back-left, 5 back-right, 6 front-left, 7 front-right), the columns of the walk
// (0 a step, 1 standing, 2 the other step) ; the camera looks down 28 degrees ; the feet on the row 66 of the frame.
// "unit" : the pixels of a world unit in the frame. Each view has its row (its yaw) and its column (its step)
export const ROWS_YAW = [180, 90, 0, 270, 225, 135, 315, 45];
export const ELEVATION = 28;
export function sheetViews(unit = 36, columns = 3) {
	const frame = 72;
	const span = frame / unit;
	const lift = (66 - frame / 2) / unit / Math.cos(ELEVATION * Math.PI / 180);
	const views = [];
	for (let row = 0; row < 8; row++) {
		for (let column = 0; column < columns; column++) {
			views.push({ name: `${row}_${column}`, size: [frame, frame], span, target: [0, lift, 0], elevation: ELEVATION, row, column, yaw: 0, supersample: 4, ink: 0.9, frame: true });
		}
	}
	return views;
}

// A rounded cube (the volumes of the characters, in the manner of Cube World : soft blocks). "round" : the radius of its
// edges, relative to its smallest side (0 .. 0.5)
export function box(w, h, d, color, round = 0.3, options = {}) {
	const radius = Math.min(w, h, d) * round;
	return part(new RoundedBoxGeometry(w, h, d, 4, radius), color, options);
}

// The eyes of a Cube World face : tall dark rounded blocks set low and wide on the face, a white glint in the top
// corner, a touch of blush under them. "face" : the z of the front of the head
export function eyes(THREE, head, { y = -0.02, spread = 0.13, face = 0.3, size = 1, blush = PAL.cheek } = {}) {
	for (const side of [-1, 1]) {
		const eye = box(0.075 * size, 0.12 * size, 0.04, PAL.black, 0.45, { flat: true });
		eye.position.set(side * spread, y, face);
		eye.userData.noInk = true;
		head.add(eye);
		const glint = box(0.03 * size, 0.04 * size, 0.02, "#ffffff", 0.45, { flat: true });
		glint.position.set(side * spread - 0.016 * size, y + 0.03 * size, face + 0.022);
		glint.userData.noInk = true;
		head.add(glint);
		if (blush) {
			const cheek = box(0.07 * size, 0.03 * size, 0.02, blush, 0.45, { flat: true });
			cheek.position.set(side * (spread + 0.06 * size), y - 0.085 * size, face - 0.005);
			cheek.userData.noInk = true;
			head.add(cheek);
		}
	}
}
