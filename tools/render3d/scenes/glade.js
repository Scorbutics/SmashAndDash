// A glade of the forest, as the references show it : the ground washed with light, a path, the trees, bushes and tall
// grass, the hero and Pikachu. Two views : the camera of the map (looking down, orthographic : what the game would show)
// and a low camera as in the references (a battle, a clash, a cinematic)
import { tuft, bush } from "./grass.js";
import { build as tree } from "./tree.js";
import { build as hero } from "./hero.js";
import { build as pikachu } from "./pikachu.js";

export function build(THREE, S, options = {}) {
	const rand = S.random(Number(options.seed ?? 11));
	const root = new THREE.Group();
	const P = S.PAL;

	// The ground : a wash of pale green, a sandy path that winds, a few darker patches
	const ground = S.part(new THREE.PlaneGeometry(40, 40), P.ground, { steps: 3 });
	ground.rotation.x = -Math.PI / 2;
	ground.userData.noInk = true;
	root.add(ground);
	const path = new THREE.Shape();
	path.moveTo(-1.1, 12);
	path.bezierCurveTo(-0.4, 6, -2.2, 2, -0.6, -2);
	path.bezierCurveTo(0.4, -5, 2.4, -7, 2.0, -12);
	path.lineTo(3.6, -12);
	path.bezierCurveTo(4.0, -7, 1.9, -4.6, 1.0, -1.6);
	path.bezierCurveTo(-0.3, 2.4, 1.2, 6, 0.6, 12);
	path.closePath();
	const road = S.part(new THREE.ShapeGeometry(path, 40), P.path, { steps: 3 });
	road.rotation.x = -Math.PI / 2;
	road.position.y = 0.005;
	road.userData.noInk = true;
	root.add(road);
	for (let i = 0; i < 14; i++) {
		const patch = S.part(new THREE.CircleGeometry(0.6 + rand() * 1.2, 7), rand() < 0.5 ? P.groundDeep : P.groundPale, { steps: 3 });
		patch.rotation.x = -Math.PI / 2;
		patch.position.set((rand() - 0.5) * 18, 0.003, (rand() - 0.5) * 18);
		patch.scale.y = 0.6;
		patch.userData.noInk = true;
		root.add(patch);
	}

	// The sky : deep blue at the top, pale at the horizon (the wash of a watercolour sky)
	const sky = new THREE.Mesh(new THREE.SphereGeometry(80, 32, 16), new THREE.ShaderMaterial({
		side: THREE.BackSide,
		depthWrite: false,
		uniforms: { top: { value: new THREE.Color("#5c9fe6") }, horizon: { value: new THREE.Color("#d4ebf6") } },
		vertexShader: "varying vec3 vDir; void main() { vDir = normalize(position); gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); }",
		fragmentShader: "uniform vec3 top; uniform vec3 horizon; varying vec3 vDir; void main() { gl_FragColor = vec4(mix(horizon, top, smoothstep(0.0, 0.35, vDir.y)), 1.0); }",
	}));
	sky.userData.noInk = true;
	root.add(sky);

	// The trees : rows at the back and on the sides, out of the glade
	const spots = [];
	for (let i = 0; i < 26; i++) {
		const x = (rand() - 0.5) * 22;
		const z = -3 - rand() * 9;
		if (Math.abs(x - 0.3 * z) < 1.6) continue;
		spots.push([x, z]);
	}
	for (const [x, z] of [[-5.5, 1.5], [-6.8, 4], [5.8, 0.5], [6.5, 3.5], [-4.2, -1.8], [4.8, -2.2]]) spots.push([x, z]);
	for (const [x, z] of spots) {
		const t = tree(THREE, S, { seed: Math.floor(rand() * 1000), height: 3.6 + rand() * 1.6 }).root;
		t.position.set(x, 0, z);
		t.rotation.y = rand() * Math.PI * 2;
		root.add(t);
	}
	// The bushes and the tall grass around the glade, the grass in the front
	for (let i = 0; i < 14; i++) {
		const b = bush(THREE, S, rand, 0.9 + rand() * 0.8);
		const side = rand() < 0.5 ? -1 : 1;
		b.position.set(side * (2.4 + rand() * 4), 0, -3 + rand() * 6);
		root.add(b);
	}
	for (let i = 0; i < 70; i++) {
		const g = tuft(THREE, S, rand, 7 + Math.floor(rand() * 6));
		const x = (rand() - 0.5) * 12;
		const z = -2 + rand() * 8;
		if (Math.abs(x - 0.4) < 0.9 && z < 3) continue;
		g.position.set(x, 0, z);
		g.scale.setScalar(0.9 + rand() * 0.9);
		g.rotation.y = rand() * Math.PI * 2;
		root.add(g);
	}

	// The hero and Pikachu, in the glade
	const h = hero(THREE, S);
	h.pose({ row: 6, column: 1 });
	h.root.position.set(-0.4, 0, 1.2);
	h.root.rotation.y += 0.3;
	root.add(h.root);
	const p = pikachu(THREE, S);
	p.pose({ row: 2, column: 1 });
	p.root.position.set(0.9, 0, 0.8);
	p.root.rotation.y = -0.5;
	root.add(p.root);

	return {
		root,
		views: [
			{ name: "glade_map", size: [960, 600], span: 9, target: [0.2, 0.4, 0.6], elevation: 50, yaw: 0, background: P.ground, ink: 1.2 },
			{ name: "glade_low", size: [960, 540], perspective: true, fov: 38, target: [0.2, 1.1, 0.5], elevation: 8, distance: 7.5, yaw: 0, background: P.sky, ink: 1.3 },
		],
	};
}
