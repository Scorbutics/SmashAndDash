#include "Game/__internalConfig/LoggerConfig.h"
#include "StateFightLoading.h"
#include "../Fight/FighterComponent.h"
#include "../Fight/BattleComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "../Fight/BattleEvent.h"
#include "../PokeballComponent.h"
#include "../World/WorldState.h"
#include "../CustomEntityManager.h"
#include "Physic/Space.h"
#include "Physic/Body.h"
#include "Physic/Vect.h"

StateFightLoading::StateFightLoading(CustomEntityManager& em, PokemonGameEventDispatcher& ged, WorldState& ws, ska::EntityId pokemonId, ska::EntityId opponentId, ska::EntityId trainerId, const ska::Point<float>& landing, ska::InputComponent** ic) :
	m_eventDispatcher(ged),
	m_entityManager(em),
	m_pokemonId(pokemonId),
	m_opponentId(opponentId),
	m_trainerId(trainerId),
	m_landing(landing),
	m_ic(ic),
	m_worldState(ws) {

}

void StateFightLoading::setFighters(BattleComponent pokemon, BattleComponent opponent) {
	m_pokemonFighter = std::move(pokemon);
	m_opponentFighter = std::move(opponent);
}

std::unique_ptr<ska::Task> StateFightLoading::load() {
	return std::make_unique<ska::Task>([&](ska::Task&) {
		/* Ajout InputComponent au Pokémon,
		Ajout d'un IAMovementComponent au dresseur (m_player),
		Ajout d'un composant de combat au Pokémon
		Ajout d'une HP Bar */

		m_entityManager.addComponent<BattleComponent>(m_pokemonId, BattleComponent{ m_pokemonFighter });
		m_entityManager.addComponent<BattleComponent>(m_opponentId, BattleComponent{ m_opponentFighter });

		// The trainer stays where it is : walking around its Pokemon, it would push the fighters (physics bodies)
		m_entityManager.addComponent<ska::InputComponent>(m_pokemonId, ska::InputComponent{ **m_ic });
		*m_ic = nullptr;

		// The Pokemon comes out where the pokeball opened (its feet : the middle of its hitbox there)
		auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(m_pokemonId);
		auto pc = ska::PositionComponent{};
		pc.x = m_landing.x - static_cast<float>(hc.xOffset) - static_cast<float>(hc.width) / 2.F;
		pc.y = m_landing.y - static_cast<float>(hc.yOffset) - static_cast<float>(hc.height) / 2.F;
		ska::Rectangle hitbox{ pc.x + hc.xOffset, pc.y + hc.yOffset, static_cast<int>(hc.width), static_cast<int>(hc.height) };

		const auto targetBlock = m_worldState.getWorld().getCollisionProfile().placeOnNearestPracticableBlock(1, hitbox, 1);
		pc.x = targetBlock.x - hc.xOffset;
		pc.y = targetBlock.y - hc.yOffset;
		// Its physics body moves with it
		CustomEntityManager::place(m_entityManager, m_worldState.getSpace(), m_pokemonId, ska::Point<float>(static_cast<float>(pc.x), static_cast<float>(pc.y)));
		m_entityManager.addComponent<ska::PositionComponent>(m_pokemonId, std::move(pc));
		// It grows out of the light
		if (m_entityManager.hasComponent<FighterComponent>(m_pokemonId)) {
			m_entityManager.getComponent<FighterComponent>(m_pokemonId).appear = 0;
		}

		auto be = BattleEvent{ BATTLE_START };
		m_eventDispatcher.ska::Observable<BattleEvent>::notifyObservers(be);

		return false;
	});
}

std::unique_ptr<ska::Task> StateFightLoading::unload() {
	return std::make_unique<ska::Task>([&](ska::Task&) {
		// A copy : the storage of a removed component is reset
		m_input = m_entityManager.getComponent<ska::InputComponent>(m_pokemonId);
		*m_ic = &m_input;
		m_entityManager.removeComponent<ska::InputComponent>(m_pokemonId);

		auto be = BattleEvent{ BATTLE_END };
		m_eventDispatcher.ska::Observable<BattleEvent>::notifyObservers(be);

		m_entityManager.removeComponent<BattleComponent>(m_pokemonId);
		m_entityManager.removeComponent<BattleComponent>(m_opponentId);
		m_entityManager.removeComponent<FighterComponent>(m_pokemonId);
		m_entityManager.removeComponent<FighterComponent>(m_opponentId);

		if (m_entityManager.hasComponent<ska::AnimationComponent>(m_trainerId)) {
			auto& dac = m_entityManager.getComponent<ska::AnimationComponent>(m_trainerId);
			/*dac.type = ska::AnimationComponent::MOVEMENT;
			dac.looked = 0;*/
		}
		return false;
	});
}

