#pragma once
#include <random>
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

// Movement of the characters on the map, block by block (as the classic Pokemon games). The player (mode Keys) :
// - 4 directions : holding a key walks one block after another (a step : WalkStep ms, RunStep ms when running : the run key,
//   or the "run" movement mode), a short press towards another direction only turns the character
// - a step starts only when the next block is practicable and free (no character on it, nor going to it) : nothing is pushed
// - the character is placed at each frame (its physics body follows), exactly on the block at the end of the step
// The free movement of the keys (InputSystem) is disabled for these characters (their move power is 0 on the map : the
// power of their normal walk is kept by MovementModeComponent, ex : for the battles). Characters moved by a script
// (IADefinedMovementComponent) or without InputComponent (ex : disabled during a cutscene) do not take steps.
// The other characters wander (random steps after random waits) or walk a path, with the same rules (they wait when the
// next block is not free).
class GridMovementSystem :
	public ska::System<ska::RequiredComponent<GridMovementComponent, ska::PositionComponent, ska::HitboxComponent, ska::ForceComponent, ska::AnimationComponent>,
		ska::PossibleComponent<ska::IADefinedMovementComponent, MovementModeComponent, ska::InputComponent>>,
	public ska::SubObserver<ska::InputKeyEvent> {
public:
	static constexpr unsigned int WalkStep = 250;
	static constexpr unsigned int RunStep = 130;
	// A press shorter than this, towards a new direction, only turns the character
	static constexpr unsigned int TurnDelay = 90;

	GridMovementSystem(ska::EntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState);
	~GridMovementSystem() override = default;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	bool onKeyInputEvent(ska::InputKeyEvent& event);
	// Direction held (-1 : none), the current facing first when it is held
	int heldDirection(int facing) const;
	// Position (PositionComponent) of the next block in a direction, when it can be walked to
	std::optional<ska::Point<float>> nextBlock(ska::EntityId entity, int direction) const;
	void place(ska::EntityId entity, ska::Point<float> position, ska::Point<float> velocity);
	// Turns or starts a step towards the direction held
	void startStep(ska::EntityId entity, GridMovementComponent& grid, bool locked, bool running, unsigned int ellapsedTime, bool walking);
	// The next step of a character not controlled by the player (wander, path)
	void startAutomaticStep(ska::EntityId entity, GridMovementComponent& grid, unsigned int ellapsedTime, bool walking);
	bool step(ska::EntityId entity, GridMovementComponent& grid, int direction, unsigned int duration);

	ska::EntityManager& m_entities;
	WorldState& m_worldState;
	ska::InputToggleContainer m_toggles;
	ska::InputToggleContainer m_receivedToggles;
	bool m_received = false;
	// The last step was refused by a wall (not a character)
	mutable bool m_bumped = false;
	BlockOccupants m_occupants;
	std::mt19937 m_random { std::random_device{}() };
};
