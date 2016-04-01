#pragma once
#include "../../ska/Scene/Scene.h"
#include "../../ska/Physic/System/CollisionSystem.h"
#include "../../ska/Physic/System\ForceSystem.h"
#include "../../ska/Physic/System\GravitySystem.h"
#include "../../ska/Inputs/System/InputSystem.h"
#include "../../ska/Physic/System\MovementSystem.h"
#include "../../ska/Graphic/System/GraphicSystem.h"
#include "../../ska/Graphic/System/ShadowSystem.h"
#include "../../ska/Graphic/System/DirectionalAnimationSystem.h"
#include "../../ska/Graphic/System/DeleterSystem.h"

#include "../Data/SavegameManager.h"
#include "../Mobs/MobSpawner.h"
#include "WorldImpl.h"
#include "../../ska/Graphic/Rectangle.h"

namespace ska {
	class IniReader;
	class CameraSystem;
}
class CustomEntityManager;

class WorldScene : public ska::Scene, public MobSpawner, public ska::CameraAware {
public:
	WorldScene(CustomEntityManager& entityManager, ska::SceneHolder& sh, ska::InputContextManager& ril, const unsigned int screenW, const unsigned int screenH);
	virtual void load() override;
	virtual void unload() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual ~WorldScene();

	int spawnMob(ska::Rectangle pos, unsigned int rmin, unsigned int rmax, unsigned int nbrSpawns, ska::IniReader* dataSpawn);
	std::unordered_map<std::string, ska::EntityId> reinit(std::string fileName, std::string chipsetName);
	void linkCamera(ska::CameraSystem* cs);

	ska::World& getWorld();
	std::vector<ska::IniReader>& getMobSettings();
	CustomEntityManager& getEntityManager();
	SavegameManager& getSaveGame();
	ska::EntityId getPlayer();
	bool loadedOnce();

	const unsigned int getScreenW() const;
	const unsigned int getScreenH() const;

private:
	const unsigned int m_screenW;
	const unsigned int m_screenH;
	bool m_loadedOnce;
	ska::EntityId m_player;
	SavegameManager m_saveManager;

	CustomEntityManager& m_entityManager;

	/* Special system : camera (linked to World) */
	ska::CameraSystem* m_cameraSystem;
	WorldImpl m_world;

	/* Systems */
	/* Logics */
	ska::ForceSystem m_forceSystem;
	ska::GravitySystem m_gravitySystem;
	ska::MovementSystem m_movementSystem;
	ska::DirectionalAnimationSystem m_daSystem;
	ska::DeleterSystem m_deleterSystem;
	ska::CollisionSystem m_collisionSystem;
	ska::InputSystem m_inputSystem;

	/* Graphics */
	ska::GraphicSystem m_graphicSystem;
	ska::ShadowSystem m_shadowSystem;

};

