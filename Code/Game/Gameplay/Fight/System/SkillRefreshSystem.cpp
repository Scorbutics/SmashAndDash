
#include "SkillRefreshSystem.h"
#include "Utils/TimeUtils.h"
#include "Utils/NumberUtils.h"
#include "Utils/PhysicUtils.h"
#include "Utils/RectangleUtils.h"
#include "Inputs/Readers/IniReader.h"
#include "Inputs/InputContextManager.h"
#include "../../CustomEntityManager.h"

SkillRefreshSystem::SkillRefreshSystem(ska::EntityManager& em) :
System(em) {

}

void SkillRefreshSystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	for (ska::EntityId entityId : processed) {
		auto& sc = m_componentAccessor.get<SkillComponent>(entityId);
		auto& mc = m_componentAccessor.get<ska::MovementComponent>(entityId);
		auto& pc = m_componentAccessor.get<ska::PositionComponent>(entityId);

		const unsigned int t = sc.currentTime;

		/* Example : sinusoidal move with noise */
		mc.vx = sc.speed + 1;
		mc.vy = sc.amplitude * static_cast<float>(ska::NumberUtils::sinus((t-sc.firstTime + sc.noise) * M_PI / 180));
		mc.vz = 0;
		/* End of example */

		auto pp = ska::PolarPoint<float>::polar(sc.target.x - sc.origin.x,  sc.target.y - sc.origin.y);
		ska::Point<float> emptyFloatPoint;
		ska::Point<float> velocityPoint(mc.vx, mc.vy);
		const auto& directionalPoint = ska::Point<float>::rotate(emptyFloatPoint, pp.angle, velocityPoint);
		mc.vx = directionalPoint.x;
		mc.vy = directionalPoint.y;

		/* Max range reached : delete the skill */
		if (static_cast<unsigned int>(ska::RectangleUtils::distanceSquared<int>(sc.origin, pc)) > sc.range*sc.range) {
			scheduleDeferredRemove(entityId);
		}

		sc.lastTime = sc.currentTime;
		sc.currentTime = ska::TimeUtils::getTicks();
	}
}

SkillRefreshSystem::~SkillRefreshSystem() {
}
