
#include "SkillRefreshSystem.h"
#include "../../../ska/Utils/TimeUtils.h"
#include "../../../ska/Utils/NumberUtils.h"
#include "../../../ska/Utils/PhysicUtils.h"
#include "../../../ska/Utils/RectangleUtils.h"
#include "../../../ska/Inputs/Readers/IniReader.h"
#include "../../../ska/Inputs/InputContextManager.h"
#include "../../CustomEntityManager.h"

SkillRefreshSystem::SkillRefreshSystem(ska::EntityManager& em) :
System(em) {

}

void SkillRefreshSystem::refresh() {
	for (ska::EntityId entityId : m_processed) {
		auto& sc = m_entityManager.getComponent<SkillComponent>(entityId);
		auto& mc = m_entityManager.getComponent<ska::MovementComponent>(entityId);
		auto& pc = m_entityManager.getComponent<ska::PositionComponent>(entityId);

		const unsigned int t = sc.currentTime;

		/* Example : sinusoidal move with noise */
		mc.vx = sc.speed + 1;
		mc.vy = sc.amplitude * static_cast<float>(ska::NumberUtils::sinus((t-sc.firstTime + sc.noise) * M_PI / 180));
		mc.vz = 0;
		/* End of example */

		auto pp = ska::NumberUtils::polar(sc.target.x - sc.origin.x,  sc.target.y - sc.origin.y);
		ska::Point<float> emptyFloatPoint;
		ska::Point<float> velocityPoint(mc.vx, mc.vy);
		const auto& directionalPoint = ska::NumberUtils::rotate<float>(emptyFloatPoint, pp.angle, velocityPoint);
		mc.vx = directionalPoint.x;
		mc.vy = directionalPoint.y;

		/* Max range reached : delete the skill */
		if (ska::RectangleUtils::distanceSquared(sc.origin, pc) > sc.range*sc.range) {
			scheduleDeferredRemove(entityId);
		}

		sc.lastTime = sc.currentTime;
		sc.currentTime = ska::TimeUtils::getTicks();
	}
}

SkillRefreshSystem::~SkillRefreshSystem() {
}
