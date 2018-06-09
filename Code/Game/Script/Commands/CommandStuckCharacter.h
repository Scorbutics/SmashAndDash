#pragma once
#include "Script/Command/AbstractFunctionCommand.h"

class CommandStuckCharacter :
	public ska::AbstractFunctionCommand {
public:
	CommandStuckCharacter(ska::EntityManager& entityManager);
	virtual ~CommandStuckCharacter() = default;

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

