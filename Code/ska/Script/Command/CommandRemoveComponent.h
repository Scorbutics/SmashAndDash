#pragma once
#include "AbstractFunctionCommand.h"
namespace ska {
	class CommandRemoveComponent :
		public ska::AbstractFunctionCommand
	{
	public:
		CommandRemoveComponent(EntityManager& e);
		virtual ~CommandRemoveComponent();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber();
	};
}
