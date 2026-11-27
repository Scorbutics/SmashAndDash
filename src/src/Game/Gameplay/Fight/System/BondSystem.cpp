#include "Game/__internalConfig/LoggerConfig.h"
#include "BondSystem.h"
#include <cstdlib>
#include "Base/IO/Readers/IniReader.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "Core/Inputs/InputContextManager.h"
#include "../BattleEffects.h"
#include "../BattleRules.h"
#include "../SkillsHolderComponent.h"
#include "../../World/WorldState.h"
#include "BattleSystem.h"

BondSystem::BondSystem(ska::EntityManager& em, PokemonGameEventDispatcher& ged, WorldState& worldState, BattleStage& stage, BattleSystem& battle) :
	System(em),
	SubObserver<ska::InputKeyEvent>(std::bind(&BondSystem::onKeyInputEvent, this, std::placeholders::_1), ged),
	m_entities(em),
	m_worldState(worldState),
	m_stage(stage),
	m_battle(battle) {
}

bool BondSystem::standing(ska::EntityId entity) const {
	const auto& processed = getEntities();
	if (processed.find(entity) == processed.end()) {
		return false;
	}
	const auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
	return !fighter.knockedOut && !fighter.inBall && m_componentAccessor.get<BattleComponent>(entity).hp > 0;
}

int BondSystem::strongestSkill() const {
	if (!m_entities.hasComponent<SkillsHolderComponent>(m_stage.player)) {
		return -1;
	}
	const auto& skills = m_entities.getComponent<SkillsHolderComponent>(m_stage.player).skills;
	auto best = -1;
	for (auto i = 0; i < static_cast<int>(skills.size()); i++) {
		const auto& skill = skills[static_cast<std::size_t>(i)];
		if (skill.kind == SkillKind::Self || (skill.power <= 0 && skill.fixedDamage <= 0)) {
			continue;
		}
		if (best < 0 || skill.power > skills[static_cast<std::size_t>(best)].power) {
			best = i;
		}
	}
	return best;
}

bool BondSystem::ready() const {
	if (m_stage.phase() != BattlePhase::Fight || m_stage.cinematic.active || m_stage.bond < BattleStage::BondFull || !standing(m_stage.player)) {
		return false;
	}
	const auto& fighter = m_componentAccessor.get<FighterComponent>(m_stage.player);
	return !fighter.busy() && standing(fighter.opponent) && strongestSkill() >= 0;
}

bool BondSystem::onKeyInputEvent(ska::InputKeyEvent& event) {
	if (!event.icm.getActions()[ska::Special] || !standing(m_stage.player) || m_componentAccessor.get<FighterComponent>(m_stage.player).controlledByAI) {
		return true;
	}
	auto& cinematic = m_stage.cinematic;
	if (!cinematic.active) {
		m_requested = true;
	} else if (!cinematic.pressed && cinematic.time >= BondCinematic::RingStart) {
		const auto gap = static_cast<int>(cinematic.time) - static_cast<int>(BondCinematic::RingClose);
		press(std::abs(gap) <= static_cast<int>(BondCinematic::PerfectWindow));
	}
	return true;
}

void BondSystem::start() {
	m_skill = strongestSkill();
	const auto& skill = m_entities.getComponent<SkillsHolderComponent>(m_stage.player).skills[static_cast<std::size_t>(m_skill)];
	const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(m_componentAccessor.get<BattleComponent>(m_stage.player).monster) + ".ini" };
	auto& cinematic = m_stage.cinematic;
	cinematic = BondCinematic{};
	cinematic.active = true;
	cinematic.user = m_stage.player;
	cinematic.pokemon = data.exists("Description name") ? data.get<std::string>("Description name") : "?";
	cinematic.skill = skill.name;
	cinematic.line = m_worldState.getBattleRules().line("bond", cinematic.pokemon, cinematic.skill, 0, ska::NumberUtils::random(0, 999));
	if (cinematic.line.empty()) {
		cinematic.line = cinematic.pokemon + ", " + cinematic.skill + " !";
	}
	m_stage.bond = 0;
	m_stage.hitStop(BondCinematic::Release);
	m_stage.sound(BattleSound::Cast);
	SLOG(ska::LogLevel::Info) << "Battle : the bond unleashes " << cinematic.skill << " (\"" << cinematic.line << "\")";
}

void BondSystem::press(bool perfect) {
	auto& cinematic = m_stage.cinematic;
	cinematic.pressed = true;
	cinematic.pressedAt = cinematic.time;
	cinematic.perfect = perfect;
	m_stage.sound(perfect ? BattleSound::StrongHit : BattleSound::Guard);
}

void BondSystem::release() {
	auto& cinematic = m_stage.cinematic;
	cinematic.released = true;
	auto& fighter = m_componentAccessor.get<FighterComponent>(m_stage.player);
	fighter.bondPower = m_worldState.getBattleRules().bondPower(cinematic.perfect);
	fighter.bondTime = BondTime;
	if (!m_battle.useSkill(static_cast<unsigned int>(m_skill), m_stage.player, true)) {
		fighter.bondTime = 0;
		SLOG(ska::LogLevel::Info) << "Battle : the skill of the bond could not go";
		return;
	}
	if (cinematic.perfect) {
		MarkText(m_entities, m_stage, m_stage.player, "PARFAIT !", 1);
	}
	m_stage.shake(260, 6);
	SLOG(ska::LogLevel::Info) << "Battle : the bond skill goes, " << fighter.bondPower << "% damage" << (cinematic.perfect ? " (perfect)" : "");
}

void BondSystem::refresh(unsigned int ellapsedTime) {
	auto& cinematic = m_stage.cinematic;
	if (!cinematic.active) {
		const auto requested = m_requested;
		m_requested = false;
		const auto autoplay = standing(m_stage.player) && m_componentAccessor.get<FighterComponent>(m_stage.player).controlledByAI;
		if ((requested || autoplay) && ready()) {
			start();
		}
		return;
	}
	cinematic.time += ellapsedTime;
	if (m_stage.phase() != BattlePhase::Fight || !standing(cinematic.user)) {
		cinematic.active = false;
		return;
	}
	if (!cinematic.released) {
		// The battle stays stopped until the skill goes
		if (cinematic.time < BondCinematic::Release) {
			m_stage.hitStop(BondCinematic::Release - cinematic.time);
		}
		// The AI presses when the ring closes, perfect 1 time in 2
		if (!cinematic.pressed && m_componentAccessor.get<FighterComponent>(cinematic.user).controlledByAI && cinematic.time >= BondCinematic::RingClose - 40) {
			press(ska::NumberUtils::random(0, 1) == 0);
		}
		if (cinematic.time >= BondCinematic::Release && !m_stage.frozen()) {
			release();
		}
	}
	if (cinematic.time >= BondCinematic::Duration && cinematic.released) {
		cinematic.active = false;
	}
}
