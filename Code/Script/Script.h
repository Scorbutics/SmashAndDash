#pragma once
#include <string>
#include <memory>
#include <fstream>
#include "IScript.h"
#include <unordered_map>


class Script :
	public IScript
{
public:
	Script(int triggeringType, Uint32 scriptPeriod, std::string& fullPath, std::string& extendedName, const std::string& key);
	Script(int triggeringType, Uint32 scriptPeriod, std::string& fullPath, std::string& extendedName, const std::string& key, std::string& extraArgs);
	bool play();
	ScriptState getCurrentState();
	unsigned int getCurrentLine();
	std::string& getFullPath();
	std::string& getExtendedName();
	std::string& getKey();
	bool canBePlayed();
	void delay(int d);
	void stop();
	std::string nextLine();
	std::string getLastResult();
	float getPriority(Uint32 currentTimeMillis);
	bool eof();
	virtual ~Script();
	
private:
	static unsigned int MAX_CONSECUTIVE_COMMANDS_PLAYED;

	ScriptState manageCurrentState();
	bool transferActiveToDelay();
	void commonPartConstructor(Uint32 scriptPeriod, int triggeringType);

	virtual std::unordered_map<std::string, std::string>& getVarMap() override;

	long m_scriptPeriod;
	Uint32 m_lastTimeStarted;
	int m_active;
	
	/* Manage states in order to dispatch all different scripts */
	ScriptState m_state;
	unsigned int m_commandsPlayed;
	unsigned int m_currentLine;
	
	std::unordered_map<std::string, std::string> m_varMap;
	std::string m_fullPath;
	std::string m_extendedName;
	std::string m_extraArgs;
	std::string m_key;
	std::string m_lastResult;
	std::ifstream m_fscript;
	int m_triggeringType;
	int m_delay;
	Uint32 m_lastTimeDelayed;
};

typedef std::unique_ptr<Script> Script_ptr;
