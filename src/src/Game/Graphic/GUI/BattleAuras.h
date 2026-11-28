#pragma once
#include "Core/Point.h"
#include "../../Gameplay/Fight/FighterComponent.h"

namespace Ui {
	class Canvas;
}

// The animations over a fighter that show its state, drawn the same way in the real time battle (GUIBattle, over the
// map) and in a clash (ClashArena, its battleground) : from the feet of the Pokemon on the screen and the height of its
// body there (pixels : its drawing is scaled in a clash).
//   its status   poison : purple bubbles that rise and pop ; burn : flames ; paralysis : sparks that crackle around it ;
//                sleep : "Z" that rise ; leech : seeds that turn around it, its life drained upwards. Stronger a moment
//                when it lands or hurts ("burst" : 0 .. 1)
//   a statistic  arrows over it as in the first games : red rising (up), blue falling (down) ; "progress" 0 .. 1
namespace BattleAuras {
	void status(const Ui::Canvas& canvas, FighterStatus status, ska::Point<float> feet, float body, unsigned int time, float burst);
	void statChange(const Ui::Canvas& canvas, bool up, ska::Point<float> feet, float body, float progress);
}
