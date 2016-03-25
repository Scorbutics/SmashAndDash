#pragma once
#include <unordered_set>
#include <string>
#include <unordered_map>
#include "../Script.h"
#include "../Command/Command.h"
#include "../../Data/Savegame.h"
#include "../ScriptComponent.h"
#include "../../ECS/System.h"

namespace ska {
	class ScriptAutoSystem : public System<std::unordered_set<EntityId>, ScriptComponent>
	{

	public:
		ScriptAutoSystem(EntityManager& entityManager, ska::Savegame& saveGame);
		
		Savegame& getSavegame();
		void registerScript(ScriptComponent* parent, ScriptComponent& script);
		void setupScriptArgs(ScriptComponent* parent, ScriptComponent& script, const std::vector<std::string>& args);
		void kill(const std::string& keyScript);
		virtual void refresh() override;

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
		std::unordered_map<std::string, ScriptComponent*> m_scripts;
		std::unordered_map<std::string, CommandPtr> m_commands;
		
	};

}
