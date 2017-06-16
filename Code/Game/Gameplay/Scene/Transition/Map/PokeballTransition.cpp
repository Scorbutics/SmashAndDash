#include "PokeballTransition.h"
#include "../../../Fight/BattleComponent.h"
#include "../../../CustomEntityManager.h"
#include "ECS/Basics/Graphic/DirectionalAnimationComponent.h"
#include "ECS/Basics/Graphic/DialogComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "../../../PokeballComponent.h"

PokeballTransition::PokeballTransition(unsigned int delay, CustomEntityManager& em, const ska::EntityId& destinationEntity, const ska::EntityId& sourceEntity) :
    ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>([&, delay](ska::Task<bool, ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>& t) {
		if (m_loadState == 0) {
			m_loadState++;

			/* Création d'une entité : Pokéball (Position + Pokeball) */
			m_pokeball = m_entityManager.createEntity();
			auto& pokemonPc = m_entityManager.getComponent<ska::PositionComponent>(m_pokemonId);
			auto& pokemonHp = m_entityManager.getComponent<ska::HitboxComponent>(m_pokemonId);
			auto& pc = m_entityManager.getComponent<ska::PositionComponent>(m_trainerId);
			auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(m_trainerId);
			PokeballComponent pokeballc;

			pokeballc.finalPos = { static_cast<int>(pokemonPc.x + pokemonHp.xOffset + pokemonHp.width / 2), static_cast<int>(pokemonPc.y + pokemonHp.yOffset + pokemonHp.height / 2) };
			m_entityManager.addComponent<PokeballComponent>(m_pokeball, pokeballc);
			ska::PositionComponent pokePc;
			pokePc = pc;
			pokePc.x += hc.xOffset + hc.width / 2;
			pokePc.y += hc.yOffset + hc.height / 2;
			m_entityManager.addComponent<ska::PositionComponent>(m_pokeball, pokePc);

            auto& ic = m_entityManager.getComponent<ska::InputComponent>(m_pokemonId);
			t.forward(ic);
			return true;
		}
		// Continue until pokeball is still visible
		return m_entityManager.hasComponent<PokeballComponent>(m_pokeball);
	}),
	m_pokeball(0),
	m_loadState(0),
	m_trainerId(sourceEntity),
	m_pokemonId(destinationEntity),
	m_entityManager(em) {

}
