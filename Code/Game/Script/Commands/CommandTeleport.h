#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
#include "../../Gameplay/World/WorldState.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"

namespace ska {
	class TileWorld;
	class EntityLocator;
}

class CommandTeleport :
	public ska::AbstractFunctionCommand {
public:
	CommandTeleport(const ska::TileWorld& w, const ska::EntityLocator& locator, ska::EntityManager& entityManager, PokemonGameEventDispatcher& ged);
	virtual ~CommandTeleport() = default;

	static void teleportHeroToMap(ska::TileWorld& w, std::string param);

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memoryScript, const std::vector<std::string>& args) override;
	virtual int argumentsNumber();

private:
	const ska::TileWorld& m_world;
	PokemonGameEventDispatcher& m_ged;
	const ska::EntityLocator& m_locator;
};

