#pragma once
#include "AbstractFunctionCommand.h"
class CommandTeleport :
	public AbstractFunctionCommand
{
public:
	CommandTeleport();
	virtual ~CommandTeleport();

	static void teleportHeroToMap(std::string param);

	virtual std::string execute(IScript* script, std::vector<std::string>& args, std::ofstream& scriptList) override;
	virtual int argumentsNumber();
};

