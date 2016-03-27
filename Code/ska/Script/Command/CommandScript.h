#pragma once
#include "AbstractFunctionCommand.h"
namespace ska {
	class CommandScript :
		public ska::AbstractFunctionCommand
	{
	public:
		CommandScript(EntityManager& e);
		virtual ~CommandScript();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber();
	};

}