#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandMessage :
	public ska::AbstractFunctionCommand
{
public:
	CommandMessage(ska::EntityManager& entityManager);

	virtual ~CommandMessage();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

