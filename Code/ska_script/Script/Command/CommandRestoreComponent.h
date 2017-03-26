#pragma once
#include "AbstractFunctionCommand.h"
namespace ska {
	class CommandRestoreComponent :
		public AbstractFunctionCommand
	{
	public:
		explicit CommandRestoreComponent(EntityManager& e);
		void operator=(const CommandRestoreComponent&) = delete;
		virtual ~CommandRestoreComponent();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	};
}
