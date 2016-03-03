#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class CommandCalculate :
		public ska::AbstractFunctionCommand
	{
	public:
		CommandCalculate();
		virtual ~CommandCalculate();

		virtual std::string execute(ska::IScript* script, std::vector<std::string>& args) override;
		virtual int argumentsNumber();
	};
}
