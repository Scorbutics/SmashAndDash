#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandFollow :
	public ska::AbstractFunctionCommand
{
public:
	CommandFollow(ska::EntityManager& entityManager);
	virtual ~CommandFollow();

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

