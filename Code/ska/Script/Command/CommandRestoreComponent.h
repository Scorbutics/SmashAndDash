#pragma once
#include "AbstractFunctionCommand.h"
namespace ska {
	class CommandRestoreComponent :
		public ska::AbstractFunctionCommand
	{
	public:
		CommandRestoreComponent(EntityManager& e);
		virtual ~CommandRestoreComponent();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber();
	};
}
