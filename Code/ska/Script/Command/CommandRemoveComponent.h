#pragma once
#include "AbstractFunctionCommand.h"
namespace ska {
	class CommandRemoveComponent :
		public AbstractFunctionCommand {
	public:
		explicit CommandRemoveComponent(EntityManager& e);
		void operator=(const CommandRemoveComponent&) = delete;
		virtual ~CommandRemoveComponent();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	};
}
