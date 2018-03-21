#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandHideCharacter :
	public ska::AbstractFunctionCommand
{
public:
	CommandHideCharacter(ska::EntityManager& entityManager);
	virtual ~CommandHideCharacter();

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

