#pragma once
#include "AbstractFunctionCommand.h"
namespace ska {
	class CommandRandom :
		public AbstractFunctionCommand {
	public:
		explicit CommandRandom(EntityManager& e);
		void operator=(const CommandRandom&) = delete;
		virtual ~CommandRandom();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	};
}
