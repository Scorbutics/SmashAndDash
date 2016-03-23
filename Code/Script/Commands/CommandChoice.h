#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandChoice :
	public ska::AbstractFunctionCommand
{
public:
	CommandChoice();
	
	virtual ~CommandChoice();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

