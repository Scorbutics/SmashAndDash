#pragma once
#include "Script/Command/AbstractFunctionCommand.h"

class CommandPlayShaking :
	public ska::AbstractFunctionCommand {
public:
	CommandPlayShaking(ska::EntityManager& entityManager);
	virtual ~CommandPlayShaking() = default;

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

