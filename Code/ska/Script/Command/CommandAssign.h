#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class CommandAssign :
		public ska::AbstractFunctionCommand
	{
	public:
		CommandAssign();
		virtual ~CommandAssign();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber();
	};
}
