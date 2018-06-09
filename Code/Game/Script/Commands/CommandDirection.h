#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandDirection :
	public ska::AbstractFunctionCommand
{
public:
	CommandDirection(ska::EntityManager& entityManager);
	virtual ~CommandDirection();

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

