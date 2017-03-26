#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandMove :
	public ska::AbstractFunctionCommand
{
public:
	CommandMove(ska::EntityManager& entityManager);

	virtual ~CommandMove();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

