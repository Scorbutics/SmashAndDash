#pragma once

#include <memory>
#include <unordered_map>
#include "Base/Patterns/Observer.h"
#include "Core/ECS/ECSDefines.h"

#include "../../Gameplay/PokemonGameEventDispatcher.h"
#include "../../Gameplay/Fight/BattleEvent.h"
#include "../../Gameplay/Fight/StatisticsChangeEvent.h"
#include "SkillsBar.h"

class Bar;
using BarPtr = std::unique_ptr<Bar>;

namespace ska {
	class CameraSystem;
	class EntityManager;
	class Window;
	class InputContextManager;
	class DrawableContainer;
}

using StatisticsChangeObserver = ska::Observer<StatisticsChangeEvent>;
using BattleStartObserver = ska::Observer<BattleEvent>;

class GUIBattle :
	public StatisticsChangeObserver,
	public BattleStartObserver  {
public:
	GUIBattle(PokemonGameEventDispatcher& ged);
	virtual ~GUIBattle();

	GUIBattle& operator=(const GUIBattle&) = delete;

	void clear();
	void removeHPBar(const ska::EntityId& entity);
	bool onBattleStart(BattleEvent& be);
	bool onStatisticsChange(StatisticsChangeEvent& sce);

	void graphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables);

private:
	void addHPBar(ska::CameraSystem& camSys, unsigned int maxValue, unsigned int currentValue, ska::EntityManager& em, const ska::EntityId& entityId);

	std::unordered_map<ska::EntityId, BarPtr> m_bars;
	PokemonGameEventDispatcher& m_ged;
	SkillsBar* m_skillsBar;
};
