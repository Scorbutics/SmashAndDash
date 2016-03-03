#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandMove :
	public ska::AbstractFunctionCommand
{
public:
	CommandMove();
	
	virtual ~CommandMove();

	virtual std::string execute(ska::IScript* script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

