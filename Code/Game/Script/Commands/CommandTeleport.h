#pragma once
#include "Script/Command/AbstractFunctionCommand.h"

#include "../../Gameplay/World/WorldState.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"

namespace ska {
	class TileWorld;
}


class CommandTeleport :
	public ska::AbstractFunctionCommand
{
public:
	CommandTeleport(const ska::TileWorld& w, ska::EntityManager& entityManager, PokemonGameEventDispatcher& ged);
	virtual ~CommandTeleport();

	static void teleportHeroToMap(ska::TileWorld& w, std::string param);

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memoryScript, std::vector<std::string>& args) override;
	virtual int argumentsNumber();

private:
	//SceneChangeObservable& m_sceneChanger;
	const ska::TileWorld& m_world;
	PokemonGameEventDispatcher& m_ged;
};

