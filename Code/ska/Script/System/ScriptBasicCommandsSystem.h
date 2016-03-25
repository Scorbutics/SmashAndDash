#pragma once
#include "ScriptAutoSystem.h"

#include "../Command\CommandEnd.h"
#include "../Command\CommandCalculate.h"
#include "../Command\CommandEndScript.h"
#include "../Command\CommandAssign.h"
#include "../Command\CommandRandom.h"
#include "../Command\CommandWait.h"
#include "../Command/CommandLog.h"
#include "../Command/CommandScript.h"

namespace ska {

	class ScriptBasicCommandsSystem : public ScriptAutoSystem {
	public :
		ScriptBasicCommandsSystem(EntityManager& entityManager, ska::Savegame& saveGame) : ScriptAutoSystem(BasicScriptCommandHelper(), entityManager, saveGame) {

		}

		struct BasicScriptCommandHelper : public ScriptCommandHelper {
			virtual void setupCommands(std::unordered_map<std::string, CommandPtr>& c) const override {
				c[CommandEnd::getCmdName()] = std::move(ska::CommandPtr(new CommandEnd()));
				c["end_script"] = std::move(ska::CommandPtr(new CommandEndScript()));
				c["calculate"] = std::move(ska::CommandPtr(new CommandCalculate()));
				c["assign"] = std::move(ska::CommandPtr(new CommandAssign()));
				c["random"] = std::move(ska::CommandPtr(new CommandRandom()));
				c["log"] = std::move(ska::CommandPtr(new CommandLog()));
				c["script"] = std::move(ska::CommandPtr(new CommandScript()));
			}
		};
	};

}