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

// A feedback drawn over the battle (in the world) : damage numbers, impacts
struct BattleMark {
	// Slash : the sprite of a contact skill where it hits ("value" : the id of the sprite)
	enum class Kind { Damage, Impact, Slash };
	Kind kind = Kind::Impact;
	ska::Point<float> position;
	int value = 0;
	unsigned int age = 0;
	bool onPlayer = false;
};

// State of a real time battle, shared by the battle systems and the battle interface : time (hit stops, slow motion),
// arena, feedbacks, which fighter is the one of the player. The state of each fighter is its FighterComponent.
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

	// The Pokemon of the player and the one it fights (the bars of the interface, the result of the battle)
	ska::EntityId player = 0;
	ska::EntityId opponent = 0;
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
