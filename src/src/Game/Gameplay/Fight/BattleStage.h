#pragma once
#include <string>
#include <vector>
#include "Core/ECS/ECSDefines.h"
#include "Core/Point.h"
#include "Core/Rectangle.h"

namespace ska {
	class CameraSystem;
}

enum class BattlePhase {
	// The fighters appear (dialog, pokeball)
	Intro,
	Fight,
	// A fighter is K.O. : slow motion, then the end of the battle
	KnockOut,
	// The player left the arena (a wild Pokemon) : the battle ends
	Fled,
	Over
};

// What a fighter does now, decided by the battle AI (script) or by the player (keys)
enum class FighterIntent {
	Hold = 0,
	Approach = 1,
	Retreat = 2,
	SidestepLeft = 3,
	SidestepRight = 4
};

struct FighterState {
	ska::EntityId entity = 0;
	ska::EntityId opponent = 0;
	// Played by the player (keys), else by the battle AI
	bool player = false;
	bool controlledByAI = false;

	FighterIntent intent = FighterIntent::Hold;
	// Milliseconds left : stunned after a hit (no control), white flash, wind-up before a skill, stretch after it
	unsigned int stun = 0;
	unsigned int flash = 0;
	unsigned int windUp = 0;
	unsigned int release = 0;
	// The skill used at the end of the wind-up (-1 : none)
	int pendingSkill = -1;
	// Pushed back after a hit (pixels per second), applied when the hit stop ends, then slowed down
	ska::Point<float> knockback;
	// Hit points shown by the bar : they follow the real ones with a delay (the lost part stays visible a moment)
	float shownHp = 0.F;
	unsigned int hpDelay = 0;
	// The bar shakes after a hit
	unsigned int barShake = 0;
	// Time spent outside of the arena (leaving it for 1 s : the player flees)
	unsigned int outside = 0;
};

// A feedback drawn over the battle (in the world) : damage numbers, impacts
struct BattleMark {
	enum class Kind { Damage, Impact };
	Kind kind = Kind::Impact;
	ska::Point<float> position;
	int value = 0;
	unsigned int age = 0;
	bool onPlayer = false;
};

// State of a real time battle, shared by the battle systems and the battle interface : time (hit stops, slow motion),
// arena, fighters, feedbacks. The systems change it, the interface draws it.
class BattleStage {
public:
	static constexpr unsigned int FleeDelay = 1000;

	// Logic update : the battle time elapsed (0 during a hit stop, slower during a slow motion)
	void update(unsigned int ellapsedTime);
	unsigned int delta() const { return m_delta; }
	bool frozen() const { return m_freeze > 0; }
	// Everything stops a moment (a hit)
	void hitStop(unsigned int duration);
	void slowMotion(float factor, unsigned int duration);

	void changePhase(BattlePhase phase);
	BattlePhase phase() const { return m_phase; }
	// Time in the current phase (real time)
	unsigned int phaseTime() const { return m_phaseTime; }

	// The big text in the middle of the screen ("COMBAT !", "K.O. !"), and its age
	void announce(std::string text);
	const std::string& banner() const { return m_banner; }
	unsigned int bannerAge() const { return m_bannerAge; }

	void mark(BattleMark mark);
	const std::vector<BattleMark>& marks() const { return m_marks; }

	void shake(unsigned int duration, int intensity);
	void setCamera(ska::CameraSystem* camera) { m_camera = camera; }
	ska::CameraSystem* camera() const { return m_camera; }

	FighterState& fighter(ska::EntityId entity) { return entity == player.entity ? player : opponent; }
	bool isFighter(ska::EntityId entity) const { return entity == player.entity || entity == opponent.entity; }

	FighterState player;
	FighterState opponent;
	// The area of the battle (world pixels) : the AI stays in it, the player flees a wild Pokemon by leaving it
	ska::Rectangle arena{};
	bool canFlee = true;
	ska::EntityId loser = 0;

private:
	unsigned int m_delta = 0;
	unsigned int m_freeze = 0;
	unsigned int m_slow = 0;
	float m_slowFactor = 1.F;
	float m_slowRemainder = 0.F;
	BattlePhase m_phase = BattlePhase::Intro;
	unsigned int m_phaseTime = 0;
	std::string m_banner;
	unsigned int m_bannerAge = 0;
	std::vector<BattleMark> m_marks;
	ska::CameraSystem* m_camera = nullptr;
};
