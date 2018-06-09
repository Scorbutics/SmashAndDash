#pragma once
#include "Script/Command/AbstractFunctionCommand.h"

class CommandPlayCrumbling :
	public ska::AbstractFunctionCommand {
public:
	CommandPlayCrumbling(ska::EntityManager& entityManager);
	virtual ~CommandPlayCrumbling() = default;

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

