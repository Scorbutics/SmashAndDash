#pragma once
#include "../../ska/Script/Command/AbstractFunctionCommand.h"

namespace ska {
	class World;
}

class CommandTeleport :
	public ska::AbstractFunctionCommand
{
public:
	CommandTeleport(ska::EntityManager& entityManager);
	virtual ~CommandTeleport();

	static void teleportHeroToMap(ska::World& w, std::string param);

	virtual std::string execute(ska::ScriptComponent& script, std::vector<std::string>& args) override;
	virtual int argumentsNumber();
};

