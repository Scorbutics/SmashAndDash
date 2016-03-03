#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandCinematic :
	public ska::AbstractFunctionCommand
{
public:
	CommandCinematic();
	virtual ~CommandCinematic();

	virtual std::string execute(ska::IScript* script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

