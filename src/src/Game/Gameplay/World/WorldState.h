#pragma once
#include "../../Graphic/ScreenFade.h"
#include <memory>
#include <optional>
#include "Core/ECS/Basics/Script/ScriptRunner.h"
#include "Core/State/StateBase.h"
#include "Core/Inputs/System/InputSystem.h"

#include "Graphic/System/CameraSystem.h"
#include "../Data/SavegameManager.h"
#include "../Mobs/MobSpawner.h"
#include "WorldImpl.h"
#include "WorldStreamer.h"
#include "ChunkEntities.h"
#include "Core/Rectangle.h"
#include "Audio/Music.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"
#include "Graphic/System/JumpAnimationStateMachine.h"
#include "Physic/SpaceCollisionEventSender.h"
#include "Base/Patterns/SubObserver.h"
#include "Graphic/Polygon.h"
#include "Core/ECS/EntityLocator.h"
#include "Scripting/EntityHandles.h"
#include "Graphic/SDLSurface.h"
#include "../Data/Bag.h"

namespace ska {
	namespace scripting {
		class ScriptVM;
		class EngineModule;
	}
}
struct GameScriptModules;
class BattleRules;
class BattleStage;

// End of a battle
struct BattleResult {
	bool won = false;
	// The player left the arena (neither won nor lost)
	bool fled = false;
	// The Pokemon of the player : its index in the team (-1 : not from the team), its hit points left
	int teamIndex = -1;
	int hpLeft = 0;
	// Experience won
	int experience = 0;
};
#include "Game/Graphic/DebugDrawer.h"

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
struct ArrivalComponent;

class WorldState : 
	public ska::StateBase,
	public MobSpawner,
	public ska::SubObserver<ska::GameEvent> {

public:
	WorldState(CustomEntityManager& data, PokemonGameEventDispatcher& pged, Settings& settings);
	WorldState(const WorldState&) = delete;
	WorldState& operator=(const WorldState&) = delete;

	~WorldState() override;

	int spawnMob(ska::Rectangle pos, unsigned int rmin, unsigned int rmax, unsigned int nbrSpawns, ska::IniReader* dataSpawn) override;
	// A wild Pokemon on a block of the map, with its attitude ("aggressive", "fearful", "passive"), its walk (pathType 1 :
	// it wanders), how long it stays before it flies away (a bird)
	ska::EntityId createWild(int monster, int level, int pathType, const std::string& attitude, ska::Point<int> block, std::optional<int> stay);
	// A wild Pokemon arrives near the player at the next frame, as the rules want it (flying in, out of a hideout, walking
	// in) : the scripts (Battle.spawnWild ; the rules are scripts, not called while a script runs). False : no player
	bool spawnWild(int monster, int level);
	// The verbs of the arrivals (Data/Battle/arrivals.miniska ; Battle.land, Battle.emerge, Battle.walkIn). False : no player.
	// A bird flies in from out of the screen (beyond the block, seen from the player) and lands on the block ; it flies away
	// after "stay" ms (0 : it stays)
	bool land(int monster, int level, const std::string& attitude, ska::Point<int> block, int stay);
	// The hideout rustles "rustle" ms, then the Pokemon hops out of it onto the block. "effect" : what the hideout throws
	// while it rustles ("leaves", "dust" : a rock, "splash" : the water)
	bool emerge(int monster, int level, const std::string& attitude, ska::Point<int> hideout, ska::Point<int> block, int rustle, const std::string& effect);
	// The leaves of a tree rustle "rustle" ms, then the Pokemon comes down from them on a silk thread onto the block
	// (under the tree)
	bool drop(int monster, int level, const std::string& attitude, ska::Point<int> leaves, ska::Point<int> block, int rustle);
	// It walks in from out of the screen (no room for it : false)
	bool walkIn(int monster, int level, const std::string& attitude);
	// The script of the arrivals that do not walk in : the spawner starts it
	static constexpr auto ArrivalsFile = "./Data/Battle/arrivals.miniska";
	// An arrival is over (ArrivalSystem) : the wild Pokemon on its block
	void wildArrived(const ArrivalComponent& arrival);
	// A wild bird flies away (its stay is over, a fearful one sees the player) : a drawing flies off, the wild Pokemon goes
	void takeOff(ska::EntityId wild);
	// The birds fly in from this far (blocks) and this high (pixels : over the highest buildings, the air of the drawing)
	static constexpr float FlightDistance = 13.F;
	static constexpr int FlightHeight = 280;

	const ska::EntityLocator& getEntityLocator() const;
	// The entities given to the scripts (handles)
	const ska::scripting::EntityHandles& getEntityHandles() const { return m_entityHandles; }
	ska::TileWorld& getWorld();
	SavegameManager& getSaveGame();
	// Items and money of the player
	Bag& getBag() { return m_bag; }
	ska::cp::Space& getSpace();
	// Runs the skalang scripts started by the script triggers
	ska::ScriptRunner& getScriptRunner();
	ska::scripting::ScriptVM& getScripts();
	// Camera of the map
	ska::CameraSystem& getCamera() { return *m_cameraSystem; }
	// Zoom of the camera on the map : close to the player, as the classic games (about 17 x 10 blocks). 4 / 3 : a pixel of
	// the 16 px tiles (enlarged 3 times in the 48 px chipsets) is 4 pixels of the screen, all of the same size
	static constexpr float MapZoom = 4.F / 3.F;
	// The fade of the map changes
	ScreenFade& getFade() { return m_fade; }
	// Battle rules and AI (scripts)
	BattleRules& getBattleRules();
	// The level of the first Pokemon of the team able to fight (1 : none)
	int teamLevel();
	// The battle going on (StateFight), else nullptr : the scripts act on it (Battle.announce)
	void setBattleStage(BattleStage* stage) { m_battleStage = stage; }
	BattleStage* battleStage() { return m_battleStage; }

	// End of a battle : its result is given to the scripts waiting for it (Battle.start), and to the hooks
	// ("battle.won", "battle.lost", with the player)
	void battleEnded(const BattleResult& result);
	// Starts the hooks of an engine event about the player (nothing when there is no player)
	void emitPlayerHook(const std::string& event);
	unsigned int battles() const { return m_battles; }
	bool lastBattleWon() const { return m_lastBattleWon; }
	bool lastBattleFled() const { return m_lastBattleFled; }

	bool loadedOnce() const;
	std::vector<ska::IniReader>& getMobSettings() override;

	// Name of the current map (the chunk of the player outdoors, ex : "w_5_4")
	std::string getMapName() const;
	// The tiles of the chunk of a block (their texture, their clips : the battleground of a clash is drawn with them)
	const ska::Tileset& getTileset(ska::Point<int> block) const { return m_streamer->tilesetAt(block); }
	// The block of the world of the first block of a map of the world the player is in ((0, 0) for another map)
	ska::Point<int> originOf(const std::string& map) const { return m_streamer->originOf(map); }
	// A step of the chunks around the player (StateMap) : the ones to come, the ones to leave
	void updateStreaming(unsigned int ellapsedTime);
	// Saves the game : the player (its map, its block in it), the variables, the entities of the chunks
	void saveGame();

	// A character of the map (Character.addCharacter) : it belongs to its chunk (ChunkEntities)
	ska::EntityId addCharacter(int sprite, ska::Point<int> block, const std::string& name, const std::string& script, std::vector<std::string> arguments);
	// Nothing pushes it any more (Character.anchor)
	void anchor(ska::EntityId entity);
	// An entity leaves the map, its physics bodies freed
	void removeCharacter(ska::EntityId entity);
	// Its physics bodies leave the map (freed at a next step, once no system has it) : for an entity about to be removed
	void releaseBodies(ska::EntityId entity);
	// The music of the map (a clash plays its own one, then this one again)
	ska::Music& getWorldMusic() { return m_worldBGM; }

	bool spawnWildNow(int monster, int level);
	std::vector<std::pair<int, int>> m_wildToSpawn;
	// One wild Pokemon near a point (the player) : as its arrival wants it (the script of the arrivals) ; walking in, between
	// rmin and rmax pixels away
	bool spawnOne(int idMob, int level, int pathType, ska::Point<int> near, float angle, unsigned int rmin, unsigned int rmax, const std::vector<unsigned int>& idBlocks);
	bool walkInAt(int idMob, int level, int pathType, const std::string& attitude, ska::Point<int> near, float angle, unsigned int rmin, unsigned int rmax, const std::vector<unsigned int>& idBlocks);
	std::optional<ska::Point<float>> playerPosition();
	// A block a wild Pokemon can stand on : on the map, no wall, no water, nobody there
	bool freeBlock(int x, int y);
	// The drawing of a wild Pokemon arriving or leaving (no physics body)
	ska::EntityId arrivalDrawing(int monster, ArrivalComponent arrival);
	// Another world (the outdoors from a door, a room) : the map, the block of the player in it (none : the one of the
	// save). The block of the player in the world is given
	ska::Point<int> reinit(const std::string& map, std::optional<ska::Point<int>> localBlock);

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
	Bag m_bag;

	ska::Music m_worldBGM;

	ska::GraphicSystem* m_graphicSystem = nullptr;
	ska::ShadowSystem* m_shadowSystem = nullptr;
	
	PokemonGameEventDispatcher& m_eventDispatcher;
	CustomEntityManager& m_entityManager;
	ska::WalkAnimationStateMachine* m_walkASM = nullptr;
	
	ska::EntityLocator m_entityLocator;
	ska::scripting::EntityHandles m_entityHandles;

	ska::TilesetCorrespondanceMapper m_correspondanceMapper;
	ScreenFade m_fade;
	// The map the world was entered by (then the map of the player : getMapName)
	std::string m_mapName;
	
	WorldImpl m_world;
	std::unique_ptr<ChunkEntities> m_chunkEntities;
	std::unique_ptr<WorldStreamer> m_streamer;
	// The records of the entities were read from the save
	bool m_recordsLoaded = false;
	// The bodies to free : queued at this step, at the step before
	std::vector<std::size_t> m_released;
	std::vector<std::size_t> m_releasing;

	ska::CameraSystem* m_cameraSystem{};
	ska::Rectangle m_screenSize{};
	bool m_firstState{};

	ska::cp::SpaceSystem* m_spaceSystem = nullptr;
	std::unique_ptr<ska::cp::SpaceCollisionEventSender> m_collisionEventSender;
	
	ska::Polygon<int> m_posHeroPolygon;
	ska::Point<int> m_posHero;

	DebugDrawer m_debugDrawer;

	// skalang scripts (".miniska") : script VM and the functions given to the scripts
	std::unique_ptr<ska::scripting::ScriptVM> m_scripts;
	std::unique_ptr<ska::scripting::EngineModule> m_engineScriptModule;
	std::unique_ptr<GameScriptModules> m_gameScriptModules;
	std::unique_ptr<BattleRules> m_battleRules;
	unsigned int m_battles = 0;
	bool m_lastBattleWon = false;
	bool m_lastBattleFled = false;

	BattleStage* m_battleStage = nullptr;
};

