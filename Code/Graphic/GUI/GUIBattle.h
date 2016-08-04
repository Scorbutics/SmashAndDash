#pragma once

#include <memory>
#include <unordered_map>
#include "../../ska/Utils/Observer.h"
#include "../../ska/Utils/Observable.h"
#include "../../ska/ECS/ECSDefines.h"
#include "../../ska/Scene/HasGraphic.h"
#include "../../ska/Scene/HasLogic.h"
#include "DialogMenu.h"

class Bar;
using BarPtr = std::unique_ptr<Bar>;

namespace ska {
	class CameraSystem;
	class EntityManager;
	class Window;
}

template <typename T>
struct RawStatistics;

class SkillsHolderComponent;

using StatisticsChangeObserver = ska::Observer<const ska::EntityId&, RawStatistics<int>&, const ska::EntityId&>;
using StatisticsChangeObservable = ska::Observable<const ska::EntityId&, RawStatistics<int>&, const ska::EntityId&>;

using BattleStartObserver = ska::Observer<const ska::EntityId&, SkillsHolderComponent&>;
using BattleStartObservable = ska::Observable<const ska::EntityId&, SkillsHolderComponent&>;

class GUIBattle : public ska::HasGraphic, public ska::HasLogic, public StatisticsChangeObserver, public BattleStartObserver  {
public:
	GUIBattle(ska::Window& w, StatisticsChangeObservable& statObs, BattleStartObservable& battleStartObs);
	~GUIBattle();

	void addHPBar(ska::CameraSystem& camSys, unsigned int maxValue, unsigned int currentValue, ska::EntityManager& em, const ska::EntityId& entityId);

	void clear();
	void onBattleStart(const ska::EntityId& target, SkillsHolderComponent& targetStats);
	void onStatisticsChange(const ska::EntityId& target, RawStatistics<int>& targetStats, const ska::EntityId& src);
	
	void graphicUpdate(ska::DrawableContainer& drawables) override;
	void eventUpdate(bool movingDisallowed) override;

private:
	std::unordered_map<ska::EntityId, BarPtr> m_bars;
	StatisticsChangeObservable& m_statsObservable;
	BattleStartObservable& m_battleStartObservable;
	DialogMenu m_moves;
};