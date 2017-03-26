#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandChoice :
	public ska::AbstractFunctionCommand
{
public:
	CommandChoice(ska::EntityManager& entityManager);

	virtual ~CommandChoice();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

