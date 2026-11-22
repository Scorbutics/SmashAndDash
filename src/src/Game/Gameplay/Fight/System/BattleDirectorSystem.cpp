#include "Game/__internalConfig/LoggerConfig.h"
#include "BattleDirectorSystem.h"
#include <algorithm>
#include <array>
#include "Base/IO/Readers/IniReader.h"
#include "Core/ECS/EntityLocator.h"
#include "Graphic/System/CameraSystem.h"
#include "Graphic/System/CameraFixedStrategy.h"
#include "Graphic/System/CameraFollowStrategy.h"
#include "../BattleRules.h"
#include "../../Data/MapEvent.h"
#include "../../../Script/ScriptConstants.h"

BattleDirectorSystem::BattleDirectorSystem(ska::EntityManager& em, WorldState& ws, PokemonGameEventDispatcher& ged, BattleStage& stage, int teamIndex) :
	System(em),
	m_entities(em),
	m_worldState(ws),
	m_ged(ged),
	m_stage(stage),
	m_teamIndex(teamIndex) {
	m_stage.setCamera(&m_worldState.getCamera());
}

bool BattleDirectorSystem::present(ska::EntityId fighter) const {
	const auto& processed = getEntities();
	return processed.find(fighter) != processed.end();
}

ska::Point<float> BattleDirectorSystem::center(ska::EntityId fighter) {
	return ska::PositionComponent::getCenterPosition(m_componentAccessor.get<ska::PositionComponent>(fighter), m_componentAccessor.get<ska::HitboxComponent>(fighter));
}

void BattleDirectorSystem::start() {
	// The arena : around the fighters, in the map
	const auto blockSize = static_cast<int>(m_worldState.getWorld().getBlockSize());
	auto a = center(m_stage.player);
	auto b = a;
	for (const auto entity : m_stage.fighters) {
		const auto position = center(entity);
		a = ska::Point<float>{ std::min(a.x, position.x), std::min(a.y, position.y) };
		b = ska::Point<float>{ std::max(b.x, position.x), std::max(b.y, position.y) };
	}
	const auto width = ArenaWidth * blockSize;
	const auto height = ArenaHeight * blockSize;
	const auto worldWidth = static_cast<int>(m_worldState.getWorld().getPixelWidth());
	const auto worldHeight = static_cast<int>(m_worldState.getWorld().getPixelHeight());
	auto x = static_cast<int>((a.x + b.x) / 2.F) - width / 2;
	auto y = static_cast<int>((a.y + b.y) / 2.F) - height / 2;
	x = std::max(0, std::min(x, worldWidth - width));
	y = std::max(0, std::min(y, worldHeight - height));
	m_stage.arena = ska::Rectangle{ x, y, width, height };

	// The camera of the battle : BattleCameraSystem

	for (const auto entity : getEntities()) {
		m_componentAccessor.get<FighterComponent>(entity).shownHp = static_cast<float>(m_componentAccessor.get<BattleComponent>(entity).hp);
	}
	m_stage.changePhase(BattlePhase::Fight);
	m_stage.announce("COMBAT !");
	SLOG(ska::LogLevel::Info) << "Battle : fight in the arena " << x << ", " << y << " (" << width << " x " << height << ")";
}

void BattleDirectorSystem::updateBars(unsigned int ellapsedTime) {
	for (const auto entity : getEntities()) {
		auto* fighter = &m_componentAccessor.get<FighterComponent>(entity);
		const auto& bc = m_componentAccessor.get<BattleComponent>(entity);
		const auto hp = static_cast<float>(std::max(0, bc.hp));
		fighter->barShake = fighter->barShake > ellapsedTime ? fighter->barShake - ellapsedTime : 0;
		if (fighter->shownHp <= hp) {
			fighter->shownHp = hp;
			continue;
		}
		// The lost part stays a moment, then drains
		if (fighter->hpDelay > ellapsedTime) {
			fighter->hpDelay -= ellapsedTime;
			continue;
		}
		fighter->hpDelay = 0;
		const auto drain = std::max(1.F, static_cast<float>(bc.maxHp) * 0.9F) * static_cast<float>(ellapsedTime) / 1000.F;
		fighter->shownHp = std::max(hp, fighter->shownHp - drain);
	}
}

void BattleDirectorSystem::refresh(unsigned int ellapsedTime) {
	m_stage.update(ellapsedTime);
	updateBars(ellapsedTime);

	switch (m_stage.phase()) {
	case BattlePhase::Intro:
		if (std::all_of(m_stage.fighters.begin(), m_stage.fighters.end(), [this](ska::EntityId entity) { return present(entity); })) {
			start();
		}
		break;

	case BattlePhase::Fight:
		{
			// A fighter without hit points is K.O. : it leaves the fight. A side without fighter standing lost
			std::array<int, 2> standing{ 0, 0 };
			for (const auto entity : getEntities()) {
				auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
				if (!fighter.knockedOut && m_componentAccessor.get<BattleComponent>(entity).hp <= 0) {
					fighter.knockedOut = true;
					fighter.knockedOutTime = 0;
					m_stage.hitStop(180);
					m_stage.shake(260, 7);
					m_stage.sound(BattleSound::KnockOut);
					SLOG(ska::LogLevel::Info) << "Battle : " << (fighter.player ? "the pokemon of the player" : "a wild pokemon") << " is K.O.";
				}
				if (!fighter.knockedOut && fighter.team >= 0 && fighter.team < 2) {
					standing[static_cast<std::size_t>(fighter.team)]++;
				}
			}
			if (standing[0] == 0 || standing[1] == 0) {
				m_stage.winnerTeam = standing[0] == 0 ? 1 : 0;
				m_stage.changePhase(BattlePhase::KnockOut);
				m_stage.hitStop(250);
				m_stage.slowMotion(0.25F, 1400);
				m_stage.shake(400, 10);
				m_stage.announce("K.O. !");
				SLOG(ska::LogLevel::Info) << "Battle : the side " << m_stage.winnerTeam << " won";
			}
		}
		if (m_stage.phase() == BattlePhase::Fight && m_stage.canFlee && present(m_stage.player) && m_componentAccessor.get<FighterComponent>(m_stage.player).outside >= BattleStage::FleeDelay) {
			m_stage.changePhase(BattlePhase::Fled);
			m_stage.announce("FUITE !");
			SLOG(ska::LogLevel::Info) << "Battle : the player flees";
		}
		break;

	case BattlePhase::KnockOut:
		if (m_stage.phaseTime() >= KnockOutDuration) {
			end();
		}
		break;

	case BattlePhase::Fled:
		if (m_stage.phaseTime() >= FleeDuration) {
			end();
		}
		break;

	default:
		break;
	}
}

void BattleDirectorSystem::end() {
	const auto fled = m_stage.phase() == BattlePhase::Fled;
	const auto won = !fled && m_stage.winnerTeam == 0;
	m_stage.changePhase(BattlePhase::Over);
	SLOG(ska::LogLevel::Info) << "Battle " << (fled ? "fled" : (won ? "won" : "lost"));

	// Result : hit points left of the Pokemon of the player, experience won for each wild Pokemon K.O. (battle rules)
	auto result = BattleResult{};
	result.won = won;
	result.fled = fled;
	result.teamIndex = m_teamIndex;
	const auto& pokemon = m_componentAccessor.get<BattleComponent>(m_stage.player);
	result.hpLeft = std::max(0, pokemon.hp);
	for (const auto entity : m_stage.fighters) {
		if (!present(entity) || m_componentAccessor.get<FighterComponent>(entity).team != 1 || !m_componentAccessor.get<FighterComponent>(entity).knockedOut) {
			continue;
		}
		const auto& opponent = m_componentAccessor.get<BattleComponent>(entity);
		const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(opponent.monster) + ".ini" };
		const auto dropped = data.exists("Experience level_1_exp_dropped") ? data.get<int>("Experience level_1_exp_dropped") : 0;
		result.experience += m_worldState.getBattleRules().experience(dropped, opponent.level);
	}
	// The wild Pokemon of the map is defeated (the others of its group only exist during the battle : StateFight)
	if (won && m_stage.fighters.size() > 1) {
		scheduleDeferredRemove(m_stage.fighters[1]);
	}
	m_worldState.battleEnded(result);

	// Back to the map, where the player is, when the screen is black (the map changes : WGameCore)
	MapEvent me(MapEventType::Switch_Map);
	me.mapName = m_worldState.getMapName();
	const auto mapReader = ska::IniReader{ "./Levels/" + me.mapName + "/" + me.mapName + ".ini" };
	me.chipsetName = mapReader.get<std::string>("Chipset file");
	const auto* trainer = m_worldState.getEntityLocator().getEntityId(SCRIPT_ENTITY_TRAINER);
	const auto& pc = m_entities.getComponent<ska::PositionComponent>(trainer != nullptr && m_entities.hasComponent<ska::PositionComponent>(*trainer) ? *trainer : m_stage.player);
	me.position = pc;
	auto back = [this, event = me]() {
		// The camera follows the player again
		m_worldState.getCamera().changeStrategy(std::make_unique<ska::CameraFollowStrategy>(m_entities));
		m_worldState.getCamera().setZoom(WorldState::MapZoom);
		auto mapEvent = event;
		m_ged.ska::Observable<MapEvent>::notifyObservers(mapEvent);
	};
	if (!m_worldState.getFade().start(back)) {
		back();
	}
}
