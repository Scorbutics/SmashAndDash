#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandDirection :
	public ska::AbstractFunctionCommand
{
public:
	CommandDirection();
	virtual ~CommandDirection();

	virtual std::string execute(ska::IScript* script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

