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
#include "../../ska/AI/System/IAMovementSystem.h"
#include "../Mobs/System/MobSpawningSystem.h"
#include "../CustomEntityManager.h"

namespace ska {
	class IniReader;
	class PrefabEntityManager;
}

class WorldImpl : public ska::World {
	
public:
	WorldImpl(ska::PrefabEntityManager& entityManager, unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight);
	~WorldImpl();
	void load(std::string fileName, std::string chipsetName, std::string saveName) override;
	void changeLevel(std::string fileName, std::string chipsetName) override;

	std::unordered_map<std::string, ska::EntityId> reinit(std::string fileName, std::string chipsetName);
	int spawnMob(ska::Rectangle pos, unsigned int rmin, unsigned int rmax, unsigned int nbrSpawns, ska::IniReader* dataSpawn);

	virtual void refreshEntities() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;

private:
	ska::EntityId m_player;
	
	ska::ForceSystem m_forceSystem;
	ska::GravitySystem m_gravitySystem;
	ska::MovementSystem m_movementSystem;
	ska::DirectionalAnimationSystem m_daSystem;

	ska::GraphicSystem m_graphicSystem;
	ska::ShadowSystem m_shadowSystem;
	ska::DeleterSystem m_deleterSystem;
	ska::IAMovementSystem m_iaMovementSystem;

	MobSpawningSystem m_mobSpawningSystem;
	ska::CollisionSystem m_collisionSystem;
	
};