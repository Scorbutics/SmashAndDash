#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandPlayShaking :
	public ska::AbstractFunctionCommand
{
public:
	CommandPlayShaking(ska::EntityManager& entityManager);
	virtual ~CommandPlayShaking();

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

