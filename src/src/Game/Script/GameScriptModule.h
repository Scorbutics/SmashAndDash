#pragma once
#include <string>
#include "BytecodeInterpreter/BytecodeScript.h"
#include "BytecodeInterpreter/Value/BytecodeInterpreterTypes.h"
#include "std/module.h"
#include "Runtime/Service/BridgeBuilder.h"
#include "BytecodeInterpreter/BytecodeInterpreter.h"

#include "../Gameplay/PokemonGameEventDispatcher.h"

class CustomEntityManager;
class WorldState;

namespace ska {
	namespace scripting {
		class ScriptVM;
	}
}

// Game functions for the scripts, by domain (ex : Character = import "bind:game.character", see the "game.<domain>" script
// templates)
class GameScriptModule : public ska::lang::Module<ska::bytecode::Interpreter> {
protected:
	GameScriptModule(ska::scripting::ScriptVM& vm, const std::string& name);
	~GameScriptModule() override = default;
	ska::BridgeBuilder<ska::bytecode::Interpreter> m_builder;
};

// Dialogs with the player : GameDialog = import "bind:game.dialog"
class DialogScriptModule : public GameScriptModule {
public:
	DialogScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState);
};

// Characters (entities) : movements, look, position : Character = import "bind:game.character"
class CharacterScriptModule : public GameScriptModule {
public:
	CharacterScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState);
};

// Maps : teleportation, blocks, map effects : Map = import "bind:game.map"
class MapScriptModule : public GameScriptModule {
public:
	MapScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState);
};

// Pokemon team of the player : Party = import "bind:game.party"
class PartyScriptModule : public GameScriptModule {
public:
	PartyScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState);
};

// Game interface : Interface = import "bind:game.interface"
class InterfaceScriptModule : public GameScriptModule {
public:
	InterfaceScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState);
};

// Battles : Battle = import "bind:game.battle"
class BattleScriptModule : public GameScriptModule {
public:
	BattleScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState);
};

// All the game functions given to the scripts.
// "worldState" : current map (world, physics space), when the functions are called
struct GameScriptModules {
	GameScriptModules(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState);

	DialogScriptModule dialog;
	CharacterScriptModule character;
	MapScriptModule map;
	PartyScriptModule party;
	InterfaceScriptModule interface;
	BattleScriptModule battle;
};
