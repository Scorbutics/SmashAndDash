#pragma once
#include <unordered_set>
#include <string>
#include <unordered_map>
#include "../Command/Command.h"
#include "Data/Savegame.h"
#include "../ScriptComponent.h"
#include "ECS/System.h"
#include "ECS/Basics/Script/ScriptRegisterer.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"

namespace ska {
	class World;

	class ScriptAutoSystem : public ScriptRegisterer,
		public System<std::unordered_set<EntityId>, RequiredComponent<ScriptComponent>, PossibleComponent<ScriptSleepComponent, ScriptComponent>> {
	public:

		Savegame& getSavegame();
		void registerScript(ScriptComponent* parent, const EntityId scriptSleepEntity, const EntityId origin) override;
		void registerCommand(const std::string& cmdName, CommandPtr& cmd);
		void setupScriptArgs(ScriptComponent* parent, ScriptComponent& script, const std::vector<std::string>& args);
		void kill(const std::string& keyScript);
		virtual const std::string map(const std::string& key, const std::string& id) const;
		void registerNamedScriptedEntity(const std::string& nameEntity, const EntityId entity);
		void clearNamedScriptedEntities();
		void removeComponent(const std::string& componentName, const std::string& id) const;
		void restoreComponent(const std::string& componentName, const std::string& id) const;
		EntityId getEntityFromName(const std::string& nameEntity);

		/* ScriptComponent methods */
		float getPriority(ScriptComponent& script, const unsigned int currentTimeMillis);
		bool canBePlayed(ScriptComponent& script);
		bool transferActiveToDelay(ScriptComponent& script);
		bool play(ScriptComponent& script, Savegame& savegame);
		void killAndSave(ScriptComponent& script, const Savegame& savegame) const;
		ScriptState manageCurrentState(ScriptComponent& script);
		std::string nextLine(ScriptComponent& script);
		std::string interpret(ScriptComponent& script, Savegame& savegame, const std::string& cmd);
		void stop(ScriptComponent& script);
		bool eof(ScriptComponent& script);

		void operator=(const ScriptAutoSystem&) = delete;

		virtual ~ScriptAutoSystem();

	private:
		Savegame& m_saveGame;
		ScriptComponent* getHighestPriorityScript();
		World& m_world;

		std::unordered_map<std::string, ScriptComponent> m_cache;
		std::unordered_map<std::string, CommandPtr> m_commands;

		std::unordered_map<std::string, EntityId> m_namedScriptedEntities;

	protected:
		virtual void refresh(unsigned int ellapsedTime) override;
		struct ScriptCommandHelper {
			ScriptCommandHelper(World&, EntityManager& parent) : m_entityManager(parent) {}
			virtual ~ScriptCommandHelper() = default;
			void operator=(const ScriptCommandHelper&) = delete;
			virtual void setupCommands(World& w, std::unordered_map<std::string, CommandPtr>& commands) const = 0;
			EntityManager& m_entityManager;
		};

		ScriptAutoSystem(EntityManager& entityManager, World& w, const ScriptCommandHelper& sch, Savegame& saveGame);
	};

}
