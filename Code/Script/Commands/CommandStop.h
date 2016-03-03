#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandStop :
	public ska::AbstractFunctionCommand
{
public:
	CommandStop();
	~CommandStop();

	virtual std::string execute(ska::IScript* script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

