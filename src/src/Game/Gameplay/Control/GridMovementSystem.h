#pragma once
#include <random>
#include "Base/Values/Numbers/NumberUtils.h"
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Core/ECS/Basics/Physic/ForceComponent.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"
#include "Core/ECS/Basics/Input/InputComponent.h"
#include "Core/ECS/Basics/Physic/CollidableComponent.h"
#include "Core/Inputs/InputToggle.h"
#include "Core/Data/Events/InputKeyEvent.h"
#include "Base/Patterns/SubObserver.h"
#include "AI/IADefinedMovementComponent.h"
#include "GridMovementComponent.h"
#include "MovementModeComponent.h"
#include "../PokemonGameEventDispatcher.h"

class WorldState;

// The characters that occupy blocks (collidable, with a physics body) : its entities are kept by the ECS
class BlockOccupants : public ska::System<ska::RequiredComponent<ska::CollidableComponent, ska::PositionComponent, ska::HitboxComponent, ska::cp::HitboxComponent>, ska::PossibleComponent<>> {
public:
	explicit BlockOccupants(ska::EntityManager& entityManager) : System(entityManager) {}
	~BlockOccupants() override = default;
	using System::getEntities;
protected:
	void refresh(unsigned int) override {}
};

// Movement of the characters on the map.
// The player (mode Keys) walks freely :
// - 8 directions (the diagonal rows of the charsets), at the speed of a block each WalkStep ms (RunStep ms when running :
//   the run key, or the "run" movement mode), reached in a few frames (Accel), stopped in a few frames (Decel)
// - its physics body moves (the walls of the map, partial tiles included, and the other characters stop it) ; it does not
//   leave the map
// - against a wall that it nearly passes (by CornerSlide pixels), it slides around its corner
// - its gait (still, walking, running) : the walk, the run, and a pose between them (setting off, pushing off, braking)
// - it faces one of the 4 directions (GridMovementComponent::facing : the block in front for the action key, the
//   scripts) : the one held, the one it already faces on a diagonal
// - pushing against a wall in one of the 4 directions touches it once (its scripts, ex : the water to surf, a ledge) ;
//   walking along a wall does not (ScriptAwareComponent::physicsTouches)
// - the character its shape touches is "touched" (GridMovementComponent::touched : a battle starts with a wild Pokemon)
// The other characters move block by block (as the classic Pokemon games) :
// - a step starts only when the next block is practicable and free (no character on it, nor going to it) : nothing is pushed
// - the character is placed at each frame (its physics body follows), exactly on the block at the end of the step
// The free movement of the keys (InputSystem) is disabled for these characters (their move power is 0 on the map : the
// power of their normal walk is kept by MovementModeComponent, ex : for the battles). Characters moved by a script
// (IADefinedMovementComponent) or without InputComponent (ex : disabled during a cutscene) do not take steps.
// The other characters wander (random steps after random waits) or walk a path, with the same rules (they wait when the
// next block is not free). A wandering character can react to another one in sight (the wild Pokemon and the trainer) :
// it approaches it (until it walks into it) or runs away from it.
// A step refused because a character stands on the next block "touches" it (GridMovementComponent::touched).
// Forced steps (GridMovementComponent::forcedSteps : a script, the player leaving a door, a ledge) are block by block for
// every character, the player too : they go through the obstacles, the keys are ignored meanwhile ; they wait for the
// screen to show the map (ScreenFade).
class GridMovementSystem :
	public ska::System<ska::RequiredComponent<GridMovementComponent, ska::PositionComponent, ska::HitboxComponent, ska::ForceComponent, ska::AnimationComponent>,
		ska::PossibleComponent<ska::IADefinedMovementComponent, MovementModeComponent, ska::InputComponent>>,
	public ska::SubObserver<ska::InputKeyEvent> {
public:
	static constexpr unsigned int WalkStep = 250;
	static constexpr unsigned int RunStep = 130;
	// The free walk of the player : the time (ms) to reach about 2/3 of its speed, to lose 2/3 of it
	static constexpr float Accel = 45.F;
	static constexpr float Decel = 35.F;
	// The poses between the gaits of the free walk (ms) : setting off, pushing off into a run, braking (from a run to a stop ;
	// 2/3 of it to a walk)
	static constexpr unsigned int SetOffTime = 90;
	static constexpr unsigned int PushOffTime = 120;
	static constexpr unsigned int BrakeTime = 180;
	// The free walk slides around the corner of a wall it misses by this much (pixels)
	static constexpr int CornerSlide = 18;
	// A character that reacts to another one (GridMovementComponent::reaction) stops this long when it notices it
	static constexpr unsigned int NoticeDelay = 500;
	// The hop of the jump key on the map : height (pixels), duration (ms)
	static constexpr float HopHeight = 16.F;
	static constexpr unsigned int HopDuration = 280;
	// The jump over a ledge : height (pixels) of its arc, over all its steps
	static constexpr float LedgeHopHeight = 24.F;

	GridMovementSystem(ska::EntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState);
	~GridMovementSystem() override = default;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	bool onKeyInputEvent(ska::InputKeyEvent& event);
	// The free walk of the player (controlled : the keys move it)
	void walkFreely(ska::EntityId entity, GridMovementComponent& grid, bool controlled, bool running, unsigned int ellapsedTime);
	// A wall right in front of the physics shape, in a direction (0 : down, 1 : right, 2 : up, 3 : left), "shift" pixels aside
	bool wallAhead(ska::EntityId entity, int direction, float shift) const;
	// Against a wall in a direction : the side (-1, 1 : along the axis across) where its corner is near, 0 : none
	int cornerSide(ska::EntityId entity, int direction) const;
	// Position (PositionComponent) of the next block in a direction, when it can be walked to
	std::optional<ska::Point<float>> nextBlock(ska::EntityId entity, int direction) const;
	void place(ska::EntityId entity, ska::Point<float> position, ska::Point<float> velocity);
	// The next step of a character not controlled by the player (wander, path)
	void startAutomaticStep(ska::EntityId entity, GridMovementComponent& grid, unsigned int ellapsedTime, bool walking);
	// A wandering character with a reaction (approach, avoid) towards a character in sight : its step. False : none in sight
	bool react(ska::EntityId entity, GridMovementComponent& grid, unsigned int ellapsedTime);
	// The wall in a direction is touched (as the physics does : the touch scripts of its block)
	void touch(ska::EntityId entity, int direction);
	bool step(ska::EntityId entity, GridMovementComponent& grid, int direction, unsigned int duration);
	// The next forced step (GridMovementComponent::forcedSteps), through the obstacles
	void forcedStep(ska::EntityId entity, GridMovementComponent& grid);

	ska::EntityManager& m_entities;
	PokemonGameEventDispatcher& m_ged;
	WorldState& m_worldState;
	ska::InputToggleContainer m_toggles;
	ska::InputToggleContainer m_receivedToggles;
	bool m_received = false;
	// The jump key was pressed since the last frame
	bool m_jumpPressed = false;
	// The last step was refused by a wall (not a character), by a character standing there (0 : none)
	mutable bool m_bumped = false;
	mutable std::optional<ska::EntityId> m_blockedBy;
	BlockOccupants m_occupants;
	// Seeded from the engine seed (SKA_SEED : the same moves at each run)
	std::mt19937 m_random { ska::NumberUtils::newSeed() };
};
