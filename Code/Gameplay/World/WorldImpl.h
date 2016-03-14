#pragma once
#include "../../ska/World/World.h"

#include "../System/CollisionSystem.h"
#include "../System\ForceSystem.h"
#include "../System\GravitySystem.h"
#include "../System\MovementSystem.h"
#include "../../Graphic/System/GraphicSystem.h"

class WorldImpl : public ska::World {
public:
	WorldImpl(const unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight);
	~WorldImpl();
	virtual void load(std::string fileName, std::string chipsetName, std::string saveName) override;
	virtual void refreshEntities() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;

private:

	CollisionSystem m_collisionSystem;
	ForceSystem m_forceSystem;
	GravitySystem m_gravitySystem;
	MovementSystem m_movementSystem;

	GraphicSystem m_graphicSystem;
};