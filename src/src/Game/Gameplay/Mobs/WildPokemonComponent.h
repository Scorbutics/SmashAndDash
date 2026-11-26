#pragma once
#include <optional>
#include <string>
#include "Core/ECS/Component.h"
#include "../Control/GridMovementComponent.h"

// A wild Pokemon on the map (MobSpawner) : its attitude towards the player, decided when it appears
// (Data/Battle/encounters.miniska). The battle starts when the trainer and the wild Pokemon touch (FightStartSystem).
struct WildPokemonComponent {
	// Aggressive : it charges the player in sight. Fearful : it runs away from the player. Passive : it wanders.
	enum class Attitude { Passive, Aggressive, Fearful };
	Attitude attitude = Attitude::Passive;
	// Milliseconds left during which it ignores the player and no battle starts with it (ex : after the player fled)
	unsigned int calm = 0;
	// A bird (Data/Battle/encounters.miniska "arrival" : "fly") : milliseconds left before it flies away ; a fearful one
	// flies away as soon as it sees the player (FightStartSystem, WorldState::takeOff)
	std::optional<int> stay;

	static Attitude FromName(const std::string& name) {
		if (name == "aggressive") {
			return Attitude::Aggressive;
		}
		if (name == "fearful") {
			return Attitude::Fearful;
		}
		return Attitude::Passive;
	}
};

// Its walk towards the player (GridMovementComponent::reaction) for an attitude : it charges a little faster than the
// walk of the player (running, the player gets away), it runs away hesitating (the player can catch it)
inline void ReactToPlayer(GridMovementComponent& grid, WildPokemonComponent::Attitude attitude, ska::EntityId trainer) {
	grid.reaction = GridMovementComponent::Reaction::None;
	grid.reactTo = trainer;
	switch (attitude) {
	case WildPokemonComponent::Attitude::Aggressive:
		grid.reaction = GridMovementComponent::Reaction::Approach;
		grid.sight = 5;
		grid.reactStep = 230;
		break;
	case WildPokemonComponent::Attitude::Fearful:
		grid.reaction = GridMovementComponent::Reaction::Avoid;
		grid.sight = 4;
		grid.reactStep = 190;
		break;
	default:
		break;
	}
}

inline const char* AttitudeName(WildPokemonComponent::Attitude attitude) {
	switch (attitude) {
	case WildPokemonComponent::Attitude::Aggressive: return "aggressive";
	case WildPokemonComponent::Attitude::Fearful: return "fearful";
	default: return "passive";
	}
}

SKA_DECLARE_COMPONENT(WildPokemonComponent);
