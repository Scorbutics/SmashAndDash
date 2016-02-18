#pragma once
#include <iosfwd>
#include <map>
#include "AbstractFunctionCommand.h"

class CommandEnd : public AbstractFunctionCommand
{
public:
	CommandEnd();
	virtual ~CommandEnd();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

