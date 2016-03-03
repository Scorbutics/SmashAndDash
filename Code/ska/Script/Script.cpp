#include <limits>
#include <algorithm>
#include <sstream>
#include <string>

#include "Script.h"
#include "../Utils\ScriptUtils.h"
#include "../Exceptions\ScriptNotActivatedException.h"
#include "../Exceptions/NumberFormatException.h"
#include "../Exceptions/ScriptDiedException.h"
#include "../Exceptions/ScriptUnknownCommandException.h"
#include "ScriptDispatcher.h"
#include "../Utils/TimeUtils.h"
#include "../Utils/SkaConstants.h"
#include "Command\CommandIf.h"
#include "Command\CommandElse.h"
#include "Command\CommandElseEnd.h"
#include "ScriptDispatcher.h"

#define TRIGGER_AUTO_PERIODIC 0
#define TRIGGER_PRESS_KEY 1

using namespace std;

unordered_map<string, ska::CommandPtr> createCmdMap() {
	unordered_map<string, ska::CommandPtr> c;
	c[ska::ControlStatement::getCommandIf()] = move(ska::CommandPtr(new ska::CommandIf()));
	c[ska::ControlStatement::getCommandElse()] = move(ska::CommandPtr(new ska::CommandElse()));
	c[ska::ControlStatement::getCommandEndIf()] = move(ska::CommandPtr(new ska::CommandElseEnd()));
	return c;
}

unordered_map<string, ska::CommandPtr> ska::Script::m_commands = createCmdMap();
unsigned int ska::Script::MAX_CONSECUTIVE_COMMANDS_PLAYED = 5;

void ska::Script::commonPartConstructor(const unsigned int scriptPeriod, const int triggeringType, const std::string& context) {
	m_fscript.open(m_fullPath.c_str());
	m_scriptPeriod = scriptPeriod;
	m_commandsPlayed = 0;
	m_context = context;
	m_state = EnumScriptState::STOPPED;
	m_triggeringType = triggeringType;
	m_lastTimeDelayed = 0;
	m_currentLine = 0;
}

std::unordered_map<std::string, std::string>& ska::Script::getVarMap()
{
	return m_varMap;
}

ska::Script::Script(ScriptDispatcher& parent, const int triggeringType, const unsigned int scriptPeriod, const std::string& fullPath, const std::string& extendedName, const std::string& context, const std::string& key) : m_parent(parent), m_extendedName(extendedName), m_fullPath(fullPath), m_key(key) {
	commonPartConstructor(scriptPeriod, triggeringType, context);
}

ska::ScriptDispatcher& ska::Script::getParent() {
	return m_parent;
}

std::string ska::Script::getContext() const {
	return m_context;
}

ska::Script::Script(ScriptDispatcher& parent, const int triggeringType, const unsigned int scriptPeriod, const std::string& fullPath, const std::string& extendedName, const std::string& context, const std::string& key, const vector<string>& extraArgs) : m_parent(parent), m_extendedName(extendedName), m_fullPath(fullPath), m_extraArgs(extraArgs), m_key(key) {
	commonPartConstructor(scriptPeriod, triggeringType, context);
}

std::string ska::Script::nextLine() {
	std::string line;
	getline(m_fscript, line);
	m_currentLine++;
	return line;
}

bool ska::Script::eof() const {
	return m_fscript.eof();
}

void ska::Script::addCommand(const std::string& key, ska::CommandPtr& cmd) {
	m_commands[key] = move(cmd);

}

std::string ska::Script::interpret(Savegame& savegame, const std::string& cmd) {
	string cmdName;
	stringstream streamCmd;

	streamCmd << cmd;
	streamCmd >> cmdName;
	/* No tabulation */
	std::remove(cmdName.begin(), cmdName.end(), '\t');

	if (cmdName.empty()) {
		if (streamCmd.eof()) {
			stop();
		}
		return "";
	}

	if (m_commands.find(cmdName) != m_commands.end()) {
		std::unordered_map<std::string, std::string>& varMap = m_varMap;
		try {
			return m_commands[cmdName]->process(savegame, this, streamCmd);
		} catch (ska::NumberFormatException nfe) {
			throw ska::ScriptException("Commande " + cmdName + " : " + std::string(nfe.what()));
		}
	}
	else {
		throw ska::ScriptUnknownCommandException("Impossible de trouver la commande " + cmdName + " dans le moteur de scripts.");
	}
}

bool ska::Script::play(Savegame& savegame) {
	string cmd;
	string result;
	

	if (!canBePlayed()) {
		return false;
	}

	m_lastTimeStarted = ska::TimeUtils::getTicks();
		
	/* Update status once */
	if (manageCurrentState() == EnumScriptState::PAUSED) {
		return false;
	}

	/* Read commands */
	while (!eof()) {
		cmd = nextLine();
		if (cmd != "") {
			m_lastResult = interpret(savegame, cmd);
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

std::string ska::Script::getKey() const {
	return m_key;
}

std::string ska::Script::getExtendedName() const {
	return m_extendedName;
}

std::string ska::Script::getLastResult() const {
	return m_lastResult;
}

void ska::Script::delay(const unsigned int d) {
	m_active = d;
}

void ska::Script::stop() {
	/* kind of delete the script */
	m_state = EnumScriptState::STOPPED;
	m_triggeringType = 2;
}

/* When possible, transfers the value of m_active containing a time to wait to m_delay */
bool ska::Script::transferActiveToDelay() {
	if (m_active > 1 && m_delay == 0) {
		m_state = EnumScriptState::PAUSED;
		m_delay = m_active;
		m_lastTimeDelayed = ska::TimeUtils::getTicks();
		m_active = 0;
		return true;
	}
	return false;
}

ska::ScriptState ska::Script::manageCurrentState() {
	/* If too many commands have been played or if the script has been paused (m_active > 1), 
	   update the script status to PAUSED.
	   If state is PAUSED and delay is passed or if state is STOPPED, runs the script */
	if (m_commandsPlayed == MAX_CONSECUTIVE_COMMANDS_PLAYED) {
		m_state = EnumScriptState::PAUSED;
		m_commandsPlayed = 0;
	} else if (transferActiveToDelay()) {
	} else if (m_state == EnumScriptState::PAUSED && ska::TimeUtils::getTicks() - m_lastTimeDelayed  > m_delay) {
		m_state = EnumScriptState::RUNNING;
		m_delay = 0;
	}  else {
		m_state = EnumScriptState::RUNNING;
	}
	return m_state;
}

ska::ScriptState ska::Script::getCurrentState() const {
	return m_state;
}

unsigned int ska::Script::getCurrentLine() const {
	return m_currentLine;
}

std::string ska::Script::getFullPath() const {
	return m_fullPath;
}

void ska::Script::killAndSave(const Savegame& savegame) {
	string& tmpScritFileName = ("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + savegame.getSaveName() + FILE_SEPARATOR"tmpscripts.data");
	std::ofstream scriptList;
	scriptList.open(tmpScritFileName.c_str(), ios::app);
	if (!scriptList.fail()) {
		scriptList << m_extendedName << endl;
		scriptList.close();
	}

	m_state = EnumScriptState::DEAD;
}

bool ska::Script::canBePlayed() {
	transferActiveToDelay();
	return !(EnumScriptState::RUNNING == m_state || m_active > 0 || (ska::TimeUtils::getTicks() - m_lastTimeDelayed) <= m_delay
		|| !(m_state == EnumScriptState::DEAD)
		&& (m_triggeringType == TRIGGER_AUTO_PERIODIC && m_state == EnumScriptState::STOPPED || m_state != EnumScriptState::STOPPED)
		&& !m_fscript.eof());
}

float ska::Script::getPriority(const unsigned int currentTimeMillis) {
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

ska::Script::~Script()
{
	m_fscript.close();
}
