#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandMessage :
	public ska::AbstractFunctionCommand
{
public:
	CommandMessage();
	
	virtual ~CommandMessage();

	virtual std::string execute(ska::IScript* script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

