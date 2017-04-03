#pragma once
#include <memory>

#ifdef SKA_DEBUG_GRAPHIC
#define AbstractSceneMap_ DebugAbstractSceneMap
#include "DebugAbstractSceneMap.h"
#else
#define AbstractSceneMap_ AbstractSceneMap
#include "AbstractSceneMap.h"
#endif

#include "Graphic/System/CameraFixedSystem.h"
#include "Inputs/Readers/IniReader.h"
#include "../Data/PokemonDescriptor.h"
#include "../System/PokeballSystem.h"
#include "../../Gameplay/Fight/System/BattleSystem.h"
#include "../../Gameplay/Fight/System/SkillRefreshSystem.h"
#include "../../Physic/System/SkillEntityCollisionResponse.h"
#include "../../Physic/System/WorldEntityCollisionResponse.h"
#include "../../Gameplay/Fight/System/StatisticsSystem.h"
#include "../../Graphic/GUI/GUIBattle.h"
#include "AI/System/IARandomMovementSystem.h"

class WorldScene;
class FightComponent;
class SkillDescriptor;
class SkillsHolderComponent;

namespace ska {
	class Window;
	class TaskQueue;
}

class SceneFight :
	public AbstractSceneMap_ {
public:
	SceneFight(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::Scene& oldScene, WorldScene& ws, ska::Point<int> fightPos, FightComponent fc);
	SceneFight(const SceneFight&) = delete;
	SceneFight& operator=(const SceneFight&) = delete;

	virtual ska::CameraSystem& getCamera() override;
	virtual ~SceneFight();

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
typedef std::unique_ptr<SceneFight> SceneFightPtr;
