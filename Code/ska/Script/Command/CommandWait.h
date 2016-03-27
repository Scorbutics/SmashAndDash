#pragma once
#include "AbstractFunctionCommand.h"
namespace ska {
	class CommandWait :
		public ska::AbstractFunctionCommand
	{
	public:
		CommandWait(EntityManager& e);
		virtual ~CommandWait();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber();
	};

}