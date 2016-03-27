#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"
class CommandTeleport :
	public ska::AbstractFunctionCommand
{
public:
	CommandTeleport(ska::EntityManager& entityManager);
	virtual ~CommandTeleport();

	static void teleportHeroToMap(std::string param);

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

