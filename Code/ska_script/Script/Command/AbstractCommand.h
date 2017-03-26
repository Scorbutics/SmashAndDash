#pragma once
#include <sstream>
#include <vector>
#include "Command.h"


namespace ska {
	class AbstractCommand : public Command {
	private:
		std::string interpretSubCommands(ScriptAutoSystem& saveGame, std::string& line, ScriptComponent& script);

	public:
		explicit AbstractCommand(EntityManager& entityManager);
		void operator=(const AbstractCommand&) = delete;
		std::string process(ScriptAutoSystem& saveGame, ScriptComponent& script, std::stringstream& streamCmd) override;
		virtual ~AbstractCommand();

	protected:
		virtual std::string process(ScriptComponent& script, std::stringstream& streamCmd, std::vector<std::string>& args) = 0;
		virtual char getSeparator() = 0;
	};
}
