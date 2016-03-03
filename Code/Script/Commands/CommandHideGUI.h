#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandHideGUI :
	public ska::AbstractFunctionCommand
{
public:
	CommandHideGUI();
	virtual ~CommandHideGUI();

	virtual std::string execute(ska::IScript* script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

