#pragma once
#include <memory>

#ifdef SKA_DEBUG_GRAPHIC
#define AbstractSceneMap_ DebugAbstractStateMap
#include "DebugAbstractStateMap.h"
#else
#define AbstractStateMap_ AbstractStateMap
#include "AbstractStateMap.h"
#endif

#include "Graphic/System/CameraFixedSystem.h"
#include "Inputs/Readers/IniReader.h"
#include "../Data/PokemonDescriptor.h"
#include "../System/PokeballSystem.h"
#include "../../Gameplay/Fight/System/BattleSystem.h"
#include "../../Physic/System/SkillEntityCollisionResponse.h"
#include "../../Physic/System/WorldEntityCollisionResponse.h"
#include "../../Gameplay/Fight/System/StatisticsSystem.h"
#include "../../Graphic/GUI/GUIBattle.h"

class WorldState;
class FightComponent;
class SkillDescriptor;
class SkillsHolderComponent;

namespace ska {
	class Window;
	class TaskQueue;
}

class StateFight :
	public AbstractStateMap_ {
public:
	StateFight(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::State& oldScene, WorldState& ws, ska::Point<int> fightPos, FightComponent fc);
	StateFight(const StateFight&) = delete;
	StateFight& operator=(const StateFight&) = delete;

	virtual ska::CameraSystem& getCamera() override;
	virtual ~StateFight();

protected:
    virtual void beforeLoad(ska::ScenePtr* lastScene) override;
	virtual bool beforeUnload() override;

private:
	void createSkill(SkillDescriptor& sd, const std::string& skillPath) const;
	void loadSkills(const ska::IniReader& reader, const ska::EntityId m_pokemonId, SkillsHolderComponent& shc) const;

	ska::InputContextManager m_iaICM;
	PokemonDescriptor m_descriptor;

	ska::CameraFixedSystem *m_cameraSystem;

	const int m_opponentScriptId;
	const unsigned int m_level;
	const ska::IniReader m_opponent;
	const ska::IniReader m_pokemon;
	const ska::EntityId m_pokemonId;
	const ska::EntityId m_trainerId;
	const ska::EntityId m_opponentId;

	bool m_sceneLoaded;
	int m_loadState;

	WorldEntityCollisionResponse m_worldEntityCollisionResponse;
	SkillEntityCollisionResponse m_skillEntityCollisionResponse;

};
typedef std::unique_ptr<StateFight> SceneFightPtr;
