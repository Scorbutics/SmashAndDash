#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class CommandCalculate :
		public AbstractFunctionCommand {
	public:
		explicit CommandCalculate(EntityManager& e);
		void operator=(const CommandCalculate&) = delete;
		virtual ~CommandCalculate();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber() override;
	};
}
