#pragma once
#include <memory>

#ifdef SKA_DEBUG_GRAPHIC
#define AbstractSceneMap_ DebugAbstractSceneMap
#include "DebugAbstractSceneMap.h"
#else
#define AbstractSceneMap_ AbstractSceneMap
#include "AbstractSceneMap.h"
#endif

#include "../../ska/Graphic/System/CameraFixedSystem.h"
#include "../../ska/Inputs/Readers/IniReader.h"
#include "../Data/PokemonDescriptor.h"
#include "../../ska/Inputs/InputComponent.h"
#include "../System/PokeballSystem.h"
#include "../../Gameplay/Fight/System/BattleSystem.h"
#include "../../Gameplay/Fight/System/SkillRefreshSystem.h"
#include "../../Physic/System/SkillEntityCollisionResponse.h"
#include "../../Physic/System/WorldEntityCollisionResponse.h"
#include "../../Gameplay/Fight/System/StatisticsSystem.h"
#include "../../Graphic/GUI/GUIBattle.h"

class WorldScene;
class FightComponent;
class SkillDescriptor;
class SkillsHolderComponent;

using BattleStartObservable = ska::Observable<ska::CameraSystem&, const ska::EntityId&, const ska::EntityId&, ska::EntityManager&>;

namespace ska {
	class Window;
	class TaskQueue;
}

class SceneFight :
	public AbstractSceneMap_, public BattleStartObservable {
public:
	SceneFight(ska::Window& w, ska::SceneHolder& sh, WorldScene& ws, ska::InputContextManager& ril, ska::Point<int> fightPos, FightComponent fc);
	virtual void load(ska::ScenePtr* lastScene) override;
	virtual bool unload() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ska::CameraSystem& getCamera() override;
	virtual ~SceneFight();
private:
	void createSkill(SkillDescriptor& sd, const std::string& skillPath);
	void loadSkills(const ska::IniReader& reader, const ska::EntityId m_pokemonId, SkillsHolderComponent& shc);

	ska::InputContextManager m_iaICM;
	PokemonDescriptor m_descriptor;
	
	ska::CameraFixedSystem m_cameraSystem;
	PokeballSystem m_pokeballSystem;
	StatisticsSystem m_statsSystem;

	const int m_opponentScriptId;
	const unsigned int m_level;
	const ska::IniReader m_opponent;
	const ska::IniReader m_pokemon;
	const ska::EntityId m_pokemonId;
	const ska::EntityId m_trainerId;
	const ska::EntityId m_opponentId;

	bool m_sceneLoaded;
	int m_loadState;

	BattleSystem m_battleSystem;
	SkillRefreshSystem m_skillRefreshSystem;

	WorldEntityCollisionResponse m_worldEntityCollisionResponse;
	SkillEntityCollisionResponse m_skillEntityCollisionResponse;

	/* TODO GUI Battle specific part with Bars and also skills displayed */
	GUIBattle m_guiBattle;

	ska::TaskQueue& m_taskQueue;
};
typedef std::unique_ptr<SceneFight> SceneFightPtr;
