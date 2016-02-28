#pragma once
#include <fstream>
#include <memory>
#include <string>

namespace ska {
	class IScript;
	class Command
	{
	public:

		Command(){}
		virtual std::string process(ska::IScript* script, std::stringstream& streamCmd) = 0;
		virtual ~Command(){}

	};
	typedef std::unique_ptr<Command> CommandPtr;
}