#pragma once
#include "BytecodeInterpreter/BytecodeScript.h"
#include "BytecodeInterpreter/Value/BytecodeInterpreterTypes.h"
#include "std/module.h"
#include "Runtime/Service/BridgeBuilder.h"
#include "BytecodeInterpreter/BytecodeInterpreter.h"

#include "../Gameplay/PokemonGameEventDispatcher.h"

class CustomEntityManager;

namespace ska {
	class TileWorld;
	namespace cp {
		class Space;
	}
	namespace scripting {
		class ScriptVM;
	}
}

// Game functions for the scripts : Game = import "bind:game" (see the "game" script template)
class GameScriptModule : public ska::lang::Module<ska::bytecode::Interpreter> {
public:
	GameScriptModule(ska::scripting::ScriptVM& vm, CustomEntityManager& entityManager, PokemonGameEventDispatcher& ged, const ska::TileWorld& world, ska::cp::Space& space);
	~GameScriptModule() override = default;

private:
	ska::BridgeBuilder<ska::bytecode::Interpreter> m_builder;
};
