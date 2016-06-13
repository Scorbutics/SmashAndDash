#pragma once
#include <memory>
#include "AbstractSceneMap.h"
#include "../../ska/Graphic/System/CameraFixedSystem.h"
#include "../../ska/Inputs/Readers/IniReader.h"
#include "../Data/PokemonDescriptor.h"
#include "../../ska/Inputs/InputComponent.h"
#include "../System/PokeballSystem.h"
#include "../../Gameplay/Fight/System/BattleSystem.h"
#include "../../Gameplay/Fight/System/SkillRefreshSystem.h"
#include "../../Physic/System/SkillCollisionSystem.h"
#include "../../Gameplay/Fight/System/StatisticsSystem.h"

class WorldScene;
class FightComponent;
class SkillDescriptor;
class SkillsHolderComponent;

class SceneFight :
	public AbstractSceneMap
{
public:
	SceneFight(ska::SceneHolder& sh, WorldScene& ws, ska::InputContextManager& ril, ska::Point<int> fightPos, FightComponent fc);
	virtual void load(ska::ScenePtr* lastScene) override;
	virtual bool unload() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~SceneFight();
private:
	void createSkill(SkillDescriptor& sd, const std::string& skillPath);
	void loadSkills(const ska::IniReader& reader, const ska::EntityId m_pokemonId, SkillsHolderComponent& shc);

	ska::InputContextManager m_iaICM;
	PokemonDescriptor m_descriptor;
	WorldScene& m_worldScene;
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

	BattleSystem m_battleSystem;
	SkillRefreshSystem m_skillRefreshSystem;
	SkillCollisionSystem m_skillCollisionSystem;
};
typedef std::unique_ptr<SceneFight> SceneFightPtr;
