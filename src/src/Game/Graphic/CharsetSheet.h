#pragma once

namespace ska {
	class AnimatedTexture;
}

// The character sheets (Sprites/Charsets) : frames of 72 px, 8 rows (the directions), 3 animated. 6 columns : the walk
// (0 to 2 ; 3 to 5 unused, the jump shows the column 2). The hero has 9 (tools/tilegen/character.py) : the run (3 to 5)
// and the poses between them (6 setting off, 7 pushing off into a run, 8 braking)
namespace Charset {
	constexpr unsigned int Columns = 6;
	constexpr unsigned int GaitColumns = 9;
	constexpr unsigned int RunColumn = 3;
	constexpr int SetOffPose = 6;
	constexpr int PushOffPose = 7;
	constexpr int BrakePose = 8;

	// Loads the sheet of a character (its id in Sprites/Charsets), of 6 or 9 columns
	void load(ska::AnimatedTexture& texture, int id);
	// The sheet has the run and its poses
	bool hasGaits(const ska::AnimatedTexture& texture);
}
