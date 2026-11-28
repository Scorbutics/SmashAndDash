#pragma once
#include <memory>

#include "Graphic/System/CameraSystem.h"
#include "Base/IO/Readers/IniReader.h"
#include "../Data/PokemonDescriptor.h"
#include "../System/PokeballSystem.h"
#include "../../Physic/System/SkillEntityCollisionResponse.h"
#include "../Fight/BattleStage.h"
#include "../Fight/BattleSounds.h"
#include "../../Graphic/GUI/GUIBattle.h"
#include "../Fight/SkillFactory.h"
#include "StateFightLoading.h"
#include "Core/State/StateBase.h"
#include "Core/ECS/Basics/Input/InputComponent.h"

class WorldState;
class FightComponent;
class SkillDescriptor;
class SkillsHolderComponent;
class ClashSystem;

namespace ska {
	class Window;
	class TaskQueue;
}

class StateFight :
	public ska::StateBase {
public:
	StateFight(CustomEntityManager& em, PokemonGameEventDispatcher& ged, WorldState& ws, ska::Point<int> fightPos, FightComponent fc);
	StateFight(const StateFight&) = delete;
	StateFight& operator=(const StateFight&) = delete;

	~StateFight() override;

	// The room between the trainer and the wild Pokemon when the battle starts (blocks) : the wild Pokemon touched jumps
	// back
	static constexpr int IntroGap = 3;

protected:
	virtual void beforeLoad(ska::State* lastScene) override;
	virtual void beforeUnload() override;

private:
	bool onGameEvent(ska::GameEvent& ge);

	PokemonGameEventDispatcher& m_eventDispatcher;
	CustomEntityManager& m_entityManager;
	
	WorldState& m_worldState;
	PokemonDescriptor m_descriptor;

	ska::CameraSystem* m_cameraSystem;
	// Owned by the logics of the state
	ClashSystem* m_clash = nullptr;

	const int m_opponentScriptId;
	const int m_pokemonScriptId;
	const int m_opponentLevel;
	const int m_pokemonExperience;
	const int m_pokemonHp;
	const int m_pokemonTeamIndex;
	const ska::Point<int> m_pokemonPosition;
	const ska::IniReader m_opponent;
	const ska::IniReader m_pokemon;
	const ska::EntityId m_pokemonId;
	const ska::EntityId m_trainerId;
	const ska::EntityId m_opponentId;

	bool m_sceneLoaded;
	int m_loadState;

	ska::InputComponent* m_ic;
	ska::InputComponent m_input;
	// Where the pokeball of the intro opened (world pixels)
	ska::Point<float> m_landing;

	SkillFactory m_skillFactory;
	StateFightLoading m_loader;
	ska::Point<int> m_fightPos;
	ska::Point<int> m_screenSize;
	// State of the battle, shared by its systems and its interface
	BattleStage m_stage;
	BattleSounds m_sounds;
	// The other wild Pokemon of the battle (a group) : created for the battle, removed after it
	const std::vector<std::pair<int, unsigned int>> m_companionsData;
	std::vector<ska::EntityId> m_companions;
	const bool m_canFlee;
	void createCompanions(BattleRules& rules);
	// The trainer does not block the fighters during the battle (its physics shape becomes a sensor)
	void trainerBlocks(bool blocks);
	// The intro : the points of the throw of the pokeball, from the trainer to the block next to the wild Pokemon
	std::pair<ska::Point<float>, ska::Point<float>> throwPath();
	// An entity removed after the battle : its physics body goes away first (the space keeps the bodies of the removed
	// entities, and the map is not loaded again after a battle)
	void park(ska::EntityId entity);
	// The block where the Pokemon of the player lands : next to the wild Pokemon, on the side of the trainer
	ska::Point<int> landingBlock();


};
