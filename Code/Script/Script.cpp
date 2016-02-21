#include <limits>
#include <string>

#include "Script.h"
#include "../Utils\ScriptUtils.h"
#include "../Exceptions\ScriptNotActivatedException.h"
#include "ScriptDispatcher.h"
#include "../Gameplay\WGameCore.h"

#define TRIGGER_AUTO_PERIODIC 0
#define TRIGGER_PRESS_KEY 1

using namespace std;

unsigned int Script::MAX_CONSECUTIVE_COMMANDS_PLAYED = 5;

void Script::commonPartConstructor(Uint32 scriptPeriod, int triggeringType) {
	m_fscript.open(m_fullPath.c_str());
	m_scriptPeriod = scriptPeriod;
	m_commandsPlayed = 0;
	m_state = EnumScriptState::STOPPED;
	m_triggeringType = triggeringType;
	m_lastTimeDelayed = 0;
	m_currentLine = 0;
}

std::unordered_map<std::string, std::string>& Script::getVarMap()
{
	return m_varMap;
}

Script::Script(int triggeringType, Uint32 scriptPeriod, std::string& fullPath, std::string& extendedName, const std::string& key) : m_extendedName(extendedName), m_fullPath(fullPath), m_key(key) {
	commonPartConstructor(scriptPeriod, triggeringType);
}

Script::Script(int triggeringType, Uint32 scriptPeriod, std::string& fullPath, std::string& extendedName, const std::string& key, std::string& extraArgs) : m_extendedName(extendedName), m_fullPath(fullPath), m_extraArgs(extraArgs), m_key(key) {
	commonPartConstructor(scriptPeriod, triggeringType);
}

std::string Script::nextLine() {
	std::string line;
	getline(m_fscript, line);
	m_currentLine++;
	return line;
}

bool Script::eof() {
	return m_fscript.eof();
}

bool Script::play()
{
	WGameCore& wScreen = WGameCore::getInstance();
	string cmd;
	string result;
	

	if (!canBePlayed()) {
		return false;
	}

	//if (m_state == EnumScriptState::STOPPED) {
	m_lastTimeStarted = SDL_GetTicks();
	//}
	
	/* Update status once */
	if (manageCurrentState() == EnumScriptState::PAUSED) {
		return false;
	}

	/* Read commands */
	while (!eof()) {
		cmd = nextLine();
		if (cmd != "") {
			m_lastResult = ScriptDispatcher::commandInterpreter(this, cmd);
			/* We need to "manageCurrentState" to keep a valid state for the script at each command except the last one (when scriptStop is true) */
			if (m_state == EnumScriptState::STOPPED || manageCurrentState() == EnumScriptState::PAUSED) {
				break;
			}
			m_commandsPlayed++;
		}
	}


	/*  If loop is exited with having a running script, it means that it's terminated.
		That's why we rewind m_fscript.
		(with a paused script here, it means that the script will resume next time we play it) */
	if (m_state == EnumScriptState::RUNNING) {
		m_state = EnumScriptState::STOPPED;
		/* If the script is terminated and triggering is not automatic, then we don't reload the script */
		if (m_triggeringType == TRIGGER_AUTO_PERIODIC) {
			m_fscript.clear();
			m_fscript.seekg(0, std::ios::beg);
		}
		m_commandsPlayed = 0;
		m_currentLine = 0;
	}

	return true;
}

std::string& Script::getKey() {
	return m_key;
}

std::string& Script::getExtendedName() {
	return m_extendedName;
}

std::string Script::getLastResult() {
	return m_lastResult;
}

void Script::delay(int d) {
	m_active = d;
}

void Script::stop() {
	/* kind of delete the script */
	m_state = EnumScriptState::STOPPED;
	m_triggeringType = 2;
}

/* When possible, transfers the value of m_active containing a time to wait to m_delay */
bool Script::transferActiveToDelay() {
	if (m_active > 1 && m_delay == 0) {
		m_state = EnumScriptState::PAUSED;
		m_delay = m_active;
		m_lastTimeDelayed = SDL_GetTicks();
		m_active = 0;
		return true;
	}
	return false;
}

ScriptState Script::manageCurrentState() {
	/* If too many commands have been played or if the script has been paused (m_active > 1), 
	   update the script status to PAUSED.
	   If state is PAUSED and delay is passed or if state is STOPPED, runs the script */
	if (m_commandsPlayed == MAX_CONSECUTIVE_COMMANDS_PLAYED) {
		m_state = EnumScriptState::PAUSED;
		m_commandsPlayed = 0;
	} else if (transferActiveToDelay()) {
	} else if (m_state == EnumScriptState::PAUSED && SDL_GetTicks() - m_lastTimeDelayed  > m_delay) {
		m_state = EnumScriptState::RUNNING;
		m_delay = 0;
	}  else {
		m_state = EnumScriptState::RUNNING;
	}
	return m_state;
}

ScriptState Script::getCurrentState() {
	return m_state;
}

unsigned int Script::getCurrentLine() {
	return m_currentLine;
}

std::string& Script::getFullPath() {
	return m_fullPath;
}

bool Script::canBePlayed() {
	transferActiveToDelay();
	return !(EnumScriptState::RUNNING == m_state || m_active > 0 || (SDL_GetTicks() - m_lastTimeDelayed) <= m_delay 
		|| !ScriptUtils::isScriptActivated(m_extendedName)
		&& (m_triggeringType == TRIGGER_AUTO_PERIODIC && m_state == EnumScriptState::STOPPED || m_state != EnumScriptState::STOPPED)
		&& !m_fscript.eof());
}

float Script::getPriority(Uint32 currentTimeMillis) {
	/* If the script cannot be played, we have a negative priority */
	if (!canBePlayed()) {
		return -1;
	}

	/* If the script is automatic (or still running or paused) and has a 0 script period, then it's max priority */
	if (m_scriptPeriod == 0) {
		return (std::numeric_limits<float>::max)();
	}

	/* In other ways this coeff variable make the priority calculation */
	const float coeff = ((float)currentTimeMillis - m_lastTimeStarted) / m_scriptPeriod;

	/* If the script period is not reached (coeff < 1), priority must be negative */
	const float result = coeff /*< 1 ? -1 : coeff*/;

	if (m_state != EnumScriptState::PAUSED /*|| coeff < 1*/) {
		return result;
	} else {
		/* A PAUSED script must gain rapidly high priority contrary to a STOPPED one.
		Then we use the exponential to simulate that */
		return exp(coeff);
	}
}

Script::~Script()
{
	m_fscript.close();
}
