#pragma once
#include "../../ska/World/World.h"

#include "../../ska/Physic/System/CollisionSystem.h"
#include "../../ska/Physic/System\ForceSystem.h"
#include "../../ska/Physic/System\GravitySystem.h"
#include "../../ska/Physic/System\MovementSystem.h"
#include "../../ska/Graphic/System/GraphicSystem.h"
#include "../../ska/Graphic/System/ShadowSystem.h"
#include "../../ska/Graphic/System/DirectionalAnimationSystem.h"
#include "../../ska/Graphic/System/DeleterSystem.h"

class WorldImpl : public ska::World {
public:
	WorldImpl(const unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight);
	~WorldImpl();
	std::unordered_map<std::string, ska::EntityId> load(std::string fileName, std::string chipsetName, std::string saveName);
	virtual void refreshEntities() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;

private:

	ska::CollisionSystem m_collisionSystem;
	ska::ForceSystem m_forceSystem;
	ska::GravitySystem m_gravitySystem;
	ska::MovementSystem m_movementSystem;
	ska::DirectionalAnimationSystem m_daSystem;

	ska::GraphicSystem m_graphicSystem;
	ska::ShadowSystem m_shadowSystem;
	ska::DeleterSystem m_deleterSystem;
};