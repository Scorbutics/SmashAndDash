#pragma once

#include <memory>
#include <unordered_map>
#include "../../ska/Utils/Observer.h"
#include "../../ska/Utils/Observable.h"
#include "../../ska/ECS/ECSDefines.h"
#include "../../ska/Scene/HasGraphic.h"
#include "../../ska/Scene/HasLogic.h"

#include "../../Gameplay/PokemonGameEventDispatcher.h"
#include "../../Gameplay/Fight/BattleEvent.h"
#include "../../Gameplay/Fight/StatisticsChangeEvent.h"
#include "DialogMenu.h"

class Bar;
using BarPtr = std::unique_ptr<Bar>;

namespace ska {
	class CameraSystem;
	class EntityManager;
	class Window;
	class InputContextManager;
}

using StatisticsChangeObserver = ska::Observer<StatisticsChangeEvent>;
using BattleStartObserver = ska::Observer<BattleEvent>;

class GUIBattle : public ska::HasGraphic, public ska::HasLogic, public StatisticsChangeObserver, public BattleStartObserver  {
public:
	GUIBattle(ska::Window& w, const ska::InputContextManager& playerICM, PokemonGameEventDispatcher& ged);
	~GUIBattle();

	void clear();
	bool onBattleStart(BattleEvent& be);
	bool onStatisticsChange(StatisticsChangeEvent& sce);
	
	void graphicUpdate(ska::DrawableContainer& drawables) override;
	void eventUpdate(bool movingDisallowed) override;

private:
	void addHPBar(ska::CameraSystem& camSys, unsigned int maxValue, unsigned int currentValue, ska::EntityManager& em, const ska::EntityId& entityId);

	const ska::InputContextManager& m_playerICM;

	std::unordered_map<ska::EntityId, BarPtr> m_bars;
	PokemonGameEventDispatcher& m_ged;
	//DialogMenu m_moves;
};