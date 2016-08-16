#pragma once
#include <unordered_set>
#include <string>
#include <unordered_map>
#include "../Command/Command.h"
#include "../../Data/Savegame.h"
#include "../ScriptComponent.h"
#include "../../ECS/System.h"

namespace ska {
	class World;

	class ScriptAutoSystem : public System<std::unordered_set<EntityId>, ScriptComponent> {
	public:
		
		Savegame& getSavegame();
		const ska::ScriptComponent registerScript(ScriptComponent* parent, const EntityId scriptSleepEntity, const EntityId origin);
		void registerCommand(const std::string& cmdName, CommandPtr& cmd);
		void setupScriptArgs(ScriptComponent* parent, ScriptComponent& script, const std::vector<std::string>& args);
		void kill(const std::string& keyScript);
		virtual const std::string map(const std::string& key, const std::string& id) const;
		void registerNamedScriptedEntity(const std::string& nameEntity, const EntityId entity);
		void clearNamedScriptedEntities();
		void removeComponent(const std::string& componentName, const std::string& id) const;
		void restoreComponent(const std::string& componentName, const std::string& id) const;
		ska::EntityId getEntityFromName(const std::string& nameEntity);

		/* Utils */
		World& getWorld();

		/* ScriptComponent methods */
		float getPriority(ScriptComponent& script, const unsigned int currentTimeMillis);
		bool canBePlayed(ScriptComponent& script);
		bool transferActiveToDelay(ScriptComponent& script);
		bool play(ScriptComponent& script, Savegame& savegame);
		void killAndSave(ScriptComponent& script, const Savegame& savegame);
		ScriptState manageCurrentState(ScriptComponent& script);
		std::string nextLine(ScriptComponent& script);
		std::string interpret(ScriptComponent& script, Savegame& savegame, const std::string& cmd);
		void stop(ScriptComponent& script);
		bool eof(ScriptComponent& script);

		virtual ~ScriptAutoSystem();

	private:
		ska::Savegame& m_saveGame;
		ScriptComponent* getHighestPriorityScript();
		World& m_world;

		std::unordered_map<std::string, ScriptComponent> m_cache;
		std::unordered_map<std::string, CommandPtr> m_commands;

		std::unordered_map<std::string, EntityId> m_namedScriptedEntities;

	protected:
		virtual void refresh() override;
		struct ScriptCommandHelper {
			ScriptCommandHelper(ska::World& w, EntityManager& parent) : m_entityManager(parent) {}
			virtual void setupCommands(ska::World& w, std::unordered_map<std::string, CommandPtr>& commands) const = 0;
			EntityManager& m_entityManager;
		};

		ScriptAutoSystem(World& w, const ScriptCommandHelper& sch, EntityManager& entityManager, ska::Savegame& saveGame);
	};

}
