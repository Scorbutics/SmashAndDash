#pragma once
#include "AbstractFunctionCommand.h"
namespace ska {
	class CommandWait :
		public AbstractFunctionCommand
	{
	public:
		explicit CommandWait(EntityManager& e);
		void operator=(const CommandWait&) = delete;
		virtual ~CommandWait();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	};

}
