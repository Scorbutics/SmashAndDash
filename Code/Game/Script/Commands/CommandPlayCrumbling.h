#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandPlayCrumbling :
	public ska::AbstractFunctionCommand
{
public:
	CommandPlayCrumbling(ska::EntityManager& entityManager);
	virtual ~CommandPlayCrumbling();

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

