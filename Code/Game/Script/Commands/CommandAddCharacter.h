#pragma once
#include "Script/Command/AbstractFunctionCommand.h"

namespace ska {
	namespace cp {
		class Space;
	}
}

class CustomEntityManager;

class CommandAddCharacter :
	public ska::AbstractFunctionCommand {
public:
	CommandAddCharacter(CustomEntityManager& entityManager, ska::cp::Space& space, unsigned int blockSize);
	virtual ~CommandAddCharacter() = default;

	virtual std::string execute(ska::ScriptComponent& script, ska::MemoryScript& memory, const std::vector<std::string>& args) override;
	virtual int argumentsNumber();
private:
	CustomEntityManager& m_customEntityManager;
	ska::cp::Space& m_space;
	unsigned int m_blockSize;
};

