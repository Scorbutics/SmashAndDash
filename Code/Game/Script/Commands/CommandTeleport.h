#pragma once
#include "Script/Command/AbstractFunctionCommand.h"

#include "../../Gameplay/World/WorldScene.h"
#include "Scene/SceneSwitcher.h"
#include "Utils/Observable.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"

namespace ska {
	class World;
}

using SceneChangeObservable = ska::Observable<ska::SceneSwitcher<WorldScene&>>;

class CommandTeleport :
	public ska::AbstractFunctionCommand
{
public:
	CommandTeleport(const ska::World& w, SceneChangeObservable& sceneChanger, ska::EntityManager& entityManager, PokemonGameEventDispatcher& ged);
	virtual ~CommandTeleport();

	static void teleportHeroToMap(ska::World& w, std::string param);

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();

private:
	SceneChangeObservable& m_sceneChanger;
	const ska::World& m_world;
	PokemonGameEventDispatcher& m_ged;
};

