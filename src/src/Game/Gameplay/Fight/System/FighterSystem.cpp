#include "Game/__internalConfig/LoggerConfig.h"
#include "FighterSystem.h"
#include <algorithm>
#include <cmath>
#include "Core/Utils/RectangleUtils.h"
#include "Physic/Space.h"
#include "Physic/Body.h"
#include "Physic/Vect.h"
#include "../../World/WorldState.h"

namespace {
	ska::Point<float> Normalized(ska::Point<float> vector) {
		const auto length = std::sqrt(vector.x * vector.x + vector.y * vector.y);
		return length < 0.001F ? ska::Point<float>{} : ska::Point<float>{ vector.x / length, vector.y / length };
	}

	unsigned int Decrease(unsigned int value, unsigned int amount) {
		return value > amount ? value - amount : 0;
	}

	// Speed of an AI fighter (force : 10 pixels per second each), a little faster when it is fast
	float AISpeed(const BattleComponent& bc) {
		return std::min(30.F, 18.F + static_cast<float>(bc.speed) / 4.F);
	}
}

FighterSystem::FighterSystem(ska::EntityManager& em, WorldState& ws, BattleStage& stage) :
	System(em),
	m_entities(em),
	m_worldState(ws),
	m_stage(stage) {
}

ska::Point<float> FighterSystem::center(ska::EntityId entity) {
	return ska::PositionComponent::getCenterPosition(m_componentAccessor.get<ska::PositionComponent>(entity), m_componentAccessor.get<ska::HitboxComponent>(entity));
}

void FighterSystem::stop(FighterState& fighter) {
	// The physics bodies stop right away (a hit stop freezes the fighters)
	auto& force = m_componentAccessor.get<ska::ForceComponent>(fighter.entity);
	force.x = 0.F;
	force.y = 0.F;
	if (m_entities.hasComponent<ska::cp::HitboxComponent>(fighter.entity)) {
		const auto& body = m_entities.getComponent<ska::cp::HitboxComponent>(fighter.entity);
		auto& space = m_worldState.getSpace();
		space.getBody(body.bodyIndex).setVelocity(ska::cp::Vect{ 0., 0. });
		space.getBody(body.controlBodyIndex).setVelocity(ska::cp::Vect{ 0., 0. });
	}
}

void FighterSystem::control(FighterState& fighter, unsigned int ellapsedTime) {
	const auto delta = m_stage.delta();
	fighter.stun = Decrease(fighter.stun, delta);
	fighter.release = Decrease(fighter.release, delta);
	fighter.flash = Decrease(fighter.flash, ellapsedTime);

	const auto fighting = m_stage.phase() == BattlePhase::Fight;
	const auto canMove = fighting && !m_stage.frozen() && fighter.stun == 0 && fighter.windUp == 0;

	// The keys move the Pokemon of the player only when it can move
	auto* input = m_componentPossibleAccessor.get<ska::InputComponent>(fighter.entity);
	if (fighter.player && input != nullptr) {
		if (m_playerPower < 0.F) {
			m_playerPower = input->movePower;
		}
		input->movePower = canMove && !fighter.controlledByAI ? m_playerPower : 0.F;
	}

	if (m_stage.frozen()) {
		stop(fighter);
		return;
	}

	auto& force = m_componentAccessor.get<ska::ForceComponent>(fighter.entity);
	const auto position = center(fighter.entity);
	const auto& arena = m_stage.arena;
	const auto inside = position.x >= arena.x && position.x <= arena.x + arena.w && position.y >= arena.y && position.y <= arena.y + arena.h;

	// Leaving the arena : the player flees a wild Pokemon, else it is brought back
	if (fighter.player && !fighter.controlledByAI && m_stage.canFlee) {
		fighter.outside = inside || !fighting ? 0 : fighter.outside + ellapsedTime;
	}

	auto move = ska::Point<float>{};
	if (canMove && fighter.controlledByAI) {
		const auto toOpponent = Normalized(center(fighter.opponent) - position);
		switch (fighter.intent) {
		case FighterIntent::Approach: move = toOpponent; break;
		case FighterIntent::Retreat: move = ska::Point<float>{ -toOpponent.x, -toOpponent.y }; break;
		// Around the opponent, slowly closer
		case FighterIntent::SidestepLeft: move = ska::Point<float>{ -toOpponent.y + toOpponent.x * 0.2F, toOpponent.x + toOpponent.y * 0.2F }; break;
		case FighterIntent::SidestepRight: move = ska::Point<float>{ toOpponent.y + toOpponent.x * 0.2F, -toOpponent.x + toOpponent.y * 0.2F }; break;
		default: break;
		}
	}

	// The AI (and a player that cannot flee) stays in the arena : near an edge, it goes back inside
	if (fighting && (fighter.controlledByAI || !m_stage.canFlee)) {
		const auto margin = 36.F;
		auto inward = ska::Point<float>{};
		if (position.x < arena.x + margin) inward.x = 1.F;
		if (position.x > arena.x + arena.w - margin) inward.x = -1.F;
		if (position.y < arena.y + margin) inward.y = 1.F;
		if (position.y > arena.y + arena.h - margin) inward.y = -1.F;
		if (inward.x != 0.F || inward.y != 0.F) {
			if (fighter.controlledByAI) {
				move = Normalized(ska::Point<float>{ move.x + inward.x * 1.5F, move.y + inward.y * 1.5F });
			} else if (!inside) {
				force.x += inward.x * m_playerPower;
				force.y += inward.y * m_playerPower;
			}
		}
	}

	if (move.x != 0.F || move.y != 0.F) {
		const auto speed = AISpeed(m_componentAccessor.get<BattleComponent>(fighter.entity));
		const auto direction = Normalized(move);
		// The battle time can be slower (slow motion)
		const auto scale = ellapsedTime == 0 ? 0.F : static_cast<float>(delta) / static_cast<float>(ellapsedTime);
		force.x += direction.x * speed * scale;
		force.y += direction.y * speed * scale;
	}

	// Pushed back after a hit : the push slows down quickly
	if (std::abs(fighter.knockback.x) > 1.F || std::abs(fighter.knockback.y) > 1.F) {
		force.x += fighter.knockback.x / 10.F;
		force.y += fighter.knockback.y / 10.F;
		const auto decay = std::pow(0.82F, static_cast<float>(delta) / 20.F);
		fighter.knockback.x *= decay;
		fighter.knockback.y *= decay;
	} else {
		fighter.knockback = {};
	}

	// It faces its opponent when it does not walk (and when it uses a skill)
	const auto* movement = m_componentPossibleAccessor.get<ska::MovementComponent>(fighter.entity);
	const auto walking = movement != nullptr && (std::abs(movement->vx) > 0.5F || std::abs(movement->vy) > 0.5F);
	if ((!walking || fighter.windUp > 0) && fighting) {
		const auto toOpponent = center(fighter.opponent) - position;
		auto& animation = m_componentAccessor.get<ska::AnimationComponent>(fighter.entity);
		animation.state = ska::RectangleUtils::getDirectionFromPos(ska::Point<int>{}, ska::Point<int>{ static_cast<int>(toOpponent.x), static_cast<int>(toOpponent.y) });
	}
}

void FighterSystem::draw(FighterState& fighter) {
	auto& effect = m_componentAccessor.get<ska::GraphicComponent>(fighter.entity).effect;
	effect = ska::RenderEffect{};

	// K.O. : a flash, then it sinks and disappears
	if (fighter.entity == m_stage.loser && (m_stage.phase() == BattlePhase::KnockOut || m_stage.phase() == BattlePhase::Over)) {
		const auto time = m_stage.phaseTime();
		if (time < 300) {
			effect.flash = static_cast<unsigned char>((time / 60) % 2 == 0 ? 255 : 120);
			return;
		}
		const auto progress = std::min(1.F, static_cast<float>(time - 300) / 1300.F);
		effect.alpha = static_cast<unsigned char>(255.F * (1.F - progress));
		effect.scaleX = 1.F + 0.25F * progress;
		effect.scaleY = 1.F - 0.7F * progress;
		effect.red = 255;
		effect.green = static_cast<unsigned char>(255.F * (1.F - 0.5F * progress));
		effect.blue = static_cast<unsigned char>(255.F * (1.F - 0.5F * progress));
		return;
	}

	if (fighter.flash > 0) {
		effect.flash = static_cast<unsigned char>(255 * fighter.flash / FlashDuration);
	} else if (fighter.stun > 0 && (fighter.stun / 70) % 2 == 0) {
		// Stunned : it blinks red
		effect.green = 150;
		effect.blue = 150;
	}

	if (fighter.windUp > 0) {
		// Squashed before the skill (the wind-up)
		const auto progress = 1.F - static_cast<float>(fighter.windUp) / static_cast<float>(WindUpDuration);
		effect.scaleX = 1.F + 0.2F * progress;
		effect.scaleY = 1.F - 0.16F * progress;
	} else if (fighter.release > 0) {
		// Stretched when the skill goes
		const auto progress = static_cast<float>(fighter.release) / static_cast<float>(ReleaseDuration);
		effect.scaleX = 1.F - 0.12F * progress;
		effect.scaleY = 1.F + 0.16F * progress;
	}
}

void FighterSystem::refresh(unsigned int ellapsedTime) {
	const auto& processed = getEntities();
	// The battle is over : the keys move the Pokemon again (its input goes back to the trainer)
	if (m_stage.phase() == BattlePhase::Over && m_playerPower >= 0.F) {
		auto* input = m_componentPossibleAccessor.get<ska::InputComponent>(m_stage.player.entity);
		if (input != nullptr) {
			input->movePower = m_playerPower;
		}
	}
	for (auto* fighter : { &m_stage.player, &m_stage.opponent }) {
		if (processed.find(fighter->entity) == processed.end() || processed.find(fighter->opponent) == processed.end()) {
			continue;
		}
		if (m_stage.phase() != BattlePhase::Intro) {
			control(*fighter, ellapsedTime);
		}
		draw(*fighter);
	}
}
