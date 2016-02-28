#pragma once
#include <string>
#include <memory>
#include <fstream>
#include <unordered_map>
#include "IScript.h"
#include "Command\Command.h"

namespace ska {
	class Script :
		public IScript
	{
	public:
		Script(const int triggeringType, const unsigned int scriptPeriod, const std::string& fullPath, const std::string& extendedName, const std::string& key);
		Script(const int triggeringType, const unsigned int scriptPeriod, const std::string& fullPath, const std::string& extendedName, const std::string& key, const std::vector<std::string>& extraArgs);
		virtual ~Script();
		
		
		bool play() override;
		ScriptState getCurrentState() const override;
		unsigned int getCurrentLine() const override;
		std::string getExtendedName() const override;
		std::string getKey() const override;
		void delay(const unsigned int d) override;
		void stop() override;
		std::string nextLine() override;
		std::string getLastResult() const override;
		float getPriority(const unsigned int currentTimeMillis) override;
		bool eof() const override;
		void killAndSave(const ska::Savegame& savegame) override;
		std::string interpret(const std::string& cmd) override;

		std::string getFullPath() const;
		bool canBePlayed();
		static void addCommand(const std::string& key, CommandPtr& cmd);

	private:
		static unsigned int MAX_CONSECUTIVE_COMMANDS_PLAYED;
		static std::unordered_map<std::string, CommandPtr> m_commands;
		ScriptState manageCurrentState();
		bool transferActiveToDelay();
		void commonPartConstructor(const unsigned int scriptPeriod, const int triggeringType);

		virtual std::unordered_map<std::string, std::string>& getVarMap() override;

		long m_scriptPeriod;
		unsigned int m_lastTimeStarted;
		int m_active;

		/* Manage states in order to dispatch all different scripts */
		ScriptState m_state;
		unsigned int m_commandsPlayed;
		unsigned int m_currentLine;

		std::unordered_map<std::string, std::string> m_varMap;
		std::string m_fullPath;
		std::string m_extendedName;
		std::vector<std::string> m_extraArgs;
		std::string m_key;
		std::string m_lastResult;
		std::ifstream m_fscript;
		int m_triggeringType;
		unsigned int m_delay;
		unsigned int m_lastTimeDelayed;
	};

	typedef std::unique_ptr<Script> ScriptPtr;
}