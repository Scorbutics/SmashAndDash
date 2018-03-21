#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandJump :
	public ska::AbstractFunctionCommand
{
public:
	CommandJump(ska::EntityManager& entityManager);

	virtual ~CommandJump();

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

