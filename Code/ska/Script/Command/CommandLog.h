#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class CommandLog :
		public AbstractFunctionCommand
	{
	public:
		explicit CommandLog(EntityManager& e);
		void operator=(const CommandLog&) = delete;
		virtual ~CommandLog();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	};
}
