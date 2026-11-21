#pragma once
#include <memory>

#include "Graphic/System/CameraSystem.h"
#include "Base/IO/Readers/IniReader.h"
#include "../Data/PokemonDescriptor.h"
#include "../System/PokeballSystem.h"
#include "../../Physic/System/SkillEntityCollisionResponse.h"
#include "../Fight/BattleStage.h"
#include "../../Graphic/GUI/GUIBattle.h"
#include "../Fight/SkillFactory.h"
#include "StateFightLoading.h"
#include "Core/State/StateBase.h"
#include "Core/ECS/Basics/Input/InputComponent.h"

class WorldState;
class FightComponent;
class SkillDescriptor;
class SkillsHolderComponent;

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

	~StateFight() override = default;

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
	ska::EntityId m_pokeball;

	SkillFactory m_skillFactory;
	StateFightLoading m_loader;
	ska::Point<int> m_fightPos;
	ska::Point<int> m_screenSize;
	// State of the battle, shared by its systems and its interface
	BattleStage m_stage;


};
