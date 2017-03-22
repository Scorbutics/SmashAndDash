#pragma once
#include "AbstractFunctionCommand.h"
namespace ska {
	class CommandScript :
		public AbstractFunctionCommand {
	public:
		explicit CommandScript(EntityManager& e);
		void operator=(const CommandScript&) = delete;
		virtual ~CommandScript();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	};

}
