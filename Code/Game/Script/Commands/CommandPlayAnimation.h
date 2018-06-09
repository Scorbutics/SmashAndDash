#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandPlayAnimation :
	public ska::AbstractFunctionCommand
{
public:
	CommandPlayAnimation(ska::EntityManager& entityManager);
	virtual ~CommandPlayAnimation() = default;

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

