#pragma once
#include <fstream>
#include <memory>
#include <string>

namespace ska {
	class ScriptComponent;
	class ScriptAutoSystem;
	class Command
	{
	public:

		Command(){}
		virtual std::string process(ScriptAutoSystem& saveGame, ScriptComponent& script, std::stringstream& streamCmd) = 0;
		virtual ~Command(){}

	};
	typedef std::unique_ptr<Command> CommandPtr;
}