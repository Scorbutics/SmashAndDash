#pragma once
#include <iosfwd>
#include "AbstractFunctionCommand.h"

namespace ska {
	class CommandEnd : public AbstractFunctionCommand
	{
	public:
		CommandEnd();
		virtual ~CommandEnd();

		static const std::string& getCmdName();

		virtual std::string execute(IScript* script, std::vector<std::string>& args) override;
		virtual int argumentsNumber();
	};
}
