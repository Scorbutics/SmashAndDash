#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandPlayAnimation :
	public ska::AbstractFunctionCommand
{
public:
	CommandPlayAnimation(ska::EntityManager& entityManager);
	virtual ~CommandPlayAnimation();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

