#pragma once
#include "Script/Command/AbstractFunctionCommand.h"

class CommandMove :
	public ska::AbstractFunctionCommand {
public:
	CommandMove(ska::EntityManager& entityManager);
	virtual ~CommandMove() = default;

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

