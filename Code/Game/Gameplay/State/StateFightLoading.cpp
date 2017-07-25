#include "StateFightLoading.h"
#include "../Fight/BattleComponent.h"
#include "AI/IARandomMovementComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "../Fight/BattleEvent.h"
#include "../World/WorldState.h"
#include "../CustomEntityManager.h"

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

std::unique_ptr<ska::Task> StateFightLoading::load() {
	return std::make_unique<ska::Task>([&](ska::Task&) {
		/* Ajout InputComponent au Pokémon,
		Ajout d'un IAMovementComponent au dresseur (m_player),
		Ajout d'un composant de combat au Pokémon
		Ajout d'une HP Bar */

		m_entityManager.addComponent<BattleComponent>(m_pokemonId, BattleComponent());
		m_entityManager.addComponent<BattleComponent>(m_opponentId, BattleComponent());

		ska::IARandomMovementComponent iarmc;
		iarmc.emitter = m_pokemonId;
		iarmc.delay = 300;
		iarmc.type = ska::RandomMovementType::CIRCLE_AROUND;

		m_entityManager.addComponent<ska::IARandomMovementComponent>(m_trainerId, iarmc);
		m_entityManager.addComponent<ska::InputComponent>(m_pokemonId, **m_ic);
		*m_ic = nullptr;

		auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(m_pokemonId);
		auto& pc = m_entityManager.getComponent<ska::PositionComponent>(m_pokeball);
		m_pokeball = 0;
		ska::Rectangle hitbox{ pc.x + hc.xOffset, pc.y + hc.yOffset, static_cast<int>(hc.width), static_cast<int>(hc.height) };

		const auto targetBlock = m_worldState.getWorld().placeOnNearestPracticableBlock(hitbox, 1);
		pc.x = targetBlock.x - hc.xOffset;
		pc.y = targetBlock.y - hc.yOffset;
		m_entityManager.addComponent<ska::PositionComponent>(m_pokemonId, pc);

		BattleEvent be(BATTLE_START, **m_cameraSystem, m_pokemonId, m_opponentId, m_entityManager);
		m_eventDispatcher.ska::Observable<BattleEvent>::notifyObservers(be);

		return false;
	});
}

std::unique_ptr<ska::Task> StateFightLoading::unload() {
	return std::make_unique<ska::Task>([&](ska::Task&) {
		*m_ic = &m_entityManager.getComponent<ska::InputComponent>(m_pokemonId);
		m_entityManager.removeComponent<ska::InputComponent>(m_pokemonId);

		BattleEvent be(BATTLE_END, **m_cameraSystem, m_pokemonId, m_opponentId, m_entityManager);
		m_eventDispatcher.ska::Observable<BattleEvent>::notifyObservers(be);

		m_entityManager.removeComponent<BattleComponent>(m_pokemonId);
		m_entityManager.removeComponent<BattleComponent>(m_opponentId);

		if (m_entityManager.hasComponent<ska::DirectionalAnimationComponent>(m_trainerId)) {
			auto& dac = m_entityManager.getComponent<ska::DirectionalAnimationComponent>(m_trainerId);
			dac.type = ska::DirectionalAnimationType::MOVEMENT;
			dac.looked = 0;
		}
		return false;
	});
}

