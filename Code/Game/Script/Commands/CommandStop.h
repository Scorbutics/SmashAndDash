#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandStop :
	public ska::AbstractFunctionCommand
{
public:
	CommandStop(ska::EntityManager& entityManager);
	~CommandStop();

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

