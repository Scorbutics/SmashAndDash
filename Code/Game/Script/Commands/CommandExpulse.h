#pragma once
#include "Script/Command/AbstractFunctionCommand.h"

namespace ska {
	class TileWorld;
}

class CommandExpulse :
	public ska::AbstractFunctionCommand
{
public:
	CommandExpulse(const ska::TileWorld& w, ska::EntityManager& entityManager);

	virtual ~CommandExpulse();

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, std::vector<std::string>& args) override;
	virtual int argumentsNumber();

private:
	const ska::TileWorld& m_world;
};

