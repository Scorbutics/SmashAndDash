#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"

class CommandPlayerPresence : public ska::AbstractFunctionCommand
{
public:
	CommandPlayerPresence() { }

	virtual ~CommandPlayerPresence() { }

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};


