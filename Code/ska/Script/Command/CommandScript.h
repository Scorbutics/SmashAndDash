#pragma once
#include "AbstractFunctionCommand.h"
namespace ska {
	class CommandScript :
		public ska::AbstractFunctionCommand
	{
	public:
		CommandScript();
		virtual ~CommandScript();

		virtual std::string execute(ska::IScript* script, std::vector<std::string>& args) override;
		virtual int argumentsNumber();
	};

}