#pragma once

#include <memory>
#include <unordered_map>
#include "../../ska/Utils/Observer.h"
#include "../../ska/Utils/Observable.h"
#include "../../ska/ECS/ECSDefines.h"
#include "../../ska/Scene/HasGraphic.h"

class Bar;
using BarPtr = std::unique_ptr<Bar>;

namespace ska {
	class CameraSystem;
	class EntityManager;
}

template <typename T>
struct RawStatistics;

using StatisticsChangeObserver = ska::Observer<const ska::EntityId&, RawStatistics<int>&, const ska::EntityId&>;
using StatisticsChangeObservable = ska::Observable<const ska::EntityId&, RawStatistics<int>&, const ska::EntityId&>;

class GUIBattle : public ska::HasGraphic, public StatisticsChangeObserver  {
public:
	GUIBattle(StatisticsChangeObservable& obs);
	~GUIBattle();

	void addHPBar(ska::CameraSystem& camSys, unsigned int maxValue, unsigned int currentValue, ska::EntityManager& em, const ska::EntityId& entityId);

	void clear();
	void onStatisticsChange(const ska::EntityId& target, RawStatistics<int>& targetStats, const ska::EntityId& src);
	void graphicUpdate(ska::DrawableContainer& drawables) override;

private:
	std::unordered_map<ska::EntityId, BarPtr> m_bars;
	StatisticsChangeObservable& m_statsObservable;
};