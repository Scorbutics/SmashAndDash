// Pikachu, cute, in the manner of Cube World : soft blocks, a big head, big eyes low on the face, the red cheeks, the
// ears tipped with black, a tiny body, stubby feet and paws, the lightning tail in blocks. Its character sheet for the
// map (8 directions, the walk : style.sheetViews) and a big view
export function build(THREE, S, options = {}) {
	const root = new THREE.Group();
	const body = new THREE.Group();
	root.add(body);
	const P = S.PAL;

	const feet = [];
	for (const side of [-1, 1]) {
		const foot = S.box(0.13, 0.08, 0.17, P.yellow, 0.45);
		foot.position.set(side * 0.1, 0.04, 0.03);
		body.add(foot);
		feet.push(foot);
	}
	const torso = S.box(0.38, 0.32, 0.32, P.yellow, 0.42);
	torso.position.y = 0.22;
	body.add(torso);
	// The back : two brown stripes
	for (const y of [0.29, 0.19]) {
		const stripe = S.box(0.24, 0.04, 0.03, P.hair, 0.4, { flat: true });
		stripe.position.set(0, y, -0.16);
		stripe.userData.noInk = true;
		body.add(stripe);
	}
	const paws = [];
	for (const side of [-1, 1]) {
		const paw = S.box(0.09, 0.09, 0.09, P.yellow, 0.5);
		paw.position.set(side * 0.15, 0.25, 0.17);
		body.add(paw);
		paws.push(paw);
	}
	const head = new THREE.Group();
	head.position.y = 0.6;
	body.add(head);
	head.add(S.box(0.62, 0.5, 0.5, P.yellow, 0.36));
	// The ears : long blocks, out and back, their tips black
	for (const side of [-1, 1]) {
		const ear = new THREE.Group();
		const base = S.box(0.13, 0.3, 0.09, P.yellow, 0.45);
		base.position.y = 0.15;
		ear.add(base);
		const tip = S.box(0.1, 0.14, 0.08, P.black, 0.45);
		tip.position.y = 0.34;
		ear.add(tip);
		ear.position.set(side * 0.2, 0.22, -0.04);
		ear.rotation.z = -side * 0.62;
		ear.rotation.x = -0.12;
		head.add(ear);
	}
	S.eyes(THREE, head, { y: 0.0, spread: 0.13, face: 0.255, size: 1.2, blush: null });
	// The red cheeks, the little nose and mouth
	for (const side of [-1, 1]) {
		const cheek = S.box(0.11, 0.09, 0.04, P.cheek, 0.5, { flat: true });
		cheek.position.set(side * 0.235, -0.1, 0.235);
		cheek.userData.noInk = true;
		head.add(cheek);
	}
	const nose = S.box(0.025, 0.018, 0.02, P.black, 0.4, { flat: true });
	nose.position.set(0, -0.06, 0.255);
	nose.userData.noInk = true;
	head.add(nose);
	const mouth = S.box(0.06, 0.018, 0.02, P.hair, 0.4, { flat: true });
	mouth.position.set(0, -0.11, 0.255);
	mouth.userData.noInk = true;
	head.add(mouth);
	// The tail : a lightning bolt of blocks, brown at its root
	const tail = new THREE.Group();
	const segments = [[0, 0, 0.1, 0.16, P.hair, 0.3], [0.05, 0.13, 0.1, 0.18, P.yellow, -0.7], [0.14, 0.24, 0.12, 0.22, P.yellow, 0.4], [0.24, 0.4, 0.2, 0.26, P.yellow, -0.6]];
	for (const [x, y, w, h, color, angle] of segments) {
		const piece = S.box(w, h, 0.06, color, 0.35);
		piece.position.set(x, y, 0);
		piece.rotation.z = angle;
		tail.add(piece);
	}
	tail.position.set(0.08, 0.18, -0.2);
	tail.rotation.y = Math.PI / 2 + 0.25;
	body.add(tail);
	S.ink(root);

	const views = S.sheetViews(42);
	views.push({ name: "pikachu_big", size: [320, 320], span: 1.35, target: [0, 0.52, 0], elevation: 16, row: 6, column: 1, ink: 1.6 });
	return {
		root,
		views,
		pose(view) {
			const yaw = view.row !== undefined ? S.ROWS_YAW[view.row] : 0;
			root.rotation.y = yaw * Math.PI / 180;
			// The walk : a hop, the feet in turn
			const step = view.column === 1 ? 0 : (view.column === 0 ? 1 : -1);
			feet[0].position.z = 0.03 + step * 0.08;
			feet[1].position.z = 0.03 - step * 0.08;
			paws[0].position.y = 0.25 + step * 0.03;
			paws[1].position.y = 0.25 - step * 0.03;
			body.position.y = step === 0 ? 0 : 0.04;
			head.rotation.z = -step * 0.05;
		},
	};
}
