#pragma once
#include "Script/Command/AbstractFunctionCommand.h"

#include "../../Gameplay/World/WorldScene.h"
//#include "Core/Scene/SceneSwitcher.h"
#include "Utils/Observable.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"

namespace ska {
	class World;
}


class CommandTeleport :
	public ska::AbstractFunctionCommand
{
public:
	CommandTeleport(const ska::World& w, ska::EntityManager& entityManager, PokemonGameEventDispatcher& ged);
	virtual ~CommandTeleport();

	static void teleportHeroToMap(ska::World& w, std::string param);

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();

private:
	//SceneChangeObservable& m_sceneChanger;
	const ska::World& m_world;
	PokemonGameEventDispatcher& m_ged;
};

