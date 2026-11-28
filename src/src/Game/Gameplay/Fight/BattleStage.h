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
enum class BattleSound {
	Cast, Hit, StrongHit, Guard, Status, Dash, KnockOut,
	// The clash (ClashSystem) : the impact, the zoom, the tiles that break, the screen "VS", the battleground rebuilt, the
	// rounds, the attacks, the K.O., the menu
	ClashImpact, ClashWoosh, ClashShatter, ClashVersus, ClashSwoosh, ClashLand, ClashRound, ClashShine, ClashAttack, ClashHit,
	ClashKnockOut, ClashCursor, ClashSelect,
	// A pokeball in a clash : thrown, the Pokemon goes in, it drops, it shakes, caught, it breaks free
	BallThrow, BallIn, BallDrop, BallShake, BallCaught, BallOpen,
	// A battle starts (the encounter) ; the bond (BondSystem) : the cut-in, the ring that closes, a perfect press, the skill
	// that goes
	Encounter, BondStart, BondCharge, BondPerfect, BondRelease,
	// A statistic rises, falls (Sounds/battle_stat_up.wav, battle_stat_down.wav : tools/assets/battle_sounds.py)
	StatUp, StatDown,
	Count
};

// A feedback drawn over the battle (in the world) : damage numbers, impacts
struct BattleMark {
	// Slash : the sprite of a contact skill where it hits ; Trail : where a projectile passed ("value" : the id of the
	// sprite of the skill) ; Text : a word over a fighter ("value" : 0 neutral, 1 good for it, 2 bad for it) ; Wave : an
	// area skill ("value" : its radius) ; Ghost : an afterimage of a Pokemon dashing ("value" : its entity, "position" :
	// where its center was)
	enum class Kind { Damage, Impact, Slash, Trail, Text, Wave, Ghost };
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

// Something that happened in a battle, for the trainer who comments it (TrainerCoachSystem, Data/Battle/lines.miniska) :
// "hit", "super", "weak" (the Pokemon of the player touches), "hurt" (it is hit), "potion", "ball" (an item : its name)
struct BattleCue {
	std::string event;
	std::string detail;
};

// The skill unleashed by the bond between the trainer and its Pokemon (BondSystem) : a cinematic. The battle stops, black
// bars, the trainer calls the skill, the camera closes on the Pokemon, a ring closes on it (pressed when it closes : a
// perfect timing), then the skill goes. Its times (real time, ms) :
struct BondCinematic {
	static constexpr unsigned int RingStart = 650;
	static constexpr unsigned int RingClose = 1350;
	// A press this long around the closing of the ring is perfect
	static constexpr unsigned int PerfectWindow = 160;
	static constexpr unsigned int Release = 1450;
	static constexpr unsigned int Duration = 1900;
	bool active = false;
	unsigned int time = 0;
	ska::EntityId user = 0;
	std::string pokemon;
	std::string skill;
	std::string line;
	// The player pressed (once : too early, it is not perfect), at this time ; the timing was perfect
	bool pressed = false;
	unsigned int pressedAt = 0;
	bool perfect = false;
	bool released = false;
};

// The evolution of a Pokemon of the team at the end of a battle (Battle.evolve, the outro of Data/Battle/steps.miniska),
// drawn over the battle by GUIBattle as in the first games : the light gathers on it, it turns white, the two forms
// alternate faster and faster, a white flash, the new form. Its times (real time, ms) :
struct BattleEvolution {
	static constexpr unsigned int Glow = 900;
	static constexpr unsigned int Morph = 1500;
	static constexpr unsigned int Flash = 5200;
	static constexpr unsigned int Reveal = 5600;
	static constexpr unsigned int Duration = 7000;
	bool active = false;
	unsigned int time = 0;
	// The species before and after (Data/Monsters, Sprites/Charsets)
	int from = 0;
	int to = 0;
};

// A clash as in a cross-over of the classic battles and the fighting games (ClashSystem, drawn by ClashArena) : the battle
// stops, the camera dives on the two Pokemon, the map around them breaks tile by tile, a screen "VS", a battleground is
// rebuilt from those tiles (seen from behind the Pokemon of the player, the opponent in front of it), then rounds as in
// the first games : the menu of the skills, each skill with its message and its hit. Then the tiles go back, the real time
// goes on. Its times are real time (ms).
struct BattleTurn {
	// A clash (both attacked at once : both act), an opening (a dodge at the last moment : the player acts alone in the
	// first round)
	enum class Kind { None, Clash, Opening };
	enum class Step { Impact, Zoom, Shatter, Versus, Rebuild, Round, Menu, Acting, RoundEnd, KnockOut, Return };
	static constexpr unsigned int ImpactDuration = 500;
	static constexpr unsigned int ZoomDuration = 650;
	static constexpr unsigned int ShatterDuration = 900;
	static constexpr unsigned int VersusDuration = 1500;
	static constexpr unsigned int RebuildDuration = 1100;
	// "ROUND 1", then "FIGHT !"
	static constexpr unsigned int RoundDuration = 1400;
	static constexpr unsigned int FightAt = 750;
	static constexpr unsigned int MenuDuration = 10000;
	// An action : its message, the wind up, the hit, then the next one
	static constexpr unsigned int HitDelay = 700;
	static constexpr unsigned int ActionDuration = 1600;
	static constexpr unsigned int RoundEndDuration = 600;
	static constexpr unsigned int KnockOutDuration = 2000;
	static constexpr unsigned int ReturnDuration = 1000;
	// The battleground is shown from the step Versus to the step Return
	bool arena() const { return active() && step != Step::Impact && step != Step::Zoom && step != Step::Shatter; }

	Kind kind = Kind::None;
	Step step = Step::Impact;
	// A number for each turn (the drawing keeps the tiles of a turn)
	unsigned int id = 0;
	// Time in the step, since the start (real time, ms)
	unsigned int time = 0;
	unsigned int total = 0;
	int round = 1;
	int rounds = 3;
	ska::EntityId player = 0;
	ska::EntityId opponent = 0;
	// Between the two Pokemon (world pixels) : the camera dives there, the tiles around it make the battleground
	ska::Point<float> center;
	std::string pokemon;
	std::string opponentName;
	std::vector<std::string> skills;
	std::vector<std::string> skillTypes;
	std::size_t selected = 0;
	// The message of the round (it appears letter by letter), since when
	std::string message;
	unsigned int messageAge = 0;
	// The action played : who, on whom, the skill (its sprite : Sprites/Fight/<id>.png), its hit (the damage, since when)
	ska::EntityId attacker = 0;
	ska::EntityId target = 0;
	int skillSprite = -1;
	bool contact = false;
	bool hit = false;
	unsigned int hitAge = 0;
	int damage = 0;
	// The bag of the menu (after the skills : "SAC") : open, its items (the bag : their ids, names, amounts)
	bool bag = false;
	std::vector<int> itemIds;
	std::vector<std::string> items;
	std::vector<int> itemCounts;
	// The action played is an item of the bag (-1 : a skill) : a healing item (the hit points given), a pokeball thrown at
	// the opponent (its shakes, caught). The ball : it flies, the Pokemon goes in, the ball drops, then each shake
	int item = -1;
	int healed = 0;
	bool ball = false;
	int shakes = 0;
	bool caught = false;
	static constexpr unsigned int BallFlight = 550;
	static constexpr unsigned int BallIn = 900;
	static constexpr unsigned int BallDrop = 1250;
	static constexpr unsigned int ShakeTime = 700;
	// The ball has shaken : caught, or it breaks free
	unsigned int ballEnd() const { return BallDrop + static_cast<unsigned int>(shakes) * ShakeTime + 350; }
	// The action played is its status hurting the target (poison, burn, leech : at the end of a round) : no attack, a
	// small hit
	bool tick = false;
	// The hits in a row of the Pokemon of the player ("2 COUPS !")
	int combo = 0;
	// The Pokemon K.O. in the turn
	ska::EntityId knockedOut = 0;
	bool active() const { return kind != Kind::None; }
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

	// How long an afterimage stays (milliseconds)
	static constexpr unsigned int GhostMarkLife = 220;
	void mark(BattleMark mark);
	const std::vector<BattleMark>& marks() const { return m_marks; }

	void shake(unsigned int duration, int intensity);
	// Something happened (the trainer comments it), the cues not commented yet (they are taken)
	void cue(BattleCue cue) { m_cues.push_back(std::move(cue)); }
	std::vector<BattleCue> takeCues() { auto cues = std::move(m_cues); m_cues.clear(); return cues; }
	// What the trainer says (a speech balloon over it), how long it is shown (real time) and since when
	void say(std::string text, unsigned int duration);
	const std::string& speech() const { return m_speech; }
	unsigned int speechAge() const { return m_speechAge; }
	unsigned int speechDuration() const { return m_speechDuration; }
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
	// The bond between the trainer and its Pokemon (0 .. BondFull : BondSystem), its cinematic
	static constexpr int BondFull = 100;
	int bond = 0;
	BondCinematic cinematic;
	// An evolution shown over the battle (the outro)
	BattleEvolution evolution;
	BattleTurn turn;
	// A clash asked by a script (Battle.clash) : ClashSystem starts it
	bool clashRequested = false;
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
	std::vector<BattleCue> m_cues;
	std::string m_speech;
	unsigned int m_speechAge = 0;
	unsigned int m_speechDuration = 0;
	ska::CameraSystem* m_camera = nullptr;
	std::function<void(BattleSound)> m_soundPlayer;
};
