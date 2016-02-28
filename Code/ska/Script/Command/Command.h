#pragma once
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include "../IScript.h"

namespace ska {
	class Command
	{
	public:

		Command(){}
		virtual std::string process(ska::IScript* script, std::stringstream& streamCmd, std::ofstream& scriptList) = 0;
		virtual ~Command(){}

	};
	typedef std::unique_ptr<Command> CommandPtr;
}