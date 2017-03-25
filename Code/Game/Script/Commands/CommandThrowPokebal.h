#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandThrowPokebal :
	public ska::AbstractFunctionCommand
{
public:
	CommandThrowPokebal(ska::EntityManager& entityManager);
	virtual ~CommandThrowPokebal();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

