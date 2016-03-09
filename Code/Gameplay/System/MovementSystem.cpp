#include "MovementSystem.h"

MovementSystem::MovementSystem(ska::EntityManager& entityManager) : System(entityManager) {
}

void MovementSystem::refresh(ska::EntityId& entityId) {
	PositionComponent& posComponent = m_entityManager.getComponent<PositionComponent>(entityId);
	MovementComponent& moveComponent = m_entityManager.getComponent<MovementComponent>(entityId);


	//on termine le PFD selon x et y avec la force de frottements fluides
	//( en comptant qu'au préalable on a peut-être déjà appliqué une ou plusieurs force(s) avec "applyForce" )
	/*moveComponent.ax += -m_frictionCoeff * m_speedx / m_weight;
	moveComponent.ay += -m_frictionCoeff * m_speedy / m_weight;

	//EARTH_GRAVITY était trop élevée alors j'ai préféré la diviser par 5
	moveComponent.az -= (float)EARTH_GRAVITY / 5;*/

	//(v(t) - v(t-1))/(t - (t-1)) = dv/dt (t) = a(t)
	moveComponent.vx += moveComponent.ax;
	moveComponent.vy += moveComponent.ay;
	moveComponent.vz += moveComponent.az;

	moveComponent.ax = 0;
	moveComponent.ay = 0;
	moveComponent.az = 0;
	
	//(x(t) - x(t-1))/(t - (t-1)) = dx/dt (t) = vx(t)
	posComponent.x += moveComponent.vx;
	posComponent.y += moveComponent.vy;
	posComponent.z += moveComponent.vz;
	
}

MovementSystem::~MovementSystem()
{
}
