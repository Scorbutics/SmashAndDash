#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandKillEntity :
	public ska::AbstractFunctionCommand
{
public:
	CommandKillEntity(ska::EntityManager& entityManager);
	virtual ~CommandKillEntity();

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

