#include "Game/__internalConfig/LoggerConfig.h"
#include "StatisticsSystem.h"
#include "Game/Gameplay/State/StateMap.h"
#include "Base/IO/Readers/IniReader.h"
#include "../BattleRules.h"
#include "Core/ECS/EntityLocator.h"
#include "Game/Script/ScriptConstants.h"

StatisticsSystem::StatisticsSystem(ska::EntityManager& em, WorldState& ws, PokemonGameEventDispatcher& ged, ska::EntityId pokemon, ska::EntityId opponent, int teamIndex) :
	System(em),
	m_entities(em),
	m_worldScene(ws),
	m_ged(ged),
	m_pokemon(pokemon),
	m_opponent(opponent),
	m_teamIndex(teamIndex) {
}

void StatisticsSystem::refresh(unsigned int) {
	if (m_ended) {
		return;
	}
	const auto& processed = getEntities();
	for (const auto entityId : processed) {
		auto& bc = m_componentAccessor.get<BattleComponent>(entityId);
		if (bc.hp > 0 || (entityId != m_pokemon && entityId != m_opponent)) {
			continue;
		}

		// The battle ends once, back to the map
		m_ended = true;
		const auto won = entityId == m_opponent;
		SLOG(ska::LogLevel::Info) << "Battle " << (won ? "won" : "lost");
		if (won) {
			scheduleDeferredRemove(entityId);
		}
		// Result : hit points left of the Pokemon of the player, experience won (battle rules)
		auto result = BattleResult{};
		result.won = won;
		result.teamIndex = m_teamIndex;
		const auto& pokemon = m_componentAccessor.get<BattleComponent>(m_pokemon);
		result.hpLeft = pokemon.hp < 0 ? 0 : pokemon.hp;
		if (won) {
			const auto& opponent = m_componentAccessor.get<BattleComponent>(m_opponent);
			const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(opponent.monster) + ".ini" };
			const auto dropped = data.exists("Experience level_1_exp_dropped") ? data.get<int>("Experience level_1_exp_dropped") : 0;
			result.experience = m_worldScene.getBattleRules().experience(dropped, opponent.level);
		}
		m_worldScene.battleEnded(result);

		// Back to the map, where the player is
		MapEvent me(MapEventType::Switch_Map);
		me.mapName = m_worldScene.getMapName();
		const auto mapReader = ska::IniReader{ "./Levels/" + me.mapName + "/" + me.mapName + ".ini" };
		me.chipsetName = mapReader.get<std::string>("Chipset file");
		const auto* trainer = m_worldScene.getEntityLocator().getEntityId(SCRIPT_ENTITY_TRAINER);
		const auto& pc = m_entities.getComponent<ska::PositionComponent>(trainer != nullptr && m_entities.hasComponent<ska::PositionComponent>(*trainer) ? *trainer : entityId);
		me.position = pc;
		m_ged.ska::Observable<MapEvent>::notifyObservers(me);
		return;
	}
}

StatisticsSystem::~StatisticsSystem() {
}
