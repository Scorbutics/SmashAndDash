#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandShop :
	public ska::AbstractFunctionCommand
{
public:
	CommandShop();
	virtual ~CommandShop();

	virtual std::string execute(ska::IScript* script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

