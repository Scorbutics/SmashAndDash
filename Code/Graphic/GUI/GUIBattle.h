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
	class InputContextManager;
}

template <typename T>
struct RawStatistics;

using StatisticsChangeObserver = ska::Observer<const ska::EntityId&, RawStatistics<int>&, const ska::EntityId&>;
using StatisticsChangeObservable = ska::Observable<const ska::EntityId&, RawStatistics<int>&, const ska::EntityId&>;

using BattleStartObserver = ska::Observer<ska::CameraSystem&, const ska::EntityId&, const ska::EntityId&, ska::EntityManager&>;
using BattleStartObservable = ska::Observable<ska::CameraSystem&, const ska::EntityId&, const ska::EntityId&, ska::EntityManager&>;

class GUIBattle : public ska::HasGraphic, public ska::HasLogic, public StatisticsChangeObserver, public BattleStartObserver  {
public:
	GUIBattle(ska::Window& w, const ska::InputContextManager& playerICM, StatisticsChangeObservable& statObs, BattleStartObservable& battleStartObs);
	~GUIBattle();

	void clear();
	void onBattleStart(ska::CameraSystem& camSys, const ska::EntityId& pokemon, const ska::EntityId& opponent, ska::EntityManager& em);
	void onStatisticsChange(const ska::EntityId& target, RawStatistics<int>& targetStats, const ska::EntityId& src);
	
	void graphicUpdate(ska::DrawableContainer& drawables) override;
	void eventUpdate(bool movingDisallowed) override;

private:
	void addHPBar(ska::CameraSystem& camSys, unsigned int maxValue, unsigned int currentValue, ska::EntityManager& em, const ska::EntityId& entityId);

	const ska::InputContextManager& m_playerICM;

	std::unordered_map<ska::EntityId, BarPtr> m_bars;
	StatisticsChangeObservable& m_statsObservable;
	BattleStartObservable& m_battleStartObservable;
	DialogMenu m_moves;
};