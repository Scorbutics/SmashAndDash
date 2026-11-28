// The hero of the game, a chibi in the manner of Cube World (as tools/tilegen/character.py draws it) : soft blocks, a big
// head (half of its height) under a red cap and tufts of dark hair, big eyes low on the face, a small round body in a red
// jacket, round hands floating at its sides, stubby feet. Its character sheet for the map and a big view
export function build(THREE, S, options = {}) {
	const P = S.PAL;
	const root = new THREE.Group();
	const body = new THREE.Group();
	root.add(body);

	// The feet : stubby, red shoes
	const feet = [];
	for (const side of [-1, 1]) {
		const foot = S.box(0.16, 0.12, 0.22, P.shoe, 0.4);
		foot.position.set(side * 0.11, 0.06, 0.02);
		body.add(foot);
		feet.push(foot);
	}
	// The legs (short, blue) and the body (a soft block : the red jacket, its white zip)
	const hips = S.box(0.3, 0.12, 0.22, P.denim, 0.4);
	hips.position.y = 0.17;
	body.add(hips);
	const torso = S.box(0.36, 0.3, 0.28, P.jacket, 0.35);
	torso.position.y = 0.36;
	body.add(torso);
	const zip = S.box(0.05, 0.24, 0.02, P.white, 0.4, { flat: true });
	zip.position.set(0, 0.36, 0.145);
	zip.userData.noInk = true;
	body.add(zip);
	// The hands : round, floating at its sides (no arms, as in Cube World)
	const hands = [];
	for (const side of [-1, 1]) {
		const hand = S.box(0.12, 0.12, 0.12, P.skin, 0.5);
		hand.position.set(side * 0.27, 0.32, 0.02);
		body.add(hand);
		hands.push(hand);
	}
	// The head : big, its face, its hair (tufts out of the cap), the cap and its visor
	const head = new THREE.Group();
	head.position.y = 0.84;
	body.add(head);
	head.add(S.box(0.62, 0.56, 0.56, P.skin, 0.32));
	const hairBack = S.box(0.66, 0.4, 0.4, P.hair, 0.35);
	hairBack.position.set(0, 0.08, -0.1);
	head.add(hairBack);
	for (const [x, y, z] of [[-0.3, -0.05, 0.08], [0.3, -0.05, 0.08], [-0.2, -0.12, -0.18], [0.22, -0.13, -0.17]]) {
		const tuft = S.box(0.14, 0.18, 0.16, P.hair, 0.45);
		tuft.position.set(x, y, z);
		tuft.rotation.z = x > 0 ? -0.25 : 0.25;
		head.add(tuft);
	}
	const fringe = S.box(0.5, 0.1, 0.12, P.hair, 0.45);
	fringe.position.set(0, 0.17, 0.25);
	head.add(fringe);
	S.eyes(THREE, head, { y: -0.04, spread: 0.13, face: 0.285, size: 1.15 });
	const cap = S.box(0.68, 0.24, 0.62, P.red, 0.42);
	cap.position.set(0, 0.3, -0.01);
	head.add(cap);
	const panel = S.box(0.3, 0.19, 0.06, P.white, 0.4, { flat: true });
	panel.position.set(0, 0.32, 0.3);
	panel.userData.noInk = true;
	head.add(panel);
	const visor = S.box(0.5, 0.05, 0.26, P.red, 0.4);
	visor.position.set(0, 0.2, 0.36);
	head.add(visor);
	S.ink(root);

	const views = S.sheetViews(38);
	views.push({ name: "hero_big", size: [300, 360], span: 1.6, target: [0, 0.66, 0], elevation: 14, row: 6, column: 1, ink: 1.6 });
	return {
		root,
		views,
		pose(view) {
			const yaw = view.row !== undefined ? S.ROWS_YAW[view.row] : 0;
			root.rotation.y = yaw * Math.PI / 180;
			// The walk : the feet in turn, the hands swing the other way, a bounce
			const step = view.column === 1 ? 0 : (view.column === 0 ? 1 : -1);
			feet[0].position.z = 0.02 + step * 0.1;
			feet[1].position.z = 0.02 - step * 0.1;
			hands[0].position.z = 0.02 - step * 0.1;
			hands[1].position.z = 0.02 + step * 0.1;
			body.position.y = step === 0 ? 0 : 0.03;
			head.rotation.z = step * 0.04;
		},
	};
}
