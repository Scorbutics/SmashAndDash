#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandStuckCharacter :
	public ska::AbstractFunctionCommand
{
public:
	CommandStuckCharacter();
	virtual ~CommandStuckCharacter();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

