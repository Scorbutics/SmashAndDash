#include <sstream>
#include "Game/__internalConfig/LoggerConfig.h"
#include "DebugDrawer.h"
#include "Core/ECS/EntityLocator.h"
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "../Script/ScriptConstants.h"
#include "Core/Data/Events/DebugGUIEvent.h"

DebugDrawer::DebugDrawer(ska::GameEventDispatcher& ged, ska::EntityManager& em, ska::EntityLocator& locator, const unsigned int blockSize):
	m_eventDispatcher(ged),
	m_entityManager(em),
	m_entityLocator(locator),
	m_blockSize(blockSize) {
}

DebugDrawer::~DebugDrawer() {	
	auto dbgGuiEvent = ska::DebugGUIEvent{ ska::DebugGUIEventType::REMOVE_DEBUG_INFO };
	for (const auto& label : LABEL_CONSTANTS) {
		dbgGuiEvent.text = label;
		m_eventDispatcher.ska::Observable<ska::DebugGUIEvent>::notifyObservers(dbgGuiEvent);
	}
}

void DebugDrawer::setupUI() {
	{
		auto dbgGuiEvent = ska::DebugGUIEvent{ ska::DebugGUIEventType::ADD_DEBUG_INFO };
		dbgGuiEvent.refresher = [&]() {
			std::stringstream ss;
			ss << m_fps;
			return ss.str();
		};

		dbgGuiEvent.delay = 1000;
		dbgGuiEvent.text = FRAMES_PER_SECOND;
		m_eventDispatcher.ska::Observable<ska::DebugGUIEvent>::notifyObservers(dbgGuiEvent);
	}

	{
		auto dbgGuiEvent = ska::DebugGUIEvent{ ska::DebugGUIEventType::ADD_DEBUG_INFO };
		dbgGuiEvent.refresher = [&]() {
			std::stringstream ss;
			ss << m_physicsPerSecond;
			return ss.str();
		};

		dbgGuiEvent.delay = 1000;
		dbgGuiEvent.text = PHYSICS_PER_SECOND;
		m_eventDispatcher.ska::Observable<ska::DebugGUIEvent>::notifyObservers(dbgGuiEvent);
	}

	{
		auto dbgGuiEvent = ska::DebugGUIEvent{ ska::DebugGUIEventType::ADD_DEBUG_INFO };
		dbgGuiEvent.refresher = [&]() {
			std::stringstream ss;
			ss << (m_posHero.x / m_blockSize) << "; " << (m_posHero.y / m_blockSize) << " (";
			ss << m_posHero.x << "; " << m_posHero.y << ")";
			return ss.str();
		};

		dbgGuiEvent.delay = 200;
		dbgGuiEvent.text = HERO_POSITION;
		m_eventDispatcher.ska::Observable<ska::DebugGUIEvent>::notifyObservers(dbgGuiEvent);
	}
}

void DebugDrawer::graphicUpdate(unsigned int ellapsedTime) {
	m_fps = m_fpsCalculator.calculate(ellapsedTime);
}

ska::Point<int> DebugDrawer::getPosition() const {
	return m_posHero;
}

void DebugDrawer::eventUpdate(unsigned int ellapsedTime) {
	const auto* trainer = m_entityLocator.getEntityId(SCRIPT_ENTITY_TRAINER);
	if (trainer != nullptr) {
		const auto& pc = m_entityManager.getComponent<ska::PositionComponent>(*trainer);
		const auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(*trainer);
		m_posHero = ska::PositionComponent::getCenterPosition(pc, hc);
	}
	m_physicsPerSecond = m_physicsCalculator.calculate(ellapsedTime);
}
