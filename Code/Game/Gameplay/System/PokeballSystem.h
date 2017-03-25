#pragma once
#include "ECS/System.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "../PokeballComponent.h"
#include "Graphic/Texture.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "Graphic/GraphicComponent.h"

class PokeballSystem : public ska::System<std::unordered_set<ska::EntityId>, PokeballComponent, ska::PositionComponent>  {
public:
	explicit PokeballSystem(ska::EntityManager& entityManager);
	PokeballSystem& operator=(const PokeballSystem&) = delete;
	virtual ~PokeballSystem();

protected:
	virtual void refresh() override;
private:
	void throwBall(ska::EntityId entityId);
	ska::AnimatedTexture m_sprite;
	ska::AnimatedTexture m_openPokeball;
	ska::AnimatedTexture m_vortex;
};
