#include "Game/__internalConfig/LoggerConfig.h"
#include "SkillRefreshSystem.h"
#include "Core/Utils/TimeUtils.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "Core/Utils/PhysicUtils.h"
#include "Core/Utils/RectangleUtils.h"
#include "Base/IO/Readers/IniReader.h"
#include "Core/Inputs/InputContextManager.h"
#include "../../CustomEntityManager.h"
#include <SDL3/SDL_stdinc.h>
#include <algorithm>
#include <cmath>

SkillRefreshSystem::SkillRefreshSystem(ska::EntityManager& em, BattleStage& stage) :
	System(em),
	m_stage(stage) {

}

void SkillRefreshSystem::refresh(unsigned int ellapsedTime) {
	// A step per logic update (20 ms) : shorter in slow motion, none during a hit stop
	const auto timeFactor = ellapsedTime == 0 ? 0.F : static_cast<float>(m_stage.delta()) / static_cast<float>(ellapsedTime);
	if (timeFactor <= 0.F) {
		return;
	}
	const auto& processed = getEntities();
	for (ska::EntityId entityId : processed) {
		auto& sc = m_componentAccessor.get<SkillComponent>(entityId);
		auto& mc = m_componentAccessor.get<ska::MovementComponent>(entityId);
		auto& pc = m_componentAccessor.get<ska::PositionComponent>(entityId);

		const unsigned int t = sc.currentTime;

		/* Example : sinusoidal move with noise */
		// At least 300 pixels per second : a slow skill is avoided too easily and stays on the screen
		mc.vx = std::max(sc.speed + 1, 6.F);
		mc.vy = sc.amplitude * static_cast<float>(ska::NumberUtils::sinus((t-sc.firstTime + sc.noise) * M_PI / 180));
		mc.vz = 0;
		/* End of example */

		auto pp = ska::PolarPoint<float>::polar(sc.target.x - sc.origin.x,  sc.target.y - sc.origin.y);
		ska::Point<float> emptyFloatPoint;
		ska::Point<float> velocityPoint(mc.vx, mc.vy);
		const auto& directionalPoint = ska::Point<float>::rotate(emptyFloatPoint, pp.angle, velocityPoint);
		mc.vx = directionalPoint.x;
		mc.vy = directionalPoint.y;
		// A skill has no physics body : it moves by itself (pixels per frame), through the obstacles.
		// It crosses its range in 6 frames at least : a fast skill (ex : a melee attack) can touch its target.
		const auto maxStep = std::max(4.F, static_cast<float>(sc.range) / 6.F);
		const auto step = std::sqrt(mc.vx * mc.vx + mc.vy * mc.vy);
		const auto factor = (step > maxStep ? maxStep / step : 1.F) * timeFactor;
		pc.x += static_cast<long>(mc.vx * factor);
		pc.y += static_cast<long>(mc.vy * factor);

		/* Max range reached : delete the skill */
		if (static_cast<unsigned int>(ska::RectangleUtils::distanceSquared<int>(sc.origin, pc)) > sc.range*sc.range) {
			if (!sc.spent) {
				SLOG(ska::LogLevel::Debug) << "Battle : a skill missed at " << pc.x << ", " << pc.y;
			}
			scheduleDeferredRemove(entityId);
		}

		sc.lastTime = sc.currentTime;
		sc.currentTime = ska::TimeUtils::getTicks();
	}
}

SkillRefreshSystem::~SkillRefreshSystem() {
}
