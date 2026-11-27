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
#include "../../CustomEntityManager.h"

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
	// The dodge (the jump key)
	if (event.icm.getActions()[ska::Jump]) {
		m_dodgeKey = true;
	}
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
	fighter.guard = Decrease(fighter.guard, delta);
	fighter.dash = Decrease(fighter.dash, delta);
	fighter.invulnerable = Decrease(fighter.invulnerable, delta);
	fighter.dashCooldown = Decrease(fighter.dashCooldown, delta);
	updateEffects(entity, fighter, delta);

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

	auto position = center(entity);

	// Never inside a wall : only the outline of a wall stops the bodies, a fast push (a hit, a dash) can cross it
	const auto& world = m_worldState.getWorld();
	const auto blockSize = static_cast<float>(world.getBlockSize());
	const auto& positionComponent = m_componentAccessor.get<ska::PositionComponent>(entity);
	const auto inWall = position.x >= 0.F && position.y >= 0.F && world.getCollisionProfile().collide(1, static_cast<std::size_t>(position.x / blockSize), static_cast<std::size_t>(position.y / blockSize));
	if (inWall) {
		if (fighter.hasLastFree) {
			CustomEntityManager::place(m_entities, m_worldState.getSpace(), entity, fighter.lastFree);
			fighter.knockback = {};
			fighter.dash = 0;
			position = center(entity);
		}
	} else {
		fighter.lastFree = ska::Point<float>{ static_cast<float>(positionComponent.x), static_cast<float>(positionComponent.y) };
		fighter.hasLastFree = true;
	}

	const auto toOpponent = Normalized(center(fighter.opponent) - position);
	const auto& arena = m_stage.arena;
	const auto inside = position.x >= arena.x && position.x <= arena.x + arena.w && position.y >= arena.y && position.y <= arena.y + arena.h;

	// Leaving the arena : the player flees a wild Pokemon. Walking out (a key held), not pushed out by a hit
	const auto keys = fighter.player && !fighter.controlledByAI ? keysDirection() : ska::Point<float>{};
	const auto walking = canMove && (keys.x != 0.F || keys.y != 0.F);
	if (fighter.player && !fighter.controlledByAI && m_stage.canFlee) {
		fighter.outside = inside || !fighting ? 0 : (walking ? fighter.outside + ellapsedTime : fighter.outside);
	}

	auto move = ska::Point<float>{};
	if (canMove) {
		if (!fighter.controlledByAI) {
			// The keys move the Pokemon of the player ; a wild Pokemon without its AI (SAD_BATTLE_PASSIVE) stays
			move = fighter.player ? keysDirection() : ska::Point<float>{};
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

	// The AI (and a player that cannot flee) stays in the arena : near an edge, it goes back inside. A player pushed out
	// of it (by a hit) that does not walk goes back inside too
	if (fighting && (fighter.controlledByAI || !m_stage.canFlee || (!inside && !walking))) {
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

	// The battle time can be slower (slow motion) ; the speed changes with its statistics, a paralysis slows it down
	const auto scale = ellapsedTime == 0 ? 0.F : static_cast<float>(delta) / static_cast<float>(ellapsedTime);
	const auto slowed = fighter.statusSpeed == 100 ? 1.F : static_cast<float>(fighter.statusSpeed) / 100.F;
	const auto speed = WalkSpeed(m_componentAccessor.get<BattleComponent>(entity)) * fighter.stageMultipliers[StatSpeed] * slowed * scale;
	auto velocity = ska::Point<float>{ move.x * speed, move.y * speed };

	// The dodge : a short dash (the keys, or away from the opponent ; the AI goes aside), it cannot be hit at its start
	const auto wantsDodge = fighter.controlledByAI ? fighter.dodgeRequested : (fighter.player && m_dodgeKey);
	if (fighter.player && !fighter.controlledByAI) {
		m_dodgeKey = false;
	}
	fighter.dodgeRequested = false;
	if (wantsDodge && canMove && fighter.dashCooldown == 0) {
		auto direction = fighter.controlledByAI ? ska::Point<float>{} : keysDirection();
		if (direction.x == 0.F && direction.y == 0.F) {
			const auto side = (entity % 2 == 0) ? 1.F : -1.F;
			direction = fighter.controlledByAI ? Normalized(ska::Point<float>{ -toOpponent.y * side - toOpponent.x * 0.3F, toOpponent.x * side - toOpponent.y * 0.3F }) : ska::Point<float>{ -toOpponent.x, -toOpponent.y };
		}
		fighter.dashDirection = direction;
		fighter.dash = DashDuration;
		fighter.invulnerable = DashInvulnerability;
		fighter.dashCooldown = DashCooldown;
		m_stage.sound(BattleSound::Dash);
		SLOG(ska::LogLevel::Debug) << "Battle : " << (fighter.player ? "the pokemon of the player" : "a wild pokemon") << " dodges";
	}
	if (fighter.dash > 0) {
		velocity = ska::Point<float>{ fighter.dashDirection.x * DashSpeed * scale, fighter.dashDirection.y * DashSpeed * scale };
	}

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

	// Recalled in its pokeball : it turns red, flashes and shrinks into the light, then it is gone
	if (fighter.recall >= 0) {
		const auto progress = std::min(1.F, static_cast<float>(fighter.recall) / static_cast<float>(FighterComponent::RecallDuration));
		if (progress >= 1.F || fighter.knockedOut) {
			effect.alpha = 0;
			return;
		}
		effect.green = effect.blue = static_cast<unsigned char>(255.F * (1.F - 0.75F * std::min(1.F, progress * 3.F)));
		effect.flash = static_cast<unsigned char>(200.F * std::min(1.F, progress * 2.F));
		const auto shrink = progress < 0.35F ? 1.F : 1.F - (progress - 0.35F) / 0.65F;
		effect.scaleX = std::max(0.05F, shrink * (1.F - 0.3F * progress));
		effect.scaleY = std::max(0.05F, shrink);
		return;
	}

	// Out of its pokeball : it grows out of the light, white, a little too big, then settles
	if (fighter.appear < FighterComponent::AppearDuration) {
		const auto progress = static_cast<float>(fighter.appear) / static_cast<float>(FighterComponent::AppearDuration);
		const auto grow = progress < 0.7F ? 0.15F + 1.0F * progress / 0.7F : 1.15F - 0.15F * (progress - 0.7F) / 0.3F;
		effect.scaleX = effect.scaleY = grow;
		effect.flash = static_cast<unsigned char>(255.F * std::max(0.F, 1.F - progress * 1.3F));
		return;
	}

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

	// Its status colors it
	switch (fighter.status) {
	case FighterStatus::Poison: effect.red = 205; effect.green = 140; effect.blue = 255; break;
	case FighterStatus::Burn: effect.red = 255; effect.green = 165; effect.blue = 120; break;
	case FighterStatus::Paralysis: if ((m_time / 120) % 3 == 0) { effect.blue = 90; } break;
	case FighterStatus::Sleep: effect.red = 150; effect.green = 150; effect.blue = 210; break;
	case FighterStatus::Leech: effect.red = 170; effect.green = 255; effect.blue = 150; break;
	default: break;
	}
	if (fighter.guard > 0) {
		// Protected : a blue glow
		effect.red = 160;
		effect.green = 210;
		effect.flash = static_cast<unsigned char>(90 + 60 * ((m_time / 90) % 2));
	}
	if (fighter.flash > 0) {
		effect.flash = static_cast<unsigned char>(255 * fighter.flash / FlashDuration);
	} else if (fighter.stun > 0 && (fighter.stun / 60) % 2 == 0) {
		// Stunned : it blinks
		effect.alpha = 70;
	}
	if (fighter.dash > 0) {
		// Dashing : light, stretched
		effect.alpha = 150;
		effect.scaleX = 1.F + 0.15F * std::abs(fighter.dashDirection.x);
		effect.scaleY = 1.F + 0.15F * std::abs(fighter.dashDirection.y);
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

void FighterSystem::updateEffects(ska::EntityId entity, FighterComponent& fighter, unsigned int delta) {
	// The changes of its statistics end
	for (std::size_t stat = 0; stat < fighter.stages.size(); stat++) {
		if (fighter.stageTime[stat] > 0) {
			fighter.stageTime[stat] = Decrease(fighter.stageTime[stat], delta);
			if (fighter.stageTime[stat] == 0) {
				fighter.stages[stat] = 0;
				fighter.stageMultipliers[stat] = 1.F;
			}
		}
	}
	if (fighter.status == FighterStatus::None) {
		return;
	}
	// Its status hurts it over time (the rules of its status, asked when it landed : poison, burn, leech heals the one
	// who gave it)
	const auto hurts = fighter.statusTickPeriod > 0 && fighter.statusDamage > 0;
	if (hurts && delta > 0) {
		fighter.statusTick = Decrease(fighter.statusTick, delta);
		if (fighter.statusTick == 0) {
			fighter.statusTick = fighter.statusTickPeriod;
			auto& bc = m_componentAccessor.get<BattleComponent>(entity);
			const auto damage = fighter.statusDamage;
			bc.hp -= damage;
			auto mark = BattleMark{};
			mark.kind = BattleMark::Kind::Damage;
			mark.position = center(entity);
			mark.value = damage;
			mark.onPlayer = fighter.player;
			m_stage.mark(mark);
			fighter.hpDelay = 300;
			if (fighter.statusLeech && m_entities.hasComponent<BattleComponent>(fighter.statusSource)) {
				auto& source = m_entities.getComponent<BattleComponent>(fighter.statusSource);
				if (source.hp > 0) {
					source.hp = std::min(source.maxHp, source.hp + damage);
				}
			}
		}
	}
	fighter.statusTime = Decrease(fighter.statusTime, delta);
	if (fighter.statusTime == 0) {
		fighter.clearStatus();
	}
}

void FighterSystem::refresh(unsigned int ellapsedTime) {
	m_time += ellapsedTime;
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
		// Without an opponent (gone) it does not move, but it is still drawn : its animations go on (coming out of a
		// pokeball, going in it), else its drawing would stay as it was (ex : a few pixels, shrunk in a pokeball)
		const auto hasOpponent = processed.find(fighter.opponent) != processed.end();
		// Coming out of its pokeball, going back in it : real time (the hit stops do not hold them)
		if (fighter.appear < FighterComponent::AppearDuration) {
			fighter.appear = std::min(FighterComponent::AppearDuration, fighter.appear + ellapsedTime);
		}
		if (fighter.recall >= 0 && fighter.recall < FighterComponent::RecallDuration) {
			fighter.recall = std::min(FighterComponent::RecallDuration, fighter.recall + static_cast<int>(ellapsedTime));
		}
		if (fighter.knockedOut) {
			// It does not fight anymore : it stops where it is, and sinks (draw)
			fighter.knockedOutTime += ellapsedTime;
			if (m_stage.phase() != BattlePhase::Over) {
				setVelocity(entity, {});
			}
		} else if (m_stage.phase() == BattlePhase::Outro) {
			// The battle is over : everybody stands still
			setVelocity(entity, {});
		} else if (hasOpponent && m_stage.phase() != BattlePhase::Intro && m_stage.phase() != BattlePhase::Over) {
			control(entity, fighter, ellapsedTime);
		}
		draw(entity, fighter);
	}
}
