#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class CommandAssignSwitch :
		public AbstractFunctionCommand {
	public:
		explicit CommandAssignSwitch(EntityManager& entityManager);
		void operator=(const CommandAssignSwitch&) = delete;
		virtual ~CommandAssignSwitch();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	};
}
