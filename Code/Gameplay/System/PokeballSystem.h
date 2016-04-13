#pragma once
#include "../../ska/ECS/System.h"
#include "../../ska/Physic/ForceComponent.h"
#include "../PokeballComponent.h"
#include "../../ska/Graphic/Texture.h"
#include "../../ska/Physic/PositionComponent.h"
#include "../../ska/Graphic/GraphicComponent.h"

class PokeballSystem : public ska::System<std::unordered_set<ska::EntityId>, PokeballComponent, ska::PositionComponent>  {
public:
	PokeballSystem(ska::EntityManager& entityManager);
	virtual ~PokeballSystem();

	virtual void refresh() override;
private:
	void throwBall(ska::EntityId entityId);
	ska::AnimatedTexture m_sprite;
	ska::AnimatedTexture m_openPokeball;
	ska::AnimatedTexture m_vortex;
};
