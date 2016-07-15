#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandExpulse :
	public ska::AbstractFunctionCommand
{
public:
	CommandExpulse(ska::EntityManager& entityManager);
	
	virtual ~CommandExpulse();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

