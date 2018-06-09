#pragma once
#include "Script/Command/AbstractFunctionCommand.h"

class CommandPlayerPresence : 
	public ska::AbstractFunctionCommand {
public:
	using AbstractFunctionCommand::AbstractFunctionCommand;
	virtual ~CommandPlayerPresence() = default;

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};


