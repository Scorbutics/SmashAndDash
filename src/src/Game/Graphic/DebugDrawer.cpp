#include <sstream>
#include "Game/__internalConfig/LoggerConfig.h"
#include "DebugDrawer.h"
#include "Core/ECS/EntityLocator.h"
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "../Script/ScriptConstants.h"
#include "Core/Data/Events/DebugGUIEvent.h"
#include <iomanip>
#include <limits>
#include "Graphic/GraphicComponent.h"
#include "Graphic/System/GraphicSystem.h"
#include "../Gameplay/Fight/FightComponent.h"
#include "../Gameplay/Mobs/WildPokemonComponent.h"

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

	{
		auto dbgGuiEvent = ska::DebugGUIEvent{ ska::DebugGUIEventType::ADD_DEBUG_INFO };
		dbgGuiEvent.refresher = [&]() { return wildLine(); };
		dbgGuiEvent.delay = 200;
		dbgGuiEvent.text = NEAREST_WILD;
		m_eventDispatcher.ska::Observable<ska::DebugGUIEvent>::notifyObservers(dbgGuiEvent);
	}
}

std::string DebugDrawer::wildLine() const {
	// The nearest wild Pokemon : its monster, its block, its height ; its drawing : how many sprites, its opacity, its
	// scale, its crop and its silhouette, the offset of its sprite, whether the GraphicSystem draws it
	auto nearest = std::numeric_limits<long>::max();
	const ska::EntityId* found = nullptr;
	auto best = ska::EntityId{};
	for (const auto entity : m_entityManager.entities()) {
		if (!m_entityManager.hasComponent<WildPokemonComponent>(entity) || !m_entityManager.hasComponent<ska::PositionComponent>(entity)) {
			continue;
		}
		const auto& pc = m_entityManager.getComponent<ska::PositionComponent>(entity);
		const auto distance = std::abs(pc.x - m_posHero.x) + std::abs(pc.y - m_posHero.y);
		if (distance < nearest) {
			nearest = distance;
			best = entity;
			found = &best;
		}
	}
	if (found == nullptr) {
		return "none";
	}
	const auto& pc = m_entityManager.getComponent<ska::PositionComponent>(best);
	std::stringstream ss;
	if (m_entityManager.hasComponent<FightComponent>(best)) {
		ss << "#" << m_entityManager.getComponent<FightComponent>(best).opponentScriptId << " ";
	}
	ss << "(" << pc.x / static_cast<long>(m_blockSize) << "; " << pc.y / static_cast<long>(m_blockSize) << ") z " << pc.z;
	if (!m_entityManager.hasComponent<ska::GraphicComponent>(best)) {
		ss << " no drawing";
		return ss.str();
	}
	const auto& gc = m_entityManager.getComponent<ska::GraphicComponent>(best);
	ss << " spr " << gc.animatedSprites.size() << " a " << static_cast<int>(gc.effect.alpha) << " x" << std::fixed << std::setprecision(2) << gc.effect.scaleX
		<< " crop " << gc.cropBottom << " sil " << static_cast<int>(gc.silhouette) << " off " << gc.drawOffset.x << "," << gc.drawOffset.y;
	if (m_graphicSystem != nullptr) {
		ss << " drawn " << (m_graphicSystem->draws(best) ? "yes" : "NO");
	}
	return ss.str();
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
