// A tree of the forests of the references : a tall grey trunk that bends a little, a few branches, a canopy of faceted
// clumps (pale yellow green in the light). Options : seed, height (world units ; a block is 1)
export function build(THREE, S, options = {}) {
	const rand = S.random(Number(options.seed ?? 7));
	// Its canopy : green, golden or orange (options.kind, else drawn from its seed : mostly green)
	const kinds = { green: [S.PAL.leafPale, S.PAL.leaf, S.PAL.leafDeep], gold: [S.PAL.goldPale, S.PAL.gold, S.PAL.goldDeep], orange: [S.PAL.orangePale, S.PAL.orange, S.PAL.orangeDeep] };
	const pick = rand();
	// (options.leaves : its own three tones, light to deep)
	const leaves = options.leaves ?? kinds[options.kind ?? (pick < 0.62 ? "green" : (pick < 0.85 ? "gold" : "orange"))];
	const height = Number(options.height ?? 4.2);
	const root = new THREE.Group();

	// The trunk : segments that bend, thinner upwards
	const trunk = new THREE.Group();
	let base = new THREE.Vector3(0, 0, 0);
	let radius = 0.2;
	const segments = 5;
	const tops = [];
	for (let i = 0; i < segments; i++) {
		const length = height * 0.72 / segments;
		const lean = new THREE.Vector3((rand() - 0.5) * 0.12, 1, (rand() - 0.5) * 0.08).normalize();
		const top = base.clone().addScaledVector(lean, length);
		const next = radius * 0.86;
		const piece = S.part(new THREE.CylinderGeometry(next, radius, length * 1.05, 9), i % 2 ? S.PAL.trunk : S.PAL.trunk);
		piece.position.copy(base.clone().add(top).multiplyScalar(0.5));
		piece.quaternion.setFromUnitVectors(new THREE.Vector3(0, 1, 0), lean);
		trunk.add(piece);
		tops.push(top.clone());
		base = top;
		radius = next;
	}
	// The roots : a flare at its foot
	const flare = S.part(new THREE.CylinderGeometry(0.2, 0.32, 0.3, 9), S.PAL.bark);
	flare.position.y = 0.15;
	trunk.add(flare);
	root.add(trunk);

	// The branches : from the upper part of the trunk, up and out
	const ends = [base.clone()];
	for (let b = 0; b < 4; b++) {
		const from = tops[2 + (b % 3)].clone();
		const angle = b * 1.7 + rand();
		const direction = new THREE.Vector3(Math.cos(angle) * 0.8, 0.9, Math.sin(angle) * 0.5).normalize();
		const length = 0.7 + rand() * 0.5;
		const branch = S.part(new THREE.CylinderGeometry(0.035, 0.08, length, 6), S.PAL.trunk);
		branch.position.copy(from.clone().addScaledVector(direction, length / 2));
		branch.quaternion.setFromUnitVectors(new THREE.Vector3(0, 1, 0), direction);
		root.add(branch);
		ends.push(from.clone().addScaledVector(direction, length));
	}

	// The canopy : clumps around the ends of the branches and the top, light ones on top
	const canopy = new THREE.Group();
	for (const end of ends) {
		const count = 4 + Math.floor(rand() * 3);
		for (let k = 0; k < count; k++) {
			const r = 0.3 + rand() * 0.26;
			const color = rand() < 0.35 ? leaves[0] : (rand() < 0.25 ? leaves[2] : leaves[1]);
			const c = S.clump(r, color, rand, 1);
			c.position.set(end.x + (rand() - 0.5) * 1.0, end.y + (rand() - 0.1) * 0.7, end.z + (rand() - 0.5) * 0.9);
			c.rotation.set(rand() * 3, rand() * 3, rand() * 3);
			canopy.add(c);
		}
	}
	root.add(canopy);
	S.ink(root);

	const views = [
		{ name: "tree_front", size: [256, 320], span: height * 1.35, target: [0, height * 0.55, 0], elevation: 28 },
		{ name: "tree_map", size: [256, 320], span: height * 1.35, target: [0, height * 0.5, 0], elevation: 50 },
	];
	return { root, views, canopy };
}
