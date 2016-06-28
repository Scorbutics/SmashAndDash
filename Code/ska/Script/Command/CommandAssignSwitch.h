#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class CommandAssignSwitch :
		public ska::AbstractFunctionCommand
	{
	public:
		CommandAssignSwitch(EntityManager& entityManager);
		virtual ~CommandAssignSwitch();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber();
	};
}
