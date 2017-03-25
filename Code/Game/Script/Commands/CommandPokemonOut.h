#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandPokemonOut :
	public ska::AbstractFunctionCommand
{
public:
	CommandPokemonOut(ska::EntityManager& entityManager);
	virtual ~CommandPokemonOut();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

