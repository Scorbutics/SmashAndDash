#pragma once
#include "AbstractFunctionCommand.h"

class CommandPlayerPresence : public AbstractFunctionCommand
{
public:
	CommandPlayerPresence() { }

	virtual ~CommandPlayerPresence() { }

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};


