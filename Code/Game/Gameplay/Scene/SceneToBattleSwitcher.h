#pragma once
#include "Core/Scene/SceneSwitcher.h"
#include "Point.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"
#include "../PokemonGameEventDispatcherDeclaration.h"

class FightComponent;
class WorldScene;

class SceneToBattleSwitcher : public ska::SceneSwitcher<WorldScene&> {
public:
	SceneToBattleSwitcher(const std::string& map, const std::string& chipset, PokemonGameEventDispatcher& ged, ska::Point<int> fightPos, const FightComponent& fc);
	virtual ~SceneToBattleSwitcher() = default;
	void switchTo(ska::Window& w, ska::SceneHolder& holder, ska::Scene& lastScene, ska::InputContextManager& icm, WorldScene& ws) const override;
private:
	const FightComponent& m_fightComponent;
	ska::Point<int> m_fightPos;
	PokemonGameEventDispatcher& m_ged;
};
