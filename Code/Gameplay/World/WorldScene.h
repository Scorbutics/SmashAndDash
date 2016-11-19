#pragma once
#include "../../ska/Scene/Scene.h"
#include "../../ska/Physic/System/ForceSystem.h"
#include "../../ska/Physic/System/GravitySystem.h"
#include "../../ska/Inputs/System/InputSystem.h"
#include "../../ska/Physic/System/MovementSystem.h"
#include "../../Graphic/System/GraphicSystem.h"
#include "../../Graphic/System/ShadowSystem.h"
#include "../../ska/Graphic/System/DirectionalAnimationSystem.h"
#include "../../ska/Graphic/System/DeleterSystem.h"

#include "../Data/SavegameManager.h"
#include "../Mobs/MobSpawner.h"
#include "WorldImpl.h"
#include "../../ska/Graphic/Rectangle.h"
#include "../../Gameplay/Pokeball.h"
#include "../../Graphic/GUI/GUIMap.h"

namespace ska {
	class IniReader;
	class CameraSystem;
	class Window;
}
class CustomEntityManager;

class WorldScene : public ska::Scene, public MobSpawner, public ska::CameraAware {
public:
	WorldScene(CustomEntityManager& entityManager, ska::SceneHolder& sh, ska::InputContextManager& ril, ska::Window& w);
	virtual void load(ska::ScenePtr* scene) override;
	virtual bool unload() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
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

	const std::string getFileName() const;

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
	ska::InputSystem m_inputSystem;

	/* Graphics */
	GraphicSystem m_graphicSystem;
	ShadowSystem m_shadowSystem;

	Pokeball m_pokeball;

	GUIMap m_gui;

};

