#pragma once
#include "AbstractFunctionCommand.h"
#include <map>
class CommandHeal :
	public AbstractFunctionCommand
{
public:
	CommandHeal();
	virtual ~CommandHeal();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

