#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandHideCharacter :
	public ska::AbstractFunctionCommand
{
public:
	CommandHideCharacter();
	virtual ~CommandHideCharacter();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

