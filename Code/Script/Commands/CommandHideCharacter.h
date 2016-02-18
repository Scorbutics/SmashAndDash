#pragma once
#include "AbstractFunctionCommand.h"
class CommandHideCharacter :
	public AbstractFunctionCommand
{
public:
	CommandHideCharacter();
	virtual ~CommandHideCharacter();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

