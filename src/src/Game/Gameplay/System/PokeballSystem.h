#pragma once
#include "ECS/System.h"
#include "../PokeballComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "Graphic/GraphicComponent.h"

class PokeballSystem : public ska::System< ska::RequiredComponent<PokeballComponent, ska::PositionComponent, ska::GraphicComponent>, ska::PossibleComponent<>>  {
public:
	explicit PokeballSystem(ska::EntityManager& entityManager);
	PokeballSystem& operator=(const PokeballSystem&) = delete;
	~PokeballSystem() override = default;

protected:
	virtual void refresh(unsigned int ellapsedTime) override;
private:
	void throwBall(ska::EntityId entityId);
	ska::AnimatedTexture m_sprite;
	ska::AnimatedTexture m_openPokeball;
	ska::AnimatedTexture m_vortex;
};
