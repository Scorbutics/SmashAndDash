#include "Game/__internalConfig/LoggerConfig.h"
#include "BattleSystem.h"
#include "../SkillComponent.h"
#include "Base/IO/Readers/IniReader.h"
#include "Core/Inputs/InputContextManager.h"
#include "../../CustomEntityManager.h"
#include "Core/Utils/PhysicUtils.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "Graphic/GraphicComponent.h"
#include "../BattleRules.h"
#include "Core/ECS/Basics/Input/InputComponent.h"
#include "Core/ECS/Basics/Physic/ForceComponent.h"
#include <cmath>
#include <cstdlib>

BattleSystem::BattleSystem(CustomEntityManager& em, PokemonGameEventDispatcher& ged, BattleRules& rules, const ska::EntityId pokemon, const ska::EntityId opponent, unsigned int blockSize) :
	System(em),
	SubObserver<ska::InputKeyEvent>(std::bind(&BattleSystem::onKeyInputEvent, this, std::placeholders::_1), ged),
	m_customEM(em),
	m_rules(rules),
	m_pokemon(pokemon),
	m_opponent(opponent),
	m_blockSize(blockSize == 0 ? 1 : blockSize),
	m_autoplay(std::getenv("SAD_BATTLE_AUTOPLAY") != nullptr) {
}

bool BattleSystem::fighting(ska::EntityId fighter) const {
	const auto& processed = getEntities();
	return processed.find(fighter) != processed.end() && m_componentAccessor.get<BattleComponent>(fighter).hp > 0;
}

std::array<unsigned long, 4>& BattleSystem::lastUses(ska::EntityId fighter) {
	return fighter == m_pokemon ? m_pokemonUses : m_opponentUses;
}

const std::array<unsigned long, 4>& BattleSystem::lastUses(ska::EntityId fighter) const {
	return fighter == m_pokemon ? m_pokemonUses : m_opponentUses;
}

bool BattleSystem::skillReady(unsigned int index, ska::EntityId from) const {
	if ((from != m_pokemon && from != m_opponent) || !fighting(from) || index >= 4) {
		return false;
	}
	const auto& skill = m_componentAccessor.get<SkillsHolderComponent>(from).skills[index];
	const auto lastUse = lastUses(from)[index];
	return skill.particleNumber > 0 && (lastUse == 0 || m_time - lastUse >= skill.cooldown);
}

bool BattleSystem::useSkill(unsigned int index, ska::EntityId from) {
	if (!skillReady(index, from)) {
		return false;
	}
	// 0 means "never used"
	lastUses(from)[index] = m_time == 0 ? 1 : m_time;
	const auto& position = m_componentAccessor.get<ska::PositionComponent>(from);
	SLOG(ska::LogLevel::Debug) << "Battle t=" << m_time << " : " << (from == m_pokemon ? "the pokemon of the player" : "the wild pokemon") << " uses its skill " << index
		<< " (" << m_componentAccessor.get<SkillsHolderComponent>(from).skills[index].name << ") from " << position.x << ", " << position.y;
	createSkill(index, from);
	return true;
}

bool BattleSystem::onKeyInputEvent(ska::InputKeyEvent& event) {
	const auto& actions = event.icm.getActions();
	const ska::InputAction skillActions[] = { ska::InputAction::ShotSkill1, ska::InputAction::ShotSkill2, ska::InputAction::ShotSkill3, ska::InputAction::ShotSkill4 };
	for (auto index = 0u; index < 4; index++) {
		if (actions[skillActions[index]]) {
			useSkill(index, m_pokemon);
		}
	}
	return true;
}

void BattleSystem::decide() {
	decide(m_opponent, m_pokemon);
	if (m_autoplay) {
		decide(m_pokemon, m_opponent);
	}
}

void BattleSystem::decide(ska::EntityId fighter, ska::EntityId target) {
	if (!fighting(fighter) || !fighting(target)) {
		return;
	}
	const auto& fighterBc = m_componentAccessor.get<BattleComponent>(fighter);
	const auto& fighterPosition = m_componentAccessor.get<ska::PositionComponent>(fighter);
	const auto& targetPosition = m_componentAccessor.get<ska::PositionComponent>(target);
	const auto dx = static_cast<double>(fighterPosition.x - targetPosition.x);
	const auto dy = static_cast<double>(fighterPosition.y - targetPosition.y);
	const auto pixels = std::sqrt(dx * dx + dy * dy);
	const auto distance = static_cast<int>(pixels / m_blockSize);

	// The skills it can use now : ready, and reaching its target
	const auto& skills = m_componentAccessor.get<SkillsHolderComponent>(fighter).skills;
	auto ready = std::vector<bool>{};
	for (auto index = 0u; index < 4; index++) {
		ready.push_back(skillReady(index, fighter) && pixels <= static_cast<double>(skills[index].range));
	}
	const auto decision = m_rules.decide(fighterBc.monster, distance, fighterBc.hp, fighterBc.maxHp, ready);
	SLOG(ska::LogLevel::Debug) << "Battle t=" << m_time << " : " << (fighter == m_pokemon ? "player" : "wild") << " at " << fighterPosition.x << ", " << fighterPosition.y
		<< " distance " << static_cast<int>(pixels) << " px, decision " << decision;
	if (decision >= 0) {
		useSkill(static_cast<unsigned int>(decision), fighter);
	}
}

void BattleSystem::refresh(unsigned int ellapsedTime) {
	m_time += ellapsedTime;
	if (m_time >= m_nextDecision) {
		m_nextDecision = m_time + DecisionDelay;
		decide();
	}
}

void BattleSystem::createSkill(const unsigned int index, ska::EntityId from) {
	/* Ignore potential external entity */
	if (from != m_pokemon && from != m_opponent) {
		return;
	}
	//const ska::InputRangeContainer& irc = from == m_pokemon ? m_playerICM.getRanges() : m_opponentICM.getRanges();
	//const ska::InputRange& mousePos = irc[ska::InputRangeType::MousePos];
	auto& pc = m_componentAccessor.get<ska::PositionComponent>(from);
	auto& hc = m_componentAccessor.get<ska::HitboxComponent>(from);
	auto& dac = m_componentAccessor.get<ska::AnimationComponent>(from);
	auto& shc = m_componentAccessor.get<SkillsHolderComponent>(from);

	if (index >= shc.skills.size()) {
		return;
	}

	auto skillStartPos = ska::PositionComponent::getFrontPosition(pc, hc, dac);


	auto n = shc.skills[index].particleNumber;

	for (auto i = 0; i < n; i++) {
		// Each particle is the skill used (not the skill at the index of the particle)
		auto skill = m_customEM.createSkill(shc, index);

		const auto& scPtr = m_componentPossibleAccessor.get<SkillComponent>(skill);
		if(scPtr == nullptr) {
			continue;
		}

		auto& gcSkill = m_componentAccessor.get<ska::GraphicComponent>(skill);
		auto& sc = *scPtr;
		sc.origin = skillStartPos - ska::Point<int>(gcSkill.animatedSprites[0].getWidth() / 2, gcSkill.animatedSprites[0].getHeight() / 2);
		m_componentAccessor.add<ska::PositionComponent>(skill, sc.origin);

		const auto target = from == m_pokemon ? m_opponent : m_pokemon;
		const auto& pcTarget = m_componentAccessor.get<ska::PositionComponent>(target);
		const auto& hcTarget = m_componentAccessor.get<ska::HitboxComponent>(target);
		const auto pTarget = ska::Point<int>(hcTarget.xOffset, hcTarget.yOffset) + ska::Point<int>(pcTarget);

		sc.target = pTarget;
		sc.battler = from;
	}
}

BattleSystem::~BattleSystem() {
}
