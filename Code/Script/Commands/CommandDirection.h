#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandDirection :
	public ska::AbstractFunctionCommand
{
public:
	CommandDirection(ska::EntityManager& entityManager);
	virtual ~CommandDirection();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

