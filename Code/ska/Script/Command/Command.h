#pragma once
#include <fstream>
#include <memory>
#include <string>

namespace ska {
	class ScriptComponent;
	class ScriptAutoSystem;
	class EntityManager;
	class Command {
	public:
		explicit Command(EntityManager& entityManager) : m_entityManager(entityManager) {}
		void operator=(const Command&) = delete;
		virtual std::string process(ScriptAutoSystem& saveGame, ScriptComponent& script, std::stringstream& streamCmd) = 0;
		virtual ~Command() {}
	
	protected:
		EntityManager& m_entityManager;

	};
	typedef std::unique_ptr<Command> CommandPtr;
}
