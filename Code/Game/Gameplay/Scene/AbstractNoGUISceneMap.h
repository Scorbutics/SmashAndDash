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
	virtual ~AbstractNoGUISceneMap();

protected:
	virtual void onGraphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void onEventUpdate(unsigned int ellapsedTime) override;

};

