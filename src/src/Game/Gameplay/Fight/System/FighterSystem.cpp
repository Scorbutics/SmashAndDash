#include "Game/__internalConfig/LoggerConfig.h"
#include "FighterSystem.h"
#include <algorithm>
#include <cmath>
#include "Core/Inputs/InputContextManager.h"
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

	// Walk speed of a fighter (pixels per second), a little faster when it is fast
	float WalkSpeed(const BattleComponent& bc) {
		return std::min(260.F, 170.F + static_cast<float>(bc.speed) * 3.F);
	}

	float Ease(float progress) {
		return 1.F - (1.F - progress) * (1.F - progress);
	}
}

FighterSystem::FighterSystem(ska::EntityManager& em, PokemonGameEventDispatcher& ged, WorldState& ws, BattleStage& stage) :
	System(em),
	SubObserver<ska::InputKeyEvent>(std::bind(&FighterSystem::onKeyInputEvent, this, std::placeholders::_1), ged),
	m_entities(em),
	m_worldState(ws),
	m_stage(stage) {
}

bool FighterSystem::onKeyInputEvent(ska::InputKeyEvent& event) {
	m_receivedToggles |= event.icm.getToggles();
	m_received = true;
	return true;
}

ska::Point<float> FighterSystem::center(ska::EntityId entity) {
	return ska::PositionComponent::getCenterPosition(m_componentAccessor.get<ska::PositionComponent>(entity), m_componentAccessor.get<ska::HitboxComponent>(entity));
}

ska::Point<float> FighterSystem::keysDirection() const {
	auto direction = ska::Point<float>{};
	if (m_toggles[ska::MoveUp]) direction.y -= 1.F;
	if (m_toggles[ska::MoveDown]) direction.y += 1.F;
	if (m_toggles[ska::MoveLeft]) direction.x -= 1.F;
	if (m_toggles[ska::MoveRight]) direction.x += 1.F;
	return Normalized(direction);
}

void FighterSystem::setVelocity(ska::EntityId entity, ska::Point<float> velocity) {
	// The physics bodies take the velocity at once (no acceleration, no sliding) : the control body too (its force)
	auto& force = m_componentAccessor.get<ska::ForceComponent>(entity);
	force.x = velocity.x / 10.F;
	force.y = velocity.y / 10.F;
	if (m_entities.hasComponent<ska::cp::HitboxComponent>(entity)) {
		const auto& body = m_entities.getComponent<ska::cp::HitboxComponent>(entity);
		auto& space = m_worldState.getSpace();
		const auto vector = ska::cp::Vect{ velocity.x, velocity.y };
		space.getBody(body.bodyIndex).setVelocity(vector);
		space.getBody(body.controlBodyIndex).setVelocity(vector);
	}
}

void FighterSystem::face(ska::EntityId entity, ska::Point<float> direction) {
	auto& animation = m_componentAccessor.get<ska::AnimationComponent>(entity);
	animation.state = ska::RectangleUtils::getDirectionFromPos(ska::Point<int>{}, ska::Point<int>{ static_cast<int>(direction.x * 100.F), static_cast<int>(direction.y * 100.F) });
}

void FighterSystem::control(ska::EntityId entity, FighterComponent& fighter, unsigned int ellapsedTime) {
	const auto delta = m_stage.delta();
	fighter.stun = Decrease(fighter.stun, delta);
	fighter.release = Decrease(fighter.release, delta);
	fighter.flash = Decrease(fighter.flash, ellapsedTime);

	const auto fighting = m_stage.phase() == BattlePhase::Fight;
	const auto canMove = fighting && !m_stage.frozen() && !fighter.busy();

	// The keys move the Pokemon of the player through this system (the InputSystem does not move it)
	auto* input = m_componentPossibleAccessor.get<ska::InputComponent>(entity);
	if (fighter.player && input != nullptr) {
		if (m_playerPower < 0.F) {
			m_playerPower = input->movePower;
		}
		input->movePower = 0.F;
	}

	if (m_stage.frozen()) {
		setVelocity(entity, {});
		return;
	}

	const auto position = center(entity);
	const auto toOpponent = Normalized(center(fighter.opponent) - position);
	const auto& arena = m_stage.arena;
	const auto inside = position.x >= arena.x && position.x <= arena.x + arena.w && position.y >= arena.y && position.y <= arena.y + arena.h;

	// Leaving the arena : the player flees a wild Pokemon
	if (fighter.player && !fighter.controlledByAI && m_stage.canFlee) {
		fighter.outside = inside || !fighting ? 0 : fighter.outside + ellapsedTime;
	}

	auto move = ska::Point<float>{};
	if (canMove) {
		if (!fighter.controlledByAI) {
			move = keysDirection();
		} else {
			switch (fighter.intent) {
			case FighterIntent::Approach: move = toOpponent; break;
			case FighterIntent::Retreat: move = ska::Point<float>{ -toOpponent.x, -toOpponent.y }; break;
			// Around the opponent, slowly closer
			case FighterIntent::SidestepLeft: move = Normalized(ska::Point<float>{ -toOpponent.y + toOpponent.x * 0.2F, toOpponent.x + toOpponent.y * 0.2F }); break;
			case FighterIntent::SidestepRight: move = Normalized(ska::Point<float>{ toOpponent.y + toOpponent.x * 0.2F, -toOpponent.x + toOpponent.y * 0.2F }); break;
			default: break;
			}
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
		if ((inward.x != 0.F || inward.y != 0.F) && (canMove || !inside)) {
			move = Normalized(ska::Point<float>{ move.x + inward.x * 1.5F, move.y + inward.y * 1.5F });
		}
	}

	// The battle time can be slower (slow motion)
	const auto scale = ellapsedTime == 0 ? 0.F : static_cast<float>(delta) / static_cast<float>(ellapsedTime);
	const auto speed = WalkSpeed(m_componentAccessor.get<BattleComponent>(entity)) * scale;
	auto velocity = ska::Point<float>{ move.x * speed, move.y * speed };

	// Pushed back after a hit : the push slows down quickly
	if (std::abs(fighter.knockback.x) > 1.F || std::abs(fighter.knockback.y) > 1.F) {
		velocity.x += fighter.knockback.x * scale;
		velocity.y += fighter.knockback.y * scale;
		const auto decay = std::pow(0.8F, static_cast<float>(delta) / 20.F);
		fighter.knockback.x *= decay;
		fighter.knockback.y *= decay;
	} else {
		fighter.knockback = {};
	}
	setVelocity(entity, velocity);

	// It faces its opponent : always during an attack (the direction it committed to), else when it does not walk
	if (fighting) {
		if (fighter.windUp > 0 || fighter.lunge > 0 || fighter.recovery > 0) {
			face(entity, fighter.attackDirection);
		} else if (move.x == 0.F && move.y == 0.F) {
			face(entity, toOpponent);
		}
	}
}

void FighterSystem::draw(ska::EntityId entity, FighterComponent& fighter) {
	auto& effect = m_componentAccessor.get<ska::GraphicComponent>(entity).effect;
	effect = ska::RenderEffect{};

	// K.O. : a flash, then it sinks and disappears
	if (fighter.knockedOut) {
		const auto time = fighter.knockedOutTime;
		if (time < 300) {
			effect.flash = static_cast<unsigned char>((time / 60) % 2 == 0 ? 255 : 120);
			return;
		}
		const auto progress = std::min(1.F, static_cast<float>(time - 300) / 1300.F);
		effect.alpha = static_cast<unsigned char>(255.F * (1.F - progress));
		effect.scaleX = 1.F + 0.25F * progress;
		effect.scaleY = 1.F - 0.7F * progress;
		effect.green = static_cast<unsigned char>(255.F * (1.F - 0.5F * progress));
		effect.blue = static_cast<unsigned char>(255.F * (1.F - 0.5F * progress));
		return;
	}

	if (fighter.flash > 0) {
		effect.flash = static_cast<unsigned char>(255 * fighter.flash / FlashDuration);
	} else if (fighter.stun > 0 && (fighter.stun / 60) % 2 == 0) {
		// Stunned : it blinks
		effect.alpha = 70;
	}

	const auto& direction = fighter.attackDirection;
	if (fighter.windUp > 0 && fighter.windUpDuration > 0) {
		// The startup : it crouches and steps back a little (a contact skill takes its run-up)
		const auto progress = Ease(1.F - static_cast<float>(fighter.windUp) / static_cast<float>(fighter.windUpDuration));
		effect.scaleX = 1.F + 0.18F * progress;
		effect.scaleY = 1.F - 0.15F * progress;
		const auto back = (fighter.contactAttack ? 10.F : 4.F) * progress;
		effect.offsetX = static_cast<int>(-direction.x * back);
		effect.offsetY = static_cast<int>(-direction.y * back);
	} else if (fighter.lunge > 0) {
		// The headbutt : it lunges forward, stretched
		const auto progress = 1.F - static_cast<float>(fighter.lunge) / 140.F;
		const auto forward = 30.F * std::sin(std::min(1.F, progress * 1.6F) * 1.5708F);
		effect.offsetX = static_cast<int>(direction.x * forward);
		effect.offsetY = static_cast<int>(direction.y * forward);
		effect.scaleX = 1.F - 0.1F * std::abs(direction.y) + 0.12F * std::abs(direction.x);
		effect.scaleY = 1.F + 0.14F * std::abs(direction.y) - 0.06F * std::abs(direction.x);
	} else if (fighter.recovery > 0 && fighter.recoveryDuration > 0 && fighter.contactAttack) {
		// The recovery : it comes back where it stands
		const auto progress = static_cast<float>(fighter.recovery) / static_cast<float>(fighter.recoveryDuration);
		const auto forward = 30.F * Ease(progress);
		effect.offsetX = static_cast<int>(direction.x * forward);
		effect.offsetY = static_cast<int>(direction.y * forward);
	} else if (fighter.release > 0) {
		// A projectile goes : stretched
		const auto progress = static_cast<float>(fighter.release) / static_cast<float>(ReleaseDuration);
		effect.scaleX = 1.F - 0.12F * progress;
		effect.scaleY = 1.F + 0.16F * progress;
	}
}

void FighterSystem::refresh(unsigned int ellapsedTime) {
	if (m_received) {
		m_toggles = m_receivedToggles;
		m_receivedToggles.reset();
		m_received = false;
	}

	const auto& processed = getEntities();
	// The battle is over : the keys move the Pokemon again (its input goes back to the trainer)
	if (m_stage.phase() == BattlePhase::Over && m_playerPower >= 0.F) {
		auto* input = m_componentPossibleAccessor.get<ska::InputComponent>(m_stage.player);
		if (input != nullptr) {
			input->movePower = m_playerPower;
		}
	}
	for (const auto entity : processed) {
		auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
		if (processed.find(fighter.opponent) == processed.end()) {
			continue;
		}
		if (fighter.knockedOut) {
			// It does not fight anymore : it stops where it is, and sinks (draw)
			fighter.knockedOutTime += ellapsedTime;
			if (m_stage.phase() != BattlePhase::Over) {
				setVelocity(entity, {});
			}
		} else if (m_stage.phase() != BattlePhase::Intro && m_stage.phase() != BattlePhase::Over) {
			control(entity, fighter, ellapsedTime);
		}
		draw(entity, fighter);
	}
}
