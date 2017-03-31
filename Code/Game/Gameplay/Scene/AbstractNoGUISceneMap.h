#pragma once
#include "Core/Scene/SceneBase.h"
#include "Core/Scene/Scene.h"
#include "../PokemonGameEventDispatcher.h"

class CustomEntityManager;

class AbstractNoGUISceneMap :
	public ska::SceneBase<CustomEntityManager, PokemonGameEventDispatcher> {

public:
	AbstractNoGUISceneMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::SceneHolder& sh);
	AbstractNoGUISceneMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::Scene& oldScene);
	AbstractNoGUISceneMap& operator=(const AbstractNoGUISceneMap&) = delete;

	virtual void load(ska::ScenePtr* lastScene) override;
	virtual bool unload() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(unsigned int ellapsedTime) override;
	virtual ~AbstractNoGUISceneMap();

};

