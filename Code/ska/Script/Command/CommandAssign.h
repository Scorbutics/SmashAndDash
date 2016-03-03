#pragma once
#include "AbstractFunctionCommand.h"

namespace ska {
	class CommandAssign :
		public ska::AbstractFunctionCommand
	{
	public:
		CommandAssign();
		virtual ~CommandAssign();

		virtual std::string execute(ska::IScript* script, std::vector<std::string>& args) override;
		virtual int argumentsNumber();
	};
}
