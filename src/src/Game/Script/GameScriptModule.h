#pragma once
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

// Game functions for the scripts : Game = import "bind:game" (see the "game" script template)
class GameScriptModule : public ska::lang::Module<ska::bytecode::Interpreter> {
public:
	// "worldState" : current map (world, physics space), when the functions are called
	GameScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, WorldState& worldState);
	~GameScriptModule() override = default;

private:
	ska::BridgeBuilder<ska::bytecode::Interpreter> m_builder;
};
