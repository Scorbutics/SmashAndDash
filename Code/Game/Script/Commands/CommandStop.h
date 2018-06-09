#pragma once
#include "Script/Command/AbstractFunctionCommand.h"

class CommandStop :
	public ska::AbstractFunctionCommand {
public:
	CommandStop(ska::EntityManager& entityManager);
	virtual ~CommandStop() = default;

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

