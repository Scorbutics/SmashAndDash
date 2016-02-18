#pragma once
#include "AbstractFunctionCommand.h"
class CommandShop :
	public AbstractFunctionCommand
{
public:
	CommandShop();
	virtual ~CommandShop();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

