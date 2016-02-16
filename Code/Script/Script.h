#pragma once
#include <string>
#include <memory>
#include <fstream>
#include "../Utils\IScript.h"
#include <unordered_map>


class Script :
	public IScript
{
public:
	Script(int triggeringType, Uint32 scriptPeriod, std::string& fullPath, std::string& extendedName);
	Script(int triggeringType, Uint32 scriptPeriod, std::string& fullPath, std::string& extendedName, std::string& extraArgs);
	virtual bool play();
	ScriptState getCurrentState();
	virtual unsigned int getCurrentLine();
	std::string& getFullPath();
	bool canBePlayed();
	virtual float getPriority(Uint32 currentTimeMillis);
	virtual ~Script();
	
private:
	static unsigned int MAX_CONSECUTIVE_COMMANDS_PLAYED;

	ScriptState manageCurrentState();
	void commonPartConstructor(Uint32 scriptPeriod, int triggeringType);

	virtual std::unordered_map<std::string, std::string>& getVarMap() override;

	long m_scriptPeriod;
	Uint32 m_lastTimeStarted;
	int m_active;
	
	/* Manage states in order to dispatch all different scripts */
	ScriptState m_state;
	unsigned int m_commandsPlayed;
	
	std::unordered_map<std::string, std::string> m_varMap;
	std::string m_fullPath;
	std::string m_extendedName;
	std::string m_extraArgs;
	std::ifstream m_fscript;
	int m_triggeringType;
	int m_delay;
	Uint32 m_lastTimeDelayed;
};

typedef std::unique_ptr<Script> Script_ptr;
