#pragma once
#include "AbstractFunctionCommand.h"
class CommandStuckCharacter :
	public AbstractFunctionCommand
{
public:
	CommandStuckCharacter();
	virtual ~CommandStuckCharacter();

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

