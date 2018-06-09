#pragma once
#include "Script/Command/AbstractFunctionCommand.h"

class CommandThrowPokebal :
	public ska::AbstractFunctionCommand {
public:
	CommandThrowPokebal(ska::EntityManager& entityManager);
	virtual ~CommandThrowPokebal() = default;

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

