#pragma once
#include "AbstractFunctionCommand.h"
namespace ska {
	class CommandBlockAuthorize :
		public ska::AbstractFunctionCommand
	{
	public:
		CommandBlockAuthorize(EntityManager& e);
		virtual ~CommandBlockAuthorize();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber();
	};
}
