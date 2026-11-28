// Tall grass and a bush of the references : long blades that bend (seen from both sides), a bush of faceted clumps.
// Exported parts (the glade uses them) and their own views
export function tuft(THREE, S, rand, blades = 9) {
	const group = new THREE.Group();
	for (let i = 0; i < blades; i++) {
		// A blade : a strip, wide at its middle, sharp at its tip, bent over
		const length = 0.45 + rand() * 0.45;
		const width = 0.06 + rand() * 0.05;
		const bend = 0.25 + rand() * 0.45;
		const rows = 5;
		const positions = [];
		const index = [];
		for (let r = 0; r <= rows; r++) {
			const t = r / rows;
			const w = width * Math.sin(Math.PI * Math.min(0.98, 0.15 + t * 0.85)) * (1 - t * 0.6);
			const y = length * t;
			const z = bend * t * t * length;
			positions.push(-w, y, z, w, y, z);
			if (r < rows) {
				const a = r * 2;
				index.push(a, a + 1, a + 2, a + 1, a + 3, a + 2);
			}
		}
		const geometry = new THREE.BufferGeometry();
		geometry.setAttribute("position", new THREE.Float32BufferAttribute(positions, 3));
		geometry.setIndex(index);
		geometry.computeVertexNormals();
		const color = rand() < 0.3 ? S.PAL.leafPale : (rand() < 0.4 ? S.PAL.grassDeep : S.PAL.grass);
		const blade = S.part(geometry, color, { facets: true, side: THREE.DoubleSide });
		const angle = (i / blades) * Math.PI * 2 + rand() * 0.6;
		blade.position.set(Math.cos(angle) * 0.1 * rand(), 0, Math.sin(angle) * 0.1 * rand());
		blade.rotation.y = angle + Math.PI / 2;
		blade.rotation.x = -0.1 - rand() * 0.25;
		group.add(blade);
	}
	S.ink(group, 0.55);
	return group;
}

export function bush(THREE, S, rand, size = 1) {
	const group = new THREE.Group();
	// Mostly green, sometimes golden
	const golden = rand() < 0.25;
	const tones = golden ? [S.PAL.goldPale, S.PAL.gold, S.PAL.goldDeep] : [S.PAL.leafPale, S.PAL.leaf, S.PAL.leafDeep];
	const count = 7 + Math.floor(rand() * 4);
	for (let k = 0; k < count; k++) {
		const r = (0.22 + rand() * 0.2) * size;
		const color = rand() < 0.4 ? tones[0] : (rand() < 0.3 ? tones[2] : tones[1]);
		const c = S.clump(r, color, rand, 1);
		const a = rand() * Math.PI * 2;
		const d = rand() * 0.45 * size;
		c.position.set(Math.cos(a) * d, r * 0.7 + rand() * 0.25 * size, Math.sin(a) * d * 0.7);
		c.rotation.set(rand() * 3, rand() * 3, rand() * 3);
		group.add(c);
	}
	S.ink(group);
	return group;
}

export function build(THREE, S, options = {}) {
	const rand = S.random(Number(options.seed ?? 3));
	const root = new THREE.Group();
	const grass = tuft(THREE, S, rand, 11);
	grass.position.x = -0.8;
	root.add(grass);
	const shrub = bush(THREE, S, rand, 1.1);
	shrub.position.x = 0.8;
	root.add(shrub);
	return {
		root,
		views: [
			{ name: "grass_front", size: [320, 160], span: 1.6, target: [0, 0.45, 0], elevation: 28 },
			{ name: "grass_map", size: [320, 160], span: 1.6, target: [0, 0.4, 0], elevation: 50 },
		],
	};
}
