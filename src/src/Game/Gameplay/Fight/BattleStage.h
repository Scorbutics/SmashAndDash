#pragma once
#include <functional>
#include <optional>
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
	// The step "start" of the battle runs (Data/Battle/steps.miniska) : the fighters wait in the arena
	Starting,
	Fight,
	// A fighter is K.O. : slow motion, then the end of the battle
	KnockOut,
	// The player left the arena (a wild Pokemon) : the battle ends
	Fled,
	// The end of the battle : the Pokemon of the player goes back in its pokeball, the experience won, the camera goes
	// back to the trainer (the step "outro" : its dialogs)
	Outro,
	Over
};

// The sounds of a battle (BattleSounds)
enum class BattleSound { Cast, Hit, StrongHit, Guard, Status, Dash, KnockOut, Count };

// A feedback drawn over the battle (in the world) : damage numbers, impacts
struct BattleMark {
	// Slash : the sprite of a contact skill where it hits ; Trail : where a projectile passed ("value" : the id of the
	// sprite of the skill) ; Text : a word over a fighter ("value" : 0 neutral, 1 good for it, 2 bad for it) ; Wave : an
	// area skill ("value" : its radius)
	enum class Kind { Damage, Impact, Slash, Trail, Text, Wave };
	Kind kind = Kind::Impact;
	ska::Point<float> position;
	int value = 0;
	unsigned int age = 0;
	bool onPlayer = false;
	std::string text;
};

// The end of a battle as the interface shows it (BattlePhase::Outro) : the experience won by the Pokemon of the player,
// its level and its progress towards the next one before and after (0 .. 1)
struct BattleOutro {
	std::string pokemon;
	int experience = 0;
	int levelBefore = 1;
	int levelAfter = 1;
	float progressBefore = 0.F;
	float progressAfter = 0.F;
	// The screen was gray (a K.O.) : it comes back to its colors
	bool fromKnockOut = false;
};

// The item of the trainer in a battle (TrainerItemSystem) : the item chosen among those of the bag usable in a battle
// (-1 : none), its name, its amount, the cooldown left (1 : just used .. 0 : ready)
struct BattleItemSlot {
	int item = -1;
	std::string name;
	int amount = 0;
	float cooldown = 0.F;
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
	// A sound of the battle (played by the player given, nothing without one)
	void sound(BattleSound sound) const { if (m_soundPlayer) { m_soundPlayer(sound); } }
	void setSoundPlayer(std::function<void(BattleSound)> player) { m_soundPlayer = std::move(player); }
	void setCamera(ska::CameraSystem* camera) { m_camera = camera; }
	ska::CameraSystem* camera() const { return m_camera; }

	// The Pokemon of the player (the keys, its skills in the interface, the result of the battle), all the fighters
	ska::EntityId player = 0;
	std::vector<ska::EntityId> fighters;
	// The area of the battle (world pixels) : the AI stays in it, the player flees a wild Pokemon by leaving it
	ska::Rectangle arena{};
	bool canFlee = true;
	// The side that won (-1 : none yet)
	int winnerTeam = -1;
	// The trainer of the player (it throws the pokeball, the camera goes back to it at the end)
	std::optional<ska::EntityId> trainer;
	BattleOutro outro;
	BattleItemSlot itemSlot;
	// The last wild Pokemon out of the fight was captured (not K.O.) : the battle is won by a capture, its name
	bool captured = false;
	std::string capturedName;

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
	std::function<void(BattleSound)> m_soundPlayer;
};
