#include "GravitySystem.h"
#include "../../Utils/SkaConstants.h"

ska::GravitySystem::GravitySystem(ska::EntityManager& entityManager) : System(entityManager) {
}

void ska::GravitySystem::refresh() {
	for (ska::EntityId entityId : m_processed) {
		ForceComponent& forceComponent = m_entityManager.getComponent<ForceComponent>(entityId);
		GravityAffectedComponent& gaComponent = m_entityManager.getComponent<GravityAffectedComponent>(entityId);
		MovementComponent& moveComponent = m_entityManager.getComponent<MovementComponent>(entityId);

		//on termine le PFD selon x et y avec la force de frottements fluides
		//( en comptant qu'au pr�alable on a peut-�tre d�j� appliqu� une ou plusieurs force(s) avec "applyForce" )
		forceComponent.x += -gaComponent.friction * moveComponent.vx / gaComponent.weight;
		forceComponent.y += -gaComponent.friction * moveComponent.vy / gaComponent.weight;

		//EARTH_GRAVITY �tait trop �lev�e alors j'ai pr�f�r� la diviser par 5
		forceComponent.z += (float)EARTH_GRAVITY / 5;
	}
}

ska::GravitySystem::~GravitySystem()
{
}
