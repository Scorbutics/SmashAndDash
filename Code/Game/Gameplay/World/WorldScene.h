#pragma once
#include "Core/Scene/SceneBase.h"
#include "Physic/System/ForceSystem.h"
#include "Physic/System/GravitySystem.h"
#include "Inputs/System/InputSystem.h"
#include "Physic/System/MovementSystem.h"
#include "Graphic/System/GraphicSystem.h"
#include "Graphic/System/ShadowSystem.h"
#include "Graphic/System/DirectionalAnimationSystem.h"
#include "Graphic/System/DeleterSystem.h"

#include "../Data/SavegameManager.h"
#include "../Mobs/MobSpawner.h"
#include "WorldImpl.h"
#include "Rectangle.h"
#include "../../Gameplay/Pokeball.h"
#include "../../Graphic/GUI/GUIMap.h"
#include "Audio/Music.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"


namespace ska {
	class IniReader;
	class CameraSystem;
	class Window;
}
class CustomEntityManager;

class WorldScene : public ska::SceneBase<CustomEntityManager, PokemonGameEventDispatcher>,
	public MobSpawner,
	public ska::CameraAware {

public:
	WorldScene(CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::SceneHolder& sh, Settings& settings);
	WorldScene(const WorldScene&) = delete;
	WorldScene& operator=(const WorldScene&) = delete;

	virtual void load(ska::ScenePtr* scene) override;
	virtual bool unload() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(unsigned int ellapsedTime) override;
	virtual ~WorldScene();

	int spawnMob(ska::Rectangle pos, unsigned int rmin, unsigned int rmax, unsigned int nbrSpawns, ska::IniReader* dataSpawn) override;
	std::unordered_map<std::string, ska::EntityId> reinit(std::string fileName, std::string chipsetName);
	void linkCamera(ska::CameraSystem* cs) override;

	ska::World& getWorld();
	CustomEntityManager& getEntityManager() const;
	SavegameManager& getSaveGame();
	ska::EntityId getPlayer() const;
	bool loadedOnce();
	std::vector<ska::IniReader>& getMobSettings() override;
	unsigned int getScreenW() const;
	unsigned int getScreenH() const;

	const std::string& getFileName() const;

private:
	const unsigned int m_screenW;
	const unsigned int m_screenH;
	bool m_loadedOnce;

	Settings& m_settings;

	ska::EntityId m_player;
	SavegameManager m_saveManager;

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
	ska::GraphicSystem m_graphicSystem;
	ska::ShadowSystem m_shadowSystem;

	Pokeball m_pokeball;

	GUIMap m_gui;

	ska::Music m_worldBGM;

};

