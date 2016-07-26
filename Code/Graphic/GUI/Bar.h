#pragma once

#include <iostream>
#include <string>
#include "../../ska/Graphic/Texture.h"
#include "../../ska/Utils/Observer.h"
#include "../../ska/ECS/ECSDefines.h"
#include "../../ska/Graphic/Draw/DrawableFixedPriority.h"

class SpriteBank;

namespace ska {
	class EntityManager;
	class CameraSystem;
}

template <typename T>
struct RawStatistics;

using StatisticsChangeObserver = ska::Observer<const ska::EntityId&, RawStatistics<int>&, const ska::EntityId&>;
using StatisticsChangeObservable = ska::Observable<const ska::EntityId&, RawStatistics<int>&, const ska::EntityId&>;

//HP Bar, PP Bar, etc...
class Bar : public StatisticsChangeObserver, public ska::DrawableFixedPriority {
public:
	Bar(StatisticsChangeObservable& obs, ska::CameraSystem& cam, const std::string& barStyleName, const std::string& barContentName, int maxValue, ska::EntityManager& em, const ska::EntityId& entityId);
	~Bar();

    void display() const;
    bool isVisible() const;

	void onStatisticsChange(const ska::EntityId& target, RawStatistics<int>& targetStats, const ska::EntityId& src);
	void setCurrentValue(unsigned int v);

    void setVisible(bool x);

private:
	ska::EntityManager& m_entityManager;
	const ska::EntityId& m_entityId;
	StatisticsChangeObservable& m_observable;

	ska::CameraSystem& m_cameraSystem;
	ska::Texture m_barStyle;
	ska::Texture m_barContent;
	ska::Rectangle m_barSize;
    unsigned int m_maxValue;

	unsigned int m_curValue;
    bool m_visible;
};

using BarPtr = std::unique_ptr<Bar>;

