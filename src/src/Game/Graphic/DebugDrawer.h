#pragma once
#include "Core/Data/Events/GameEventDispatcher.h"
#include "Graphic/TicksCalculator.h"

namespace ska {
	class EntityManager;
	class EntityLocator;
}

class DebugDrawer {
public:
	DebugDrawer(ska::GameEventDispatcher& ged, ska::EntityManager& em, ska::EntityLocator& locator, unsigned int blockSize);
	~DebugDrawer();
	
	void setupUI();
	void eventUpdate(unsigned int ellapsedTime);
	void graphicUpdate(unsigned int ellapsedTime);

	ska::Point<int> getPosition() const;

private:
	ska::TicksCalculator m_fpsCalculator;
	ska::TicksCalculator m_physicsCalculator;
	ska::GameEventDispatcher& m_eventDispatcher;
	ska::EntityManager& m_entityManager;
	ska::EntityLocator& m_entityLocator;
	
	const unsigned int m_blockSize;
	ska::Point<int> m_posHero;
	double m_fps;
	double m_physicsPerSecond;

	static constexpr auto PHYSICS_PER_SECOND = "Physics : ";
	static constexpr auto FRAMES_PER_SECOND = "FPS : ";
	static constexpr auto HERO_POSITION = "Hero Pos : ";

	static constexpr const char* LABEL_CONSTANTS[] = { FRAMES_PER_SECOND, PHYSICS_PER_SECOND, HERO_POSITION };
};