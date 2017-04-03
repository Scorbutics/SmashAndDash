#pragma once
#include "Core/Scene/SceneBase.h"
#include "../CustomEntityManager.h"
#include "../PokemonGameEventDispatcher.h"
#include "../../Graphic/GUI/GUIBattle.h"

class Settings;

class SceneGUIBattle : public ska::SceneBase<CustomEntityManager, PokemonGameEventDispatcher> {
public:
    SceneGUIBattle(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::SceneHolder& sh);
    //void bindGUI(Settings& s);
    virtual ~SceneGUIBattle() = default;

protected:
	virtual void onGraphicUpdate(ska::DrawableContainer& drawables) override;
	virtual bool beforeUnload() override;
	virtual void onEventUpdate(unsigned int ellapsedTime) override;

private:
	/* TODO GUI Battle specific part with Bars and also skills displayed */
	GUIBattle m_gui;
};
