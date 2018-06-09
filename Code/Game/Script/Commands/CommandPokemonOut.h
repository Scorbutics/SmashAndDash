#pragma once
#include "Script/Command/AbstractFunctionCommand.h"

class CommandPokemonOut :
	public ska::AbstractFunctionCommand {
public:
	CommandPokemonOut(ska::EntityManager& entityManager);
	virtual ~CommandPokemonOut() = default;

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

