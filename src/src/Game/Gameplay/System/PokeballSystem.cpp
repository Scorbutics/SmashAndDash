#include "Game/__internalConfig/LoggerConfig.h"
#include "PokeballSystem.h"
#include <algorithm>
#include <cmath>
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"

namespace {
	// The ball is drawn bigger than its sprite (16 pixels : too small on the map)
	constexpr float BallScale = 1.5F;

	float EaseOut(float t) {
		return 1.F - (1.F - t) * (1.F - t);
	}

	ska::EntityId Sprite(ska::EntityManager& entities, const ska::AnimatedTexture& texture) {
		const auto entity = entities.createEntity();
		auto graphic = ska::GraphicComponent{};
		graphic.animatedSprites.push_back(texture);
		entities.addComponent<ska::GraphicComponent>(entity, std::move(graphic));
		// Drawn by the GraphicSystem (it needs a hitbox : none here, the sprite is placed by its corner)
		entities.addComponent<ska::HitboxComponent>(entity, ska::HitboxComponent{});
		entities.addComponent<ska::PositionComponent>(entity, ska::PositionComponent{});
		return entity;
	}
}

PokeballSystem::PokeballSystem(ska::EntityManager& entityManager) :
	System(entityManager),
	m_entities(entityManager) {
	m_spin.load("./Sprites/Fight/pokeball.png", 4, 1, 4);
	m_spin.setDelay(70);
	m_open.load("./Sprites/Fight/pokeball-open.png", 1, 1, 1);
	m_burst.load("./Sprites/Fight/pokeball-burst.png", 1, 1, 1);
}

ska::EntityId PokeballSystem::Throw(ska::EntityManager& entityManager, ska::Point<float> from, ska::Point<float> to, bool returning) {
	auto ball = PokeballComponent{};
	ball.from = from;
	ball.to = to;
	ball.returning = returning;
	// Longer for a longer throw, an arc higher for a longer throw
	const auto distance = std::hypot(to.x - from.x, to.y - from.y);
	ball.duration = static_cast<unsigned int>(std::clamp(300.F + distance * 1.6F, 380.F, 800.F));
	ball.height = std::clamp(20.F + distance * 0.35F, 30.F, 80.F);
	const auto entity = entityManager.createEntity();
	entityManager.addComponent<ska::HitboxComponent>(entity, ska::HitboxComponent{});
	auto position = ska::PositionComponent{};
	position.x = from.x;
	position.y = from.y;
	entityManager.addComponent<ska::PositionComponent>(entity, std::move(position));
	entityManager.addComponent<ska::GraphicComponent>(entity, ska::GraphicComponent{});
	entityManager.addComponent<PokeballComponent>(entity, std::move(ball));
	return entity;
}

void PokeballSystem::place(ska::EntityId entity, ska::Point<float> ground, float height, int width, int spriteHeight) {
	// The GraphicSystem draws the sprite from (x, y - 5) without a hitbox : its bottom centre on the point of the ground
	auto& position = m_componentAccessor.get<ska::PositionComponent>(entity);
	position.x = ground.x - static_cast<float>(width) / 2.F;
	position.y = ground.y - static_cast<float>(spriteHeight) + 5.F;
	position.z = static_cast<long>(std::lround(height));
}

void PokeballSystem::open(ska::EntityId entity, PokeballComponent& ball) {
	ball.state = PokeballComponent::State::Open;
	ball.elapsed = 0;
	auto& graphic = m_componentAccessor.get<ska::GraphicComponent>(entity);
	graphic.animatedSprites.clear();
	graphic.animatedSprites.push_back(m_open);
	// The burst of light, centred on the ball
	ball.burst = Sprite(m_entities, m_burst);
	auto& position = m_entities.getComponent<ska::PositionComponent>(*ball.burst);
	const auto size = static_cast<float>(m_burst.getWidth());
	position.x = ball.to.x - size / 2.F;
	// Its centre at the middle of the ball, drawn a little over the ground (sorted over the Pokemon coming out)
	position.y = ball.to.y - size / 2.F + 5.F - 8.F * BallScale;
}

void PokeballSystem::refresh(unsigned int ellapsedTime) {
	const auto& processed = getEntities();
	for (const auto entity : processed) {
		auto& ball = m_componentAccessor.get<PokeballComponent>(entity);
		auto& graphic = m_componentAccessor.get<ska::GraphicComponent>(entity);
		ball.elapsed += ellapsedTime;

		switch (ball.state) {
		case PokeballComponent::State::Flying: {
			if (graphic.animatedSprites.empty()) {
				graphic.animatedSprites.push_back(m_spin);
			}
			const auto t = std::min(1.F, static_cast<float>(ball.elapsed) / static_cast<float>(ball.duration));
			// On the ground : fast then slower ; in the air : an arc
			const auto ground = ska::Point<float>{ ball.from.x + (ball.to.x - ball.from.x) * EaseOut(t), ball.from.y + (ball.to.y - ball.from.y) * EaseOut(t) };
			// Thrown from the hands of the trainer (a little over the ground) and caught back there
			const auto hands = 14.F;
			const auto start = ball.returning ? 10.F : hands;
			const auto end = ball.returning ? hands : 0.F;
			const auto z = start + (end - start) * t + 4.F * ball.height * t * (1.F - t);
			place(entity, ground, z, 16, 16);
			graphic.effect = ska::RenderEffect{};
			graphic.effect.scaleX = graphic.effect.scaleY = BallScale;
			if (t >= 1.F) {
				if (ball.returning) {
					ball.state = PokeballComponent::State::Gone;
					break;
				}
				ball.state = PokeballComponent::State::Bouncing;
				ball.elapsed = 0;
			}
			break;
		}
		case PokeballComponent::State::Bouncing: {
			// A small hop where it lands, the ball still (its first frame), then it opens
			const auto t = std::min(1.F, static_cast<float>(ball.elapsed) / static_cast<float>(BounceDuration));
			place(entity, ball.to, 10.F * 4.F * t * (1.F - t), 16, 16);
			if (!graphic.animatedSprites.empty()) {
				graphic.animatedSprites[0].stop(true);
			}
			// It wobbles before opening
			graphic.effect.scaleX = BallScale * (1.F + 0.12F * std::sin(t * 12.F));
			graphic.effect.scaleY = BallScale * (1.F - 0.12F * std::sin(t * 12.F));
			if (t >= 1.F) {
				place(entity, ball.to, 0.F, 16, 16);
				if (ball.capture) {
					// The wild Pokemon goes in : the ball shakes on the ground
					ball.state = PokeballComponent::State::Shaking;
					ball.elapsed = 0;
				} else {
					open(entity, ball);
				}
			}
			break;
		}
		case PokeballComponent::State::Shaking: {
			// Still while the Pokemon goes in, then each shake : it rocks to one side and back
			graphic.effect = ska::RenderEffect{};
			graphic.effect.scaleX = graphic.effect.scaleY = BallScale;
			const auto rocking = ball.elapsed > ShakeDelay ? ball.elapsed - ShakeDelay : 0u;
			const auto shake = rocking / ShakeDuration;
			const auto phase = static_cast<float>(rocking % ShakeDuration) / static_cast<float>(ShakeDuration);
			auto offset = 0.F;
			if (static_cast<int>(shake) < ball.shakes && phase < 0.5F) {
				offset = 4.F * std::sin(phase * 2.F * 3.14159F) * (shake % 2 == 0 ? 1.F : -1.F);
			}
			place(entity, ska::Point<float>{ ball.to.x + offset, ball.to.y }, 0.F, 16, 16);
			if (ball.elapsed >= ShakeDelay + static_cast<unsigned int>(std::max(1, ball.shakes)) * ShakeDuration) {
				ball.elapsed = 0;
				if (ball.caught) {
					ball.state = PokeballComponent::State::Caught;
				} else {
					open(entity, ball);
				}
			}
			break;
		}
		case PokeballComponent::State::Caught: {
			// Click : it flashes, stays a moment, then fades
			const auto t = std::min(1.F, static_cast<float>(ball.elapsed) / static_cast<float>(CaughtDuration));
			graphic.effect = ska::RenderEffect{};
			graphic.effect.scaleX = graphic.effect.scaleY = BallScale;
			graphic.effect.flash = static_cast<unsigned char>(t < 0.25F ? 255.F * (1.F - t / 0.25F) : 0.F);
			graphic.effect.alpha = static_cast<unsigned char>(255.F * (t < 0.7F ? 1.F : 1.F - (t - 0.7F) / 0.3F));
			if (t >= 1.F) {
				ball.state = PokeballComponent::State::Gone;
			}
			break;
		}
		case PokeballComponent::State::Open: {
			// The light bursts, grows and fades ; the ball flashes and fades
			const auto t = std::min(1.F, static_cast<float>(ball.elapsed) / static_cast<float>(OpenDuration));
			graphic.effect = ska::RenderEffect{};
			graphic.effect.scaleX = graphic.effect.scaleY = BallScale;
			graphic.effect.flash = static_cast<unsigned char>(255.F * std::max(0.F, 1.F - t * 3.F));
			graphic.effect.alpha = static_cast<unsigned char>(255.F * (t < 0.6F ? 1.F : 1.F - (t - 0.6F) / 0.4F));
			if (ball.burst.has_value() && m_entities.hasComponent<ska::GraphicComponent>(*ball.burst)) {
				auto& light = m_entities.getComponent<ska::GraphicComponent>(*ball.burst).effect;
				light = ska::RenderEffect{};
				light.anchorBottom = false;
				light.scaleX = light.scaleY = 0.3F + 1.5F * EaseOut(std::min(1.F, t * 1.6F));
				light.alpha = static_cast<unsigned char>(255.F * std::max(0.F, 1.F - t));
			}
			if (t >= 1.F) {
				ball.state = PokeballComponent::State::Gone;
			}
			break;
		}
		case PokeballComponent::State::Gone:
			break;
		}

		if (ball.state == PokeballComponent::State::Gone) {
			if (ball.burst.has_value()) {
				scheduleDeferredRemove(*ball.burst);
				ball.burst.reset();
			}
			graphic.animatedSprites.clear();
			scheduleDeferredRemove(entity);
		}
	}
}
