#pragma once
#include "Core/State/StateBase.h"
#include "Inputs/System/InputSystem.h"

#include "../Data/SavegameManager.h"
#include "../Mobs/MobSpawner.h"
#include "WorldImpl.h"
#include "Rectangle.h"
#include "../../Gameplay/Pokeball.h"
#include "Audio/Music.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"
#include "Graphic/System/JumpAnimationStateMachine.h"
#include "Utils/SubObserver.h"


namespace ska {
	class IniReader;
	class CameraSystem;
	class Window;
	class ShadowSystem;
	class GraphicSystem;
	class Ticked;
	class StateHolder;
}
class CustomEntityManager;

class WorldState : 
	public ska::StateBase,
	public MobSpawner,
	public ska::CameraAware,
	public ska::SubObserver<ska::GameEvent> {

public:
	WorldState(CustomEntityManager& data, PokemonGameEventDispatcher& pged, Settings& settings);
	WorldState(const WorldState&) = delete;
	WorldState& operator=(const WorldState&) = delete;

	virtual ~WorldState();

	int spawnMob(ska::Rectangle pos, unsigned int rmin, unsigned int rmax, unsigned int nbrSpawns, ska::IniReader* dataSpawn) override;
	std::unordered_map<std::string, ska::EntityId> reinit(const std::string& fileName, const std::string& chipsetName);
	void linkCamera(ska::CameraSystem* cs) override;

	ska::TileWorld& getWorld();
	SavegameManager& getSaveGame();
	ska::EntityId getPlayer() const;
	bool loadedOnce() const;
	std::vector<ska::IniReader>& getMobSettings() override;

	const std::string& getFileName() const;

protected:
    virtual void afterLoad(ska::State* scene) override;
	virtual void beforeUnload() override;
	virtual void onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) override;
	virtual void onEventUpdate(unsigned int ellapsedTime) override;

private:
	bool onGameEvent(ska::GameEvent& ge);
	void beforeLoad(ska::State* lastState) override;

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
	
	PokemonGameEventDispatcher& m_eventDispatcher;
	CustomEntityManager& m_entityManager;
	ska::WalkAnimationStateMachine* m_walkASM;
};

