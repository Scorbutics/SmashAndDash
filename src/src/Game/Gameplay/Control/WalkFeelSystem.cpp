#include "Game/__internalConfig/LoggerConfig.h"
#include "WalkFeelSystem.h"
#include <algorithm>
#include <cmath>
#include "Core/Draw/DrawableContainer.h"
#include "Graphic/System/CameraSystem.h"
#include "World/TileWorld.h"
#include "GridMovementSystem.h"
#include "../../Utils/IDs.h"

namespace {
	constexpr float Pi = 3.14159265F;
	// The spring of the scale : its frequency (Hz), its damping (1 : no bounce)
	constexpr float SpringFrequency = 7.F;
	constexpr float SpringDamping = 0.45F;
}

WalkFeelSystem::WalkFeelSystem(ska::EntityManager& entityManager, ska::CameraSystem& camera, const ska::TileWorld& world) :
	System(entityManager),
	m_camera(camera),
	m_world(world),
	m_layer([this](Ui::Canvas& canvas) { render(canvas); }, WalkDustPriority) {
}

void WalkFeelSystem::graphicUpdate(ska::DrawableContainer& drawables) {
	drawables.add(m_layer);
}

void WalkFeelSystem::reset(ska::GraphicComponent& graphic) {
	graphic.effect.scaleX = 1.F;
	graphic.effect.scaleY = 1.F;
	graphic.effect.offsetY = 0;
}

void WalkFeelSystem::puff(ska::Point<float> feet, ska::Point<float> drift, bool water) {
	// A little variety (drawn only : not the random numbers of the game)
	m_seed = m_seed * 1103515245u + 12345u;
	const auto jitter = static_cast<float>((m_seed >> 16) % 9) - 4.F;
	auto p = Puff{};
	p.position = { feet.x + jitter, feet.y - 2.F };
	p.drift = { drift.x + jitter * 3.F, drift.y - 14.F };
	p.water = water;
	m_puffs.push_back(p);
}

void WalkFeelSystem::refresh(unsigned int ellapsedTime) {
	if (m_world.getBlockSize() > 0) {
		m_blockSize = static_cast<float>(m_world.getBlockSize());
	}
	const auto dt = static_cast<float>(std::min(ellapsedTime, 50u));
	for (auto& p : m_puffs) {
		p.age += ellapsedTime;
		p.position.x += p.drift.x * dt / 1000.F;
		p.position.y += p.drift.y * dt / 1000.F;
	}
	m_puffs.erase(std::remove_if(m_puffs.begin(), m_puffs.end(), [](const Puff& p) { return p.age >= PuffLife; }), m_puffs.end());

	const auto walkSpeed = m_blockSize * 1000.F / static_cast<float>(GridMovementSystem::WalkStep);
	for (const auto entity : getEntities()) {
		const auto& grid = m_componentAccessor.get<GridMovementComponent>(entity);
		if (grid.mode != GridMovementComponent::Mode::Keys) {
			continue;
		}
		const auto& position = m_componentAccessor.get<ska::PositionComponent>(entity);
		const auto& animation = m_componentAccessor.get<ska::AnimationComponent>(entity);
		auto& graphic = m_componentAccessor.get<ska::GraphicComponent>(entity);
		auto& feel = m_feels[entity];

		// What it really walked since the last step (a teleport, a map change : nothing)
		const auto current = ska::Point<long>{ position.x, position.y };
		auto delta = ska::Point<float>{};
		if (feel.known) {
			delta = { static_cast<float>(current.x - feel.last.x), static_cast<float>(current.y - feel.last.y) };
			if (std::abs(delta.x) + std::abs(delta.y) > m_blockSize) {
				delta = {};
			}
		}
		feel.last = current;
		feel.known = true;
		const auto length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
		const auto speed = dt > 0.F ? length * 1000.F / dt : 0.F;
		const auto moving = speed > 20.F;
		const auto onWater = animation.animatedWhenStill;
		const auto feet = ska::Point<float>{ static_cast<float>(position.x) + m_blockSize / 2.F, static_cast<float>(position.y) + m_blockSize - 2.F };

		// Starts, stops, hops, lands : the spring is pushed
		if (moving && !feel.moving) {
			feel.stretch = -0.07F;
		} else if (!moving && feel.moving) {
			feel.stretch = -0.05F;
		}
		if (position.z > 0 && feel.z == 0) {
			feel.stretch = 0.10F;
		} else if (position.z == 0 && feel.z > 0) {
			feel.stretch = -0.12F;
			for (const auto side : { -1.F, 0.F, 1.F }) {
				puff({ feet.x + side * 10.F, feet.y }, { side * 40.F, 0.F }, onWater);
			}
		}
		feel.moving = moving;
		feel.z = position.z;
		feel.still = moving ? 0 : feel.still + ellapsedTime;

		// The feet : a bob at each one, a puff behind it when running (a ripple on the water)
		auto bob = 0.F;
		if (moving) {
			const auto halfBlock = m_blockSize / 2.F;
			feel.distance += length;
			const auto foot = static_cast<int>(feel.distance / halfBlock);
			const auto running = speed > walkSpeed * 1.35F;
			if (foot != feel.foot && (running || onWater)) {
				const auto away = ska::Point<float>{ -delta.x / length, -delta.y / length };
				puff({ feet.x + away.x * 8.F, feet.y + away.y * 4.F }, { away.x * 30.F, away.y * 10.F }, onWater);
			}
			feel.foot = foot;
			if (!onWater) {
				bob = (running ? RunBobHeight : BobHeight) * std::abs(std::sin(Pi * feel.distance / halfBlock));
			}
		}

		// The spring (semi-implicit Euler) back to the normal scale
		const auto omega = 2.F * Pi * SpringFrequency;
		const auto seconds = dt / 1000.F;
		feel.stretchSpeed += (-omega * omega * feel.stretch - 2.F * SpringDamping * omega * feel.stretchSpeed) * seconds;
		feel.stretch += feel.stretchSpeed * seconds;
		if (std::abs(feel.stretch) < 0.002F && std::abs(feel.stretchSpeed) < 0.02F) {
			feel.stretch = 0.F;
			feel.stretchSpeed = 0.F;
		}

		// Breathing, slowly coming
		auto breath = 0.F;
		if (feel.still > BreathAfter && !onWater && position.z == 0) {
			const auto since = static_cast<float>(feel.still - BreathAfter);
			const auto ramp = std::min(1.F, since / 600.F);
			breath = 0.02F * ramp * (0.5F - 0.5F * std::cos(2.F * Pi * since / 1800.F));
		}

		graphic.effect.anchorBottom = true;
		graphic.effect.scaleY = 1.F + feel.stretch + breath;
		graphic.effect.scaleX = 1.F - feel.stretch * 0.6F;
		graphic.effect.offsetY = -static_cast<int>(std::lround(bob));
	}
}

void WalkFeelSystem::render(Ui::Canvas& canvas) const {
	const auto& camera = m_camera.getDisplay();
	for (const auto& p : m_puffs) {
		const auto t = static_cast<float>(p.age) / static_cast<float>(PuffLife);
		// A small cloud : it grows and fades
		const auto size = static_cast<int>(6.F + 8.F * t);
		const auto alpha = static_cast<unsigned char>(210.F * (1.F - t) * (1.F - t));
		const auto colour = p.water ? ska::Color{ 235, 245, 255, alpha } : ska::Color{ 214, 204, 182, alpha };
		const auto x = static_cast<int>(std::lround(p.position.x)) - camera.x;
		const auto y = static_cast<int>(std::lround(p.position.y)) - camera.y;
		const auto side = size * 2 / 3;
		canvas.fill(colour, ska::Rectangle{ x - size / 2, y - side / 2, size, side });
		canvas.fill(colour, ska::Rectangle{ x - side / 2, y - size / 2, side, size - side / 2 });
		canvas.fill(colour, ska::Rectangle{ x - size / 2 - side / 3, y, side / 2, side / 2 });
	}
}
