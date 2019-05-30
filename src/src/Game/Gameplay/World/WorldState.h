#pragma once
#include "Core/State/StateBase.h"
#include "Inputs/System/InputSystem.h"

#include "Graphic/System/CameraSystem.h"
#include "../Data/SavegameManager.h"
#include "../Mobs/MobSpawner.h"
#include "WorldImpl.h"
#include "Rectangle.h"
#include "../../Gameplay/Pokeball.h"
#include "Audio/Music.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"
#include "Graphic/System/JumpAnimationStateMachine.h"
#include "Physic/SpaceCollisionEventSender.h"
#include "Utils/SubObserver.h"
#include "Graphic/Polygon.h"
#include "ECS/EntityLocator.h"
#include "../../Debug/DebugDrawer.h"

namespace ska {
	namespace cp {
		class SpaceSystem;
	}
	class IniReader;
	class CameraSystem;
	class Window;
	class ShadowSystem;
	class GraphicSystem;
	class Ticked;
	class StateHolder;
	class Space;
}
class CustomEntityManager;

class WorldState : 
	public ska::StateBase,
	public MobSpawner,
	public ska::SubObserver<ska::GameEvent> {

public:
	WorldState(CustomEntityManager& data, PokemonGameEventDispatcher& pged, Settings& settings);
	WorldState(const WorldState&) = delete;
	WorldState& operator=(const WorldState&) = delete;

	~WorldState() override = default;

	int spawnMob(ska::Rectangle pos, unsigned int rmin, unsigned int rmax, unsigned int nbrSpawns, ska::IniReader* dataSpawn) override;

	const ska::EntityLocator& getEntityLocator() const;
	ska::TileWorld& getWorld();
	SavegameManager& getSaveGame();
	ska::cp::Space& getSpace();

	bool loadedOnce() const;
	std::vector<ska::IniReader>& getMobSettings() override;

	const std::string& getFileName() const;
	const std::string& getTilesetName() const;

	void reinit(const std::string& fileName, const std::string& chipsetName);

protected:
    virtual void afterLoad(ska::State* scene) override;
	virtual void beforeUnload() override;
	virtual void onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) override;
	virtual void onEventUpdate(unsigned int ellapsedTime) override;

private:
	bool onGameEvent(ska::GameEvent& ge);
	void beforeLoad(ska::State* lastState) override;

	bool m_loadedOnce = false;

	Settings& m_settings;

	SavegameManager m_saveManager;

	Pokeball m_pokeball;
	ska::Music m_worldBGM;

	ska::GraphicSystem* m_graphicSystem = nullptr;
	ska::ShadowSystem* m_shadowSystem = nullptr;
	
	PokemonGameEventDispatcher& m_eventDispatcher;
	CustomEntityManager& m_entityManager;
	ska::WalkAnimationStateMachine* m_walkASM = nullptr;
	
	ska::EntityLocator m_entityLocator;

	ska::TilesetCorrespondanceMapper m_correspondanceMapper;
	ska::TilesetPtr m_tileset;
	std::string m_worldFileName;
	
	WorldImpl m_world;

	ska::CameraSystem* m_cameraSystem{};
	ska::Rectangle m_screenSize{};
	bool m_firstState{};

	ska::cp::SpaceSystem* m_spaceSystem = nullptr;
	std::vector<ska::Polygon<int>> m_layerContours{};
	std::vector<ska::Polygon<int>> m_layerContoursWater{};
	std::unique_ptr<ska::cp::SpaceCollisionEventSender> m_collisionEventSender;
	
	ska::Polygon<int> m_posHeroPolygon;
	ska::Point<int> m_posHero;

	DebugDrawer m_debugDrawer;

};

