
#include "SkillRefreshSystem.h"
#include "../../../ska/Utils/TimeUtils.h"
#include "../../../ska/Utils/NumberUtils.h"
#include "../../../ska/Utils/PhysicUtils.h"
#include "../../../ska/Inputs/Readers/IniReader.h"
#include "../../../ska/Inputs/InputContextManager.h"
#include "../../CustomEntityManager.h"

SkillRefreshSystem::SkillRefreshSystem(ska::EntityManager& em) :
System(em) {

}

void SkillRefreshSystem::refresh() {
	for (ska::EntityId entityId : m_processed) {
		SkillComponent& sc = m_entityManager.getComponent<SkillComponent>(entityId);
		ska::PositionComponent& pc = m_entityManager.getComponent<ska::PositionComponent>(entityId);

		const unsigned int t = sc.currentTime;

		/* Example : sinusoidal move with noise */
		pc.x = sc.amplitude * ska::NumberUtils::cosinus(t * sc.speed + sc.noise);
		pc.y = sc.amplitude * ska::NumberUtils::sinus(t * sc.speed + sc.noise);
		/* End of example */

		ska::PolarPoint<int> pp = ska::NumberUtils::polar(sc.target.x - sc.origin.x, sc.target.y - sc.origin.y);
		pc = ska::NumberUtils::rotate<int>(sc.origin, pp.angle, pc);

		sc.lastTime = sc.currentTime;
		sc.currentTime = ska::TimeUtils::getTicks();
	}
}

SkillRefreshSystem::~SkillRefreshSystem() {
}