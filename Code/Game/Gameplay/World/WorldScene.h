#pragma once
#include "Core/Scene/SceneBase.h"
#include "Inputs/System/InputSystem.h"

#include "../Data/SavegameManager.h"
#include "../Mobs/MobSpawner.h"
#include "WorldImpl.h"
#include "Rectangle.h"
#include "../../Gameplay/Pokeball.h"
#include "Audio/Music.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"


namespace ska {
	class IniReader;
	class CameraSystem;
	class Window;
	class ShadowSystem;
	class GraphicSystem;
	class Ticked;
}
class CustomEntityManager;

class WorldScene : public ska::SceneBase<CustomEntityManager, PokemonGameEventDispatcher>,
	public MobSpawner,
	public ska::CameraAware {

public:
	WorldScene(CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::SceneHolder& sh, ska::Ticked& ticked, Settings& settings);
	WorldScene(const WorldScene&) = delete;
	WorldScene& operator=(const WorldScene&) = delete;

	virtual ~WorldScene();

	int spawnMob(ska::Rectangle pos, unsigned int rmin, unsigned int rmax, unsigned int nbrSpawns, ska::IniReader* dataSpawn) override;
	std::unordered_map<std::string, ska::EntityId> reinit(const std::string& fileName, const std::string& chipsetName);
	void linkCamera(ska::CameraSystem* cs) override;

	ska::World& getWorld();
	SavegameManager& getSaveGame();
	ska::EntityId getPlayer() const;
	bool loadedOnce() const;
	std::vector<ska::IniReader>& getMobSettings() override;

	const std::string& getFileName() const;

protected:
    virtual void afterLoad(ska::ScenePtr* scene) override;
	virtual bool beforeUnload() override;
	virtual void onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) override;
	virtual void onEventUpdate(unsigned int ellapsedTime) override;

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
	Pokeball m_pokeball;
	ska::Music m_worldBGM;

	ska::GraphicSystem* m_graphicSystem;
	ska::ShadowSystem* m_shadowSystem;
};

