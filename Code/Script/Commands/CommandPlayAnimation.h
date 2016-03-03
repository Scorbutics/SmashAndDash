#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandPlayAnimation :
	public ska::AbstractFunctionCommand
{
public:
	CommandPlayAnimation();
	virtual ~CommandPlayAnimation();

	virtual std::string execute(ska::IScript* script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

