#include "GravitySystem.h"
#include "Utils/SkaConstants.h"

ska::GravitySystem::GravitySystem(EntityManager& entityManager) : System(entityManager) {
}

void ska::GravitySystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	for (auto entityId : processed) {
		auto& forceComponent = m_componentAccessor.get<ForceComponent>(entityId);
		auto& gaComponent = m_componentAccessor.get<GravityAffectedComponent>(entityId);
		auto& moveComponent = m_componentAccessor.get<MovementComponent>(entityId);

		//PFD selon x et y avec la force de frottements fluides
		//( en comptant qu'au préalable on a peut-être déjà appliqué une ou plusieurs force(s) )
		forceComponent.x -= gaComponent.friction * moveComponent.vx;
		forceComponent.y -= gaComponent.friction * moveComponent.vy;

		//EARTH_GRAVITY était trop élevée alors j'ai préféré la diviser par 5
		forceComponent.z -= (EARTH_GRAVITY / 5.0F) * forceComponent.weight;
	}
}

ska::GravitySystem::~GravitySystem()
{
}
