#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandPlayShaking :
	public ska::AbstractFunctionCommand
{
public:
	CommandPlayShaking();
	virtual ~CommandPlayShaking();

	virtual std::string execute(ska::IScript* script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

