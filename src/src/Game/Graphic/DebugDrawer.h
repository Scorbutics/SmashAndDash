#pragma once
#include <string>
#include "Core/Data/Events/GameEventDispatcher.h"
#include "Graphic/TicksCalculator.h"

namespace ska {
	class EntityManager;
	class EntityLocator;
	class GraphicSystem;
}

class DebugDrawer {
public:
	DebugDrawer(ska::GameEventDispatcher& ged, ska::EntityManager& em, ska::EntityLocator& locator, unsigned int blockSize);
	~DebugDrawer();
	
	void setupUI();
	void eventUpdate(unsigned int ellapsedTime);
	void graphicUpdate(unsigned int ellapsedTime);

	ska::Point<int> getPosition() const;
	// The system that draws the sprites (the line of the nearest wild Pokemon tells whether it draws it)
	void setGraphicSystem(const ska::GraphicSystem* graphicSystem) { m_graphicSystem = graphicSystem; }

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
	const ska::GraphicSystem* m_graphicSystem = nullptr;
	// The nearest wild Pokemon and the state of its drawing (a Pokemon not seen : what is wrong with its drawing)
	std::string wildLine() const;

	static constexpr auto PHYSICS_PER_SECOND = "Physics : ";
	static constexpr auto FRAMES_PER_SECOND = "FPS : ";
	static constexpr auto HERO_POSITION = "Hero Pos : ";
	static constexpr auto NEAREST_WILD = "Wild : ";

	static constexpr const char* LABEL_CONSTANTS[] = { FRAMES_PER_SECOND, PHYSICS_PER_SECOND, HERO_POSITION, NEAREST_WILD };
};