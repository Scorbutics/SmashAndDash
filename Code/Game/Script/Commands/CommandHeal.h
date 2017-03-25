#pragma once
#include "Script/Command/AbstractFunctionCommand.h"
class CommandHeal :
	public ska::AbstractFunctionCommand
{
public:
	CommandHeal(ska::EntityManager& entityManager);
	virtual ~CommandHeal();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

