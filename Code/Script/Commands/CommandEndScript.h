#pragma once
#include "CommandEnd.h"
class CommandEndScript :
	public CommandEnd
{
public:
	CommandEndScript();
	virtual ~CommandEndScript();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

