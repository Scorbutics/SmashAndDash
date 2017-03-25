#pragma once
#include "Scene/SceneSwitcher.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"
#include "../PokemonGameEventDispatcherDeclaration.h"

class WorldScene;

class SceneToMapSwitcher : public ska::SceneSwitcher<WorldScene&> {
public:
	SceneToMapSwitcher(const std::string& map, const std::string& chipset, PokemonGameEventDispatcher& ged);
	virtual ~SceneToMapSwitcher() = default;
	void switchTo(ska::Window& w, ska::SceneHolder& holder, ska::Scene& lastScene, ska::InputContextManager& icm, WorldScene& ws) const override;
private:
	PokemonGameEventDispatcher& m_ged;
};
