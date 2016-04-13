#pragma once
#include <memory>
#include "AbstractSceneMap.h"
#include "../../ska/Graphic/System/CameraFixedSystem.h"
#include "../../ska/Inputs/Readers/IniReader.h"
#include "../Data/PokemonDescriptor.h"
#include "../../ska/Inputs/InputComponent.h"
#include "../System/PokeballSystem.h"

class WorldScene;
class FightComponent;

class SceneFight :
	public AbstractSceneMap
{
public:
	SceneFight(ska::SceneHolder& sh, WorldScene& ws, ska::InputContextManager& ril, ska::Point<int> fightPos, FightComponent fc);
	virtual void load() override;
	virtual void unload() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
	virtual void eventUpdate(bool movingDisallowed) override;
	virtual ~SceneFight();
private:
	PokemonDescriptor m_descriptor;
	WorldScene& m_worldScene;
	ska::CameraFixedSystem m_cameraSystem;
	PokeballSystem m_pokeballSystem;
	const int m_scriptId;
	const unsigned int m_level;
	const ska::IniReader m_opponent;
	const ska::EntityId m_player;
	const ska::EntityId m_opponentId;
};
typedef std::unique_ptr<SceneFight> SceneFightPtr;
