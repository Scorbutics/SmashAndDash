#pragma once
#include "Core/Scene/SceneBase.h"
#include "../CustomEntityManager.h"
#include "../PokemonGameEventDispatcher.h"
#include "../../Graphic/GUI/GUIMap.h"

class Settings;

class SceneGUIMap : public ska::SceneBase<CustomEntityManager, PokemonGameEventDispatcher> {
public:
    SceneGUIMap(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::SceneHolder& sh);
    void bindGUI(Settings& s);
    virtual ~SceneGUIMap() = default;

protected:
	virtual void onGraphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void onEventUpdate(unsigned int ellapsedTime) override;

private:
    GUIMap m_gui;
};
