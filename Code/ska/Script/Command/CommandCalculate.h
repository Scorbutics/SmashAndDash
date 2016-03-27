#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class CommandCalculate :
		public ska::AbstractFunctionCommand
	{
	public:
		CommandCalculate(EntityManager& e);
		virtual ~CommandCalculate();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber();
	};
}
