#pragma once
#include "AbstractFunctionCommand.h"
namespace ska {
	class CommandRandom :
		public ska::AbstractFunctionCommand
	{
	public:
		CommandRandom();
		virtual ~CommandRandom();

		virtual std::string execute(ScriptComponent& script, std::vector<std::string>& args) override;
		virtual int argumentsNumber();
	};
}
