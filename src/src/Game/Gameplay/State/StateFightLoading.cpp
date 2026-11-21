#include "Game/__internalConfig/LoggerConfig.h"
#include "StateFightLoading.h"
#include "../Fight/BattleComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "../Fight/BattleEvent.h"
#include "../World/WorldState.h"
#include "../CustomEntityManager.h"
#include "Physic/Space.h"
#include "Physic/Body.h"
#include "Physic/Vect.h"

StateFightLoading::StateFightLoading(CustomEntityManager& em, PokemonGameEventDispatcher& ged, WorldState& ws, ska::EntityId pokemonId, ska::EntityId opponentId, ska::EntityId trainerId, ska::EntityId& pokeball, ska::InputComponent** ic, ska::CameraSystem** cameraSystem) :
	m_eventDispatcher(ged),
	m_entityManager(em),
	m_pokemonId(pokemonId),
	m_opponentId(opponentId),
	m_trainerId(trainerId),
	m_pokeball(pokeball),
	m_ic(ic),
	m_worldState(ws),
	m_cameraSystem(cameraSystem) {

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

		auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(m_pokemonId);
		auto& pc = m_entityManager.getComponent<ska::PositionComponent>(m_pokeball);
		m_pokeball = 0;
		ska::Rectangle hitbox{ pc.x + hc.xOffset, pc.y + hc.yOffset, static_cast<int>(hc.width), static_cast<int>(hc.height) };

		const auto targetBlock = m_worldState.getWorld().getCollisionProfile().placeOnNearestPracticableBlock(1, hitbox, 1);
		pc.x = targetBlock.x - hc.xOffset;
		pc.y = targetBlock.y - hc.yOffset;
		// Its physics body moves with it
		CustomEntityManager::place(m_entityManager, m_worldState.getSpace(), m_pokemonId, ska::Point<float>(static_cast<float>(pc.x), static_cast<float>(pc.y)));
		m_entityManager.addComponent<ska::PositionComponent>(m_pokemonId, std::move(pc));

		BattleEvent be(BATTLE_START, **m_cameraSystem, m_pokemonId, m_opponentId, m_entityManager);
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

		BattleEvent be(BATTLE_END, **m_cameraSystem, m_pokemonId, m_opponentId, m_entityManager);
		m_eventDispatcher.ska::Observable<BattleEvent>::notifyObservers(be);

		m_entityManager.removeComponent<BattleComponent>(m_pokemonId);
		m_entityManager.removeComponent<BattleComponent>(m_opponentId);

		if (m_entityManager.hasComponent<ska::AnimationComponent>(m_trainerId)) {
			auto& dac = m_entityManager.getComponent<ska::AnimationComponent>(m_trainerId);
			/*dac.type = ska::AnimationComponent::MOVEMENT;
			dac.looked = 0;*/
		}
		return false;
	});
}

