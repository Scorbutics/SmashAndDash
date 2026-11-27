#include "Game/__internalConfig/LoggerConfig.h"
#include "TrainerItemSystem.h"
#include <algorithm>
#include "Base/IO/Readers/IniReader.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"
#include "Core/Inputs/InputContextManager.h"
#include "../BattleEffects.h"
#include "../BattleItems.h"
#include "../BattleRules.h"
#include "../../Data/Bag.h"
#include "../../Data/EntityLoadEvent.h"
#include "../../Data/Team.h"
#include "../../PokeballComponent.h"
#include "../../System/PokeballSystem.h"
#include "../../World/WorldState.h"

TrainerItemSystem::TrainerItemSystem(ska::EntityManager& em, PokemonGameEventDispatcher& ged, WorldState& worldState, BattleStage& stage) :
	System(em),
	SubObserver<ska::InputKeyEvent>(std::bind(&TrainerItemSystem::onKeyInputEvent, this, std::placeholders::_1), ged),
	m_entities(em),
	m_ged(ged),
	m_worldState(worldState),
	m_stage(stage) {
	updateSlot();
}

ska::Point<float> TrainerItemSystem::center(ska::EntityId entity) const {
	return ska::PositionComponent::getCenterPosition(m_entities.getComponent<ska::PositionComponent>(entity), m_entities.getComponent<ska::HitboxComponent>(entity));
}

bool TrainerItemSystem::standing(ska::EntityId entity) const {
	const auto& processed = getEntities();
	if (processed.find(entity) == processed.end()) {
		return false;
	}
	const auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
	return !fighter.knockedOut && !fighter.inBall && m_componentAccessor.get<BattleComponent>(entity).hp > 0;
}

void TrainerItemSystem::updateSlot() {
	auto& bag = m_worldState.getBag();
	const auto usable = BattleItems(bag);
	if (std::find(usable.begin(), usable.end(), m_item) == usable.end()) {
		// The item chosen ran out : the first one usable (in the order of the bag)
		m_item = usable.empty() ? -1 : usable.front();
	}
	auto& slot = m_stage.itemSlot;
	slot.item = m_item;
	slot.amount = m_item >= 0 ? bag.count(m_item) : 0;
	const auto data = m_item >= 0 ? Bag::data(m_item) : std::optional<ItemData>{};
	slot.name = data.has_value() ? data->name : "";
	slot.cooldown = m_ball.has_value() ? 1.F : static_cast<float>(m_cooldown) / static_cast<float>(Cooldown);
}
void TrainerItemSystem::next() {
	const auto usable = BattleItems(m_worldState.getBag());
	if (usable.empty()) {
		return;
	}
	const auto current = std::find(usable.begin(), usable.end(), m_item);
	m_item = current == usable.end() || current + 1 == usable.end() ? usable.front() : *(current + 1);
	updateSlot();
}
bool TrainerItemSystem::onKeyInputEvent(ska::InputKeyEvent& event) {
	const auto& actions = event.icm.getActions();
	if (m_stage.phase() != BattlePhase::Fight || m_stage.turn.active() || m_stage.cinematic.active) {
		return true;
	}
	// The Pokemon of the player played by the AI (SAD_BATTLE_AUTOPLAY) : the keys do nothing
	const auto& processed = getEntities();
	if (processed.find(m_stage.player) == processed.end() || m_componentAccessor.get<FighterComponent>(m_stage.player).controlledByAI) {
		return true;
	}
	if (actions[ska::NextItem]) {
		next();
	}
	if (actions[ska::UseItem]) {
		use();
	}
	return true;
}

void TrainerItemSystem::use() {
	if (m_item < 0 || m_cooldown > 0 || m_ball.has_value()) {
		return;
	}
	const auto data = Bag::data(m_item);
	if (!data.has_value() || m_worldState.getBag().count(m_item) <= 0) {
		return;
	}
	const auto used = data->capture ? throwBall(m_item) : heal(m_item, data->heal);
	if (used) {
		m_cooldown = Cooldown;
		m_stage.cue(BattleCue{ data->capture ? "ball" : "potion", data->name });
		SLOG(ska::LogLevel::Info) << "Battle : the trainer uses the item " << m_item << " (" << data->name << ")";
	}
	updateSlot();
}

bool TrainerItemSystem::heal(int item, int amount) {
	return standing(m_stage.player) && HealFighter(m_entities, m_stage, m_worldState.getBag(), item, amount, m_stage.player) > 0;
}
bool TrainerItemSystem::throwBall(int item) {
	// At the opponent of the Pokemon of the player (else the first wild Pokemon standing)
	auto target = ska::EntityId{ 0 };
	const auto& processed = getEntities();
	if (processed.find(m_stage.player) != processed.end() && standing(m_componentAccessor.get<FighterComponent>(m_stage.player).opponent)) {
		target = m_componentAccessor.get<FighterComponent>(m_stage.player).opponent;
	}
	for (const auto entity : m_stage.fighters) {
		if (target == 0 && standing(entity) && m_componentAccessor.get<FighterComponent>(entity).team != 0) {
			target = entity;
		}
	}
	if (target == 0 || m_componentAccessor.get<FighterComponent>(target).team == 0) {
		return false;
	}
	if (Team{ m_worldState.getSaveGame().getPathName() }.size() >= Team::MaxSize) {
		MarkText(m_entities, m_stage, m_stage.player, "ÉQUIPE PLEINE", 2);
		return false;
	}
	const auto trainer = m_stage.trainer;
	if (!trainer.has_value() || !m_entities.hasComponent<ska::PositionComponent>(*trainer) || !m_entities.hasComponent<ska::HitboxComponent>(*trainer)) {
		return false;
	}
	m_worldState.getBag().remove(item, 1);

	// The dice : the battle rolls them, the rules give the chance. Caught : 3 shakes ; else fewer the further it was
	const auto dice = RollCapture(m_entities, m_worldState, target);
	m_caught = dice.caught;
	const auto from = center(*trainer);
	const auto to = center(target);
	const auto ball = PokeballSystem::Throw(m_entities, from, to, false);
	auto& component = m_entities.getComponent<PokeballComponent>(ball);
	component.capture = true;
	component.caught = m_caught;
	component.shakes = dice.shakes;
	m_ball = ball;
	m_target = target;
	// The trainer faces where it throws (0 : down, 1 : right, 2 : up, 3 : left)
	if (m_entities.hasComponent<ska::AnimationComponent>(*trainer)) {
		const auto dx = to.x - from.x;
		const auto dy = to.y - from.y;
		m_entities.getComponent<ska::AnimationComponent>(*trainer).state = std::abs(dx) > std::abs(dy) ? (dx > 0.F ? 1 : 3) : (dy > 0.F ? 0 : 2);
	}
	m_stage.sound(BattleSound::Cast);
	return true;
}

void TrainerItemSystem::release(ska::EntityId target) {
	if (!m_entities.hasComponent<FighterComponent>(target)) {
		return;
	}
	auto& fighter = m_entities.getComponent<FighterComponent>(target);
	if (!fighter.inBall) {
		return;
	}
	fighter.inBall = false;
	fighter.recall = -1;
	fighter.appear = 0;
	MarkText(m_entities, m_stage, target, "LIBÉRÉ !", 1);
	SLOG(ska::LogLevel::Info) << "Battle : the wild pokemon breaks free";
}

void TrainerItemSystem::capture(ska::EntityId target) {
	CaptureFighter(m_entities, m_ged, m_worldState, m_stage, target);
}
void TrainerItemSystem::updateBall() {
	if (!m_ball.has_value()) {
		return;
	}
	const auto& processed = getEntities();
	const auto targetHere = processed.find(m_target) != processed.end();
	if (!m_entities.hasComponent<PokeballComponent>(*m_ball)) {
		// The ball is gone (its course is over)
		if (targetHere && !m_caught) {
			release(m_target);
		}
		m_ball.reset();
		return;
	}
	auto& ball = m_entities.getComponent<PokeballComponent>(*m_ball);
	if (!targetHere) {
		ball.capture = false;
		return;
	}
	auto& fighter = m_componentAccessor.get<FighterComponent>(m_target);
	switch (ball.state) {
	case PokeballComponent::State::Flying:
		// It follows the wild Pokemon (it moves during the flight) ; K.O. meanwhile : the ball only opens
		if (standing(m_target)) {
			ball.to = center(m_target);
		} else {
			ball.capture = false;
		}
		break;
	case PokeballComponent::State::Bouncing:
	case PokeballComponent::State::Shaking:
		// It goes in the ball : it shrinks into the light (FighterSystem), it does not fight anymore
		if (ball.capture && !fighter.inBall && !fighter.knockedOut) {
			fighter.inBall = true;
			fighter.recall = 0;
			fighter.windUp = 0;
			fighter.lunge = 0;
			fighter.recovery = 0;
			fighter.pendingSkill = -1;
			fighter.dash = 0;
		}
		break;
	case PokeballComponent::State::Caught:
		if (fighter.inBall && !fighter.captured) {
			capture(m_target);
		}
		break;
	case PokeballComponent::State::Open:
	case PokeballComponent::State::Gone:
		if (!m_caught) {
			release(m_target);
		}
		break;
	}
}

void TrainerItemSystem::refresh(unsigned int) {
	const auto delta = m_stage.delta();
	m_cooldown = m_cooldown > delta ? m_cooldown - delta : 0;
	updateBall();
	// The battle ends during a capture (the Pokemon of the player K.O., or it fled) : the wild Pokemon comes out
	if (m_stage.phase() != BattlePhase::Fight && m_stage.phase() != BattlePhase::KnockOut && m_ball.has_value() && !m_caught) {
		const auto& processed = getEntities();
		if (processed.find(m_target) != processed.end()) {
			release(m_target);
		}
		if (m_entities.hasComponent<PokeballComponent>(*m_ball)) {
			m_entities.getComponent<PokeballComponent>(*m_ball).capture = false;
		}
		m_ball.reset();
	}
	updateSlot();
}
