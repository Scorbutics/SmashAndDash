#pragma once
#include <vector>
#include "CommandEnd.h"

namespace ska {
	class CommandEndScript :
		public CommandEnd
	{
	public:
		CommandEndScript(EntityManager& e);
		virtual ~CommandEndScript();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber();
	};

}