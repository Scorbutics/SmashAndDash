#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandStop :
	public ska::AbstractFunctionCommand
{
public:
	CommandStop(ska::EntityManager& entityManager);
	~CommandStop();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

