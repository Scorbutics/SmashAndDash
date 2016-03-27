#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class CommandLog :
		public ska::AbstractFunctionCommand
	{
	public:
		CommandLog(EntityManager& e);
		virtual ~CommandLog();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber();
	};
}
