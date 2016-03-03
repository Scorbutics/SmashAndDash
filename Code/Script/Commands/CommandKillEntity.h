#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandKillEntity :
	public ska::AbstractFunctionCommand
{
public:
	CommandKillEntity();
	virtual ~CommandKillEntity();

	virtual std::string execute(ska::IScript* script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

