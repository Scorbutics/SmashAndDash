#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandFollow :
	public ska::AbstractFunctionCommand
{
public:
	CommandFollow();
	virtual ~CommandFollow();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

