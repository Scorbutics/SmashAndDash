#include "GravitySystem.h"
#include "Utils/SkaConstants.h"

ska::GravitySystem::GravitySystem(EntityManager& entityManager) : System(entityManager) {
}

void ska::GravitySystem::refresh() {
	for (auto entityId : m_processed) {
		auto& forceComponent = m_entityManager.getComponent<ForceComponent>(entityId);
		auto& gaComponent = m_entityManager.getComponent<GravityAffectedComponent>(entityId);
		auto& moveComponent = m_entityManager.getComponent<MovementComponent>(entityId);

		//PFD selon x et y avec la force de frottements fluides
		//( en comptant qu'au préalable on a peut-être déjà appliqué une ou plusieurs force(s) )
		forceComponent.x -= gaComponent.friction * moveComponent.vx;
		forceComponent.y -= gaComponent.friction * moveComponent.vy;

		//EARTH_GRAVITY était trop élevée alors j'ai préféré la diviser par 5
		forceComponent.z -= (EARTH_GRAVITY/5.0F) * forceComponent.weight;
	}
}

ska::GravitySystem::~GravitySystem()
{
}
