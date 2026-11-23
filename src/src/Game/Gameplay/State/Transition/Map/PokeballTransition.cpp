#include "Game/__internalConfig/LoggerConfig.h"
#include "PokeballTransition.h"
#include "../../../CustomEntityManager.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "../../../PokeballComponent.h"
#include "Graphic/GraphicComponent.h"
#include "Core/Utils/TimeUtils.h"

namespace {
	// The Pokemon comes out this long after the pokeball opened (its vortex shows)
	constexpr unsigned int AppearDelay = 700;
	// The longest throw : the Pokemon comes out anyway (the battle never waits for a lost pokeball)
	constexpr unsigned int MaxDuration = 4000;
}

PokeballTransition::PokeballTransition(unsigned int, CustomEntityManager& em, const ska::EntityId& destinationEntity, const ska::EntityId& sourceEntity, ska::EntityId& pokeballHolder, std::optional<ska::Point<int>> destination) :
	ska::Task(std::bind(&PokeballTransition::run, this, std::placeholders::_1)),
	m_pokeball(pokeballHolder),
	m_loadState(0),
	m_trainerId(sourceEntity),
	m_pokemonId(destinationEntity),
	m_entityManager(em),
	m_destination(std::move(destination)) {

}

bool PokeballTransition::run(ska::Task&){
	if (m_loadState == 0) {
		m_loadState++;

		/* Création d'une entité : Pokéball (Position + Pokeball) */
		m_pokeball = m_entityManager.createEntity();
		auto& pokemonPc = m_entityManager.getComponent<ska::PositionComponent>(m_pokemonId);
		auto& pokemonHp = m_entityManager.getComponent<ska::HitboxComponent>(m_pokemonId);
		auto& pc = m_entityManager.getComponent<ska::PositionComponent>(m_trainerId);
		auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(m_trainerId);
		PokeballComponent pokeballc;

		const auto pokemonPosition = m_destination.value_or(ska::Point<int>(static_cast<int>(pokemonPc.x), static_cast<int>(pokemonPc.y)));
		pokeballc.finalPos = { static_cast<int>(pokemonPosition.x + pokemonHp.xOffset + pokemonHp.width / 2), static_cast<int>(pokemonPosition.y + pokemonHp.yOffset + pokemonHp.height / 2) };
		m_entityManager.addComponent<PokeballComponent>(m_pokeball, std::move(pokeballc));
		m_entityManager.addComponent<ska::GraphicComponent>(m_pokeball, ska::GraphicComponent());
		auto pokePc = pc;
		pokePc.x += hc.xOffset + hc.width / 2;
		pokePc.y += hc.yOffset + hc.height / 2;
		m_entityManager.addComponent<ska::PositionComponent>(m_pokeball, std::move(pokePc));
		m_thrownAt = ska::TimeUtils::getTicks();

		return true;
	}
	// Until the pokeball opened, and the Pokemon appears
	const auto now = ska::TimeUtils::getTicks();
	if (!m_entityManager.hasComponent<PokeballComponent>(m_pokeball) || now - m_thrownAt >= MaxDuration) {
		return false;
	}
	if (!m_entityManager.getComponent<PokeballComponent>(m_pokeball).opened()) {
		return true;
	}
	if (m_openedAt == 0) {
		m_openedAt = now;
	}
	return now - m_openedAt < AppearDelay;
}
