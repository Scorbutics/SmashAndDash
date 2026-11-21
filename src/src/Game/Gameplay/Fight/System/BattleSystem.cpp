#include "Game/__internalConfig/LoggerConfig.h"
#include "BattleSystem.h"
#include <cmath>
#include <cstdlib>
#include "../SkillComponent.h"
#include "Core/Inputs/InputContextManager.h"
#include "../../CustomEntityManager.h"
#include "Core/Utils/RectangleUtils.h"
#include "Graphic/GraphicComponent.h"
#include "../BattleRules.h"
#include "FighterSystem.h"

BattleSystem::BattleSystem(CustomEntityManager& em, PokemonGameEventDispatcher& ged, BattleRules& rules, BattleStage& stage, unsigned int blockSize) :
	System(em),
	SubObserver<ska::InputKeyEvent>(std::bind(&BattleSystem::onKeyInputEvent, this, std::placeholders::_1), ged),
	m_customEM(em),
	m_rules(rules),
	m_stage(stage),
	m_blockSize(blockSize == 0 ? 1 : blockSize) {
}

ska::Point<float> BattleSystem::center(ska::EntityId entity) const {
	return ska::PositionComponent::getCenterPosition(m_componentAccessor.get<ska::PositionComponent>(entity), m_componentAccessor.get<ska::HitboxComponent>(entity));
}

bool BattleSystem::fighting(ska::EntityId fighter) const {
	const auto& processed = getEntities();
	return m_stage.isFighter(fighter) && processed.find(fighter) != processed.end() && m_componentAccessor.get<BattleComponent>(fighter).hp > 0;
}

bool BattleSystem::canAct(ska::EntityId fighter) const {
	if (m_stage.phase() != BattlePhase::Fight || m_stage.frozen() || !fighting(fighter)) {
		return false;
	}
	const auto& state = fighter == m_stage.player.entity ? m_stage.player : m_stage.opponent;
	return state.stun == 0 && state.windUp == 0;
}

std::array<unsigned long, 4>& BattleSystem::lastUses(ska::EntityId fighter) {
	return fighter == m_stage.player.entity ? m_pokemonUses : m_opponentUses;
}

const std::array<unsigned long, 4>& BattleSystem::lastUses(ska::EntityId fighter) const {
	return fighter == m_stage.player.entity ? m_pokemonUses : m_opponentUses;
}

bool BattleSystem::skillReady(unsigned int index, ska::EntityId from) const {
	if (!fighting(from) || index >= 4) {
		return false;
	}
	const auto& skill = m_componentAccessor.get<SkillsHolderComponent>(from).skills[index];
	const auto lastUse = lastUses(from)[index];
	return skill.particleNumber > 0 && (lastUse == 0 || m_time - lastUse >= skill.cooldown);
}

float BattleSystem::cooldown(unsigned int index, ska::EntityId from) const {
	if (!fighting(from) || index >= 4) {
		return 0.F;
	}
	const auto& skill = m_componentAccessor.get<SkillsHolderComponent>(from).skills[index];
	const auto lastUse = lastUses(from)[index];
	if (lastUse == 0 || skill.cooldown == 0 || m_time - lastUse >= skill.cooldown) {
		return 0.F;
	}
	return 1.F - static_cast<float>(m_time - lastUse) / static_cast<float>(skill.cooldown);
}

bool BattleSystem::useSkill(unsigned int index, ska::EntityId from) {
	if (!canAct(from) || !skillReady(index, from)) {
		return false;
	}
	// 0 means "never used"
	lastUses(from)[index] = m_time == 0 ? 1 : m_time;
	auto& fighter = m_stage.fighter(from);
	fighter.windUp = FighterSystem::WindUpDuration;
	fighter.pendingSkill = static_cast<int>(index);
	const auto position = center(from);
	SLOG(ska::LogLevel::Debug) << "Battle t=" << m_time << " : " << (fighter.player ? "the pokemon of the player" : "the wild pokemon") << " uses its skill " << index
		<< " (" << m_componentAccessor.get<SkillsHolderComponent>(from).skills[index].name << ") from " << position.x << ", " << position.y;
	return true;
}

bool BattleSystem::onKeyInputEvent(ska::InputKeyEvent& event) {
	if (m_stage.player.controlledByAI) {
		return true;
	}
	const auto& actions = event.icm.getActions();
	const ska::InputAction skillActions[] = { ska::InputAction::ShotSkill1, ska::InputAction::ShotSkill2, ska::InputAction::ShotSkill3, ska::InputAction::ShotSkill4 };
	for (auto index = 0u; index < 4; index++) {
		if (actions[skillActions[index]]) {
			useSkill(index, m_stage.player.entity);
		}
	}
	return true;
}

void BattleSystem::decide(FighterState& fighter) {
	if (!canAct(fighter.entity) || !fighting(fighter.opponent)) {
		return;
	}
	const auto& bc = m_componentAccessor.get<BattleComponent>(fighter.entity);
	const auto toOpponent = center(fighter.opponent) - center(fighter.entity);
	const auto pixels = std::sqrt(toOpponent.x * toOpponent.x + toOpponent.y * toOpponent.y);
	const auto distance = static_cast<int>(pixels / m_blockSize);

	// The skills it can use now : ready, and reaching its opponent. "reach" : the longest of the ready skills
	const auto& skills = m_componentAccessor.get<SkillsHolderComponent>(fighter.entity).skills;
	auto ready = std::vector<bool>{};
	auto reach = 0;
	for (auto index = 0u; index < 4; index++) {
		const auto skillReadyNow = skillReady(index, fighter.entity);
		if (skillReadyNow) {
			reach = std::max(reach, skills[index].range);
		}
		ready.push_back(skillReadyNow && pixels <= static_cast<double>(skills[index].range));
	}
	const auto decision = m_rules.decide(bc.monster, distance, bc.hp, bc.maxHp, ready);
	const auto intent = m_rules.move(bc.monster, static_cast<int>(pixels), reach, bc.hp, bc.maxHp);
	fighter.intent = static_cast<FighterIntent>(intent);
	SLOG(ska::LogLevel::Debug) << "Battle t=" << m_time << " : " << (fighter.player ? "player" : "wild") << " at " << static_cast<int>(center(fighter.entity).x) << ", "
		<< static_cast<int>(center(fighter.entity).y) << " distance " << static_cast<int>(pixels) << " px, decision " << decision << ", movement " << intent;
	if (decision >= 0) {
		useSkill(static_cast<unsigned int>(decision), fighter.entity);
	}
}

void BattleSystem::refresh(unsigned int) {
	const auto delta = m_stage.delta();
	m_time += delta;

	// End of the wind-ups : the skills go
	for (auto* fighter : { &m_stage.player, &m_stage.opponent }) {
		if (fighter->windUp == 0) {
			continue;
		}
		fighter->windUp = fighter->windUp > delta ? fighter->windUp - delta : 0;
		if (fighter->windUp == 0 && fighter->pendingSkill >= 0) {
			if (m_stage.phase() == BattlePhase::Fight && fighting(fighter->entity) && fighting(fighter->opponent)) {
				createSkill(static_cast<unsigned int>(fighter->pendingSkill), fighter->entity);
				fighter->release = FighterSystem::ReleaseDuration;
			}
			fighter->pendingSkill = -1;
		}
	}

	if (m_stage.phase() == BattlePhase::Fight) {
		auto index = 0u;
		for (auto* fighter : { &m_stage.opponent, &m_stage.player }) {
			auto& next = m_nextDecisions[index++];
			if (fighter->controlledByAI && m_time >= next) {
				next = m_time + DecisionDelay;
				decide(*fighter);
			}
		}
	}
}

void BattleSystem::createSkill(const unsigned int index, ska::EntityId from) {
	auto& shc = m_componentAccessor.get<SkillsHolderComponent>(from);
	if (index >= shc.skills.size()) {
		return;
	}
	const auto target = m_stage.fighter(from).opponent;

	// From the side of the fighter facing its target, towards the target (where it is now)
	const auto origin = center(from);
	const auto aim = center(target);
	const auto toTarget = aim - origin;
	const auto length = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
	const auto direction = length < 0.001F ? ska::Point<float>{ 1.F, 0.F } : ska::Point<float>{ toTarget.x / length, toTarget.y / length };
	const auto& hc = m_componentAccessor.get<ska::HitboxComponent>(from);
	const auto front = static_cast<float>(std::max(hc.width, hc.height)) / 2.F;
	const auto start = ska::Point<float>{ origin.x + direction.x * front, origin.y + direction.y * front };

	const auto particles = shc.skills[index].particleNumber;
	for (auto i = 0; i < particles; i++) {
		// Each particle is the skill used (not the skill at the index of the particle)
		auto skill = m_customEM.createSkill(shc, index);

		const auto& scPtr = m_componentPossibleAccessor.get<SkillComponent>(skill);
		if (scPtr == nullptr) {
			continue;
		}

		auto& gcSkill = m_componentAccessor.get<ska::GraphicComponent>(skill);
		auto& sc = *scPtr;
		const auto half = ska::Point<int>(gcSkill.animatedSprites[0].getWidth() / 2, gcSkill.animatedSprites[0].getHeight() / 2);
		sc.origin = ska::Point<int>{ static_cast<int>(start.x), static_cast<int>(start.y) } - half;
		m_componentAccessor.add<ska::PositionComponent>(skill, sc.origin);
		sc.target = ska::Point<int>{ static_cast<int>(aim.x), static_cast<int>(aim.y) } - half;
		sc.battler = from;
	}
}
