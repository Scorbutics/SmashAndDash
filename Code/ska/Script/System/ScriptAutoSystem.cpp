#include <algorithm>
#include <sstream>
#include "ScriptAutoSystem.h"
#include "../../Exceptions/ScriptDiedException.h"
#include "../../Exceptions\ScriptUnknownCommandException.h"
#include "../../Utils\ScriptUtils.h"
#include "../../Exceptions\InvalidPathException.h"
#include "../../Exceptions/NumberFormatException.h"
#include "../../Utils\StringUtils.h"
#include "../../Utils/FileUtils.h"
#include "../../Utils/TimeUtils.h"
#include "../ScriptSymbolsConstants.h"
#include "../../Utils/NumberUtils.h"
#include "../../Utils/SkaConstants.h"

using namespace std;

//Par défaut, un script "permanent" se rafraîchit toutes les 1 ms
#define SCRIPT_DEFAULT_PERIOD 1
#define MAX_CONSECUTIVE_COMMANDS_PLAYED 5

#define TRIGGER_AUTO_PERIODIC 0
#define TRIGGER_PRESS_KEY 1

ska::ScriptAutoSystem::ScriptAutoSystem(EntityManager& entityManager, ska::Savegame& saveGame) : System(entityManager), m_saveGame(saveGame){

}


void ska::ScriptAutoSystem::registerScript(ScriptComponent* parent, ScriptComponent& script) {
	script.parent = this;

	unsigned int i = 0;
	for (const string& curArg : script.extraArgs) {
		ScriptUtils::setValueFromVarOrSwitchNumber(m_saveGame, script.extendedName, "#arg" + ska::StringUtils::intToStr(i) + "#", curArg, script.varMap);
		i++;
	}
	ifstream scriptFile(script.fullPath);
	if (scriptFile.fail()) {
		throw ska::InvalidPathException("Impossible d'ouvrir le fichier script " + script.fullPath);
	}

	for (std::string line; std::getline(scriptFile, line); /**/) {
		script.file.push_back(line);
	}
}

bool ska::ScriptAutoSystem::eof(ScriptComponent& script) {
	return script.file.size() <= script.currentLine;
}

ska::Savegame& ska::ScriptAutoSystem::getSavegame() {
	return m_saveGame;
}

void ska::ScriptAutoSystem::refresh()
{
	ScriptComponent* nextScript = getHighestPriorityScript();
	if (nextScript == NULL) {
		return;
	}

	try {
		play(*nextScript, m_saveGame);
	} catch (ska::ScriptDiedException sde) {
		const std::string& scriptName = sde.getScript();
		if (scriptName.empty()) {
			killAndSave(*nextScript, m_saveGame);
		} else {
			if (m_scripts.find(scriptName) == m_scripts.end()) {
				cerr << "ERREUR SCRIPT [" << nextScript->extendedName << "] (l." << nextScript->currentLine << ") " << sde.what() << " Script not found : " << scriptName << endl;
			} else {
				killAndSave(*m_scripts[scriptName], m_saveGame);
			}
		}

	} catch (ska::ScriptException e) {
		cerr << "ERREUR SCRIPT [" << nextScript->extendedName << "] (l." << nextScript->currentLine << ") " << e.what() << endl;
	}

}

void ska::ScriptAutoSystem::killAndSave(ScriptComponent& script, const Savegame& savegame) {
	string& tmpScritFileName = ("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + savegame.getSaveName() + FILE_SEPARATOR"tmpscripts.data");
	std::ofstream scriptList;
	scriptList.open(tmpScritFileName.c_str(), ios::app);
	if (!scriptList.fail()) {
		scriptList << script.extendedName << endl;
		scriptList.close();
	}

	script.state = EnumScriptState::DEAD;
}

ska::ScriptComponent* ska::ScriptAutoSystem::getHighestPriorityScript() {
	float maxPriorityScriptValue = -1;
	ScriptComponent* maxPriorityScript = NULL;
	unsigned int currentTimeTicks = ska::TimeUtils::getTicks();

	for (EntityId entityId : m_processed) {
		ScriptComponent& script = m_entityManager.getComponent<ScriptComponent>(entityId);
		float currentVal = getPriority(script, currentTimeTicks);
		if (maxPriorityScriptValue < currentVal) {
			maxPriorityScriptValue = currentVal;
			maxPriorityScript = &script;
		}
	}

	/* maxPriorityScriptValue < 0 means no script in an "OK state" found */
	return maxPriorityScriptValue > 0 ? maxPriorityScript : NULL;
}

bool ska::ScriptAutoSystem::canBePlayed(ScriptComponent& script) {
	transferActiveToDelay(script);
	return !(EnumScriptState::RUNNING == script.state || script.active > 0 || (ska::TimeUtils::getTicks() - script.lastTimeDelayed) <= script.delay
		|| !(script.state == EnumScriptState::DEAD)
		&& (script.triggeringType == TRIGGER_AUTO_PERIODIC && script.state == EnumScriptState::STOPPED || script.state != EnumScriptState::STOPPED)
		&& !eof(script));
}


/* When possible, transfers the value of m_active containing a time to wait to m_delay */
bool ska::ScriptAutoSystem::transferActiveToDelay(ScriptComponent& script) {
	if (script.active > 1 && script.delay == 0) {
		script.state = EnumScriptState::PAUSED;
		script.delay = script.active;
		script.lastTimeDelayed = ska::TimeUtils::getTicks();
		script.active = 0;
		return true;
	}
	return false;
}

bool ska::ScriptAutoSystem::play(ScriptComponent& script, Savegame& savegame) {

	if (!canBePlayed(script)) {
		return false;
	}

	script.lastTimeStarted = ska::TimeUtils::getTicks();

	/* Update status once */
	if (manageCurrentState(script) == EnumScriptState::PAUSED) {
		return false;
	}

	script.parent = this;

	/* Read commands */
	while (!eof(script)) {
		const string cmd = nextLine(script);
		if (cmd != "") {
			script.lastResult = interpret(script, savegame, cmd);
			/* We need to "manageCurrentState" to keep a valid state for the script at each command except the last one (when scriptStop is true) */
			if (script.state == EnumScriptState::STOPPED || manageCurrentState(script) == EnumScriptState::PAUSED) {
				break;
			}
			script.commandsPlayed++;
		}
	}


	/*  If loop is exited with having a running script, it means that it's terminated.
	That's why we rewind m_fscript.
	(with a paused script here, it means that the script will resume next time we play it) */
	if (script.state == EnumScriptState::RUNNING) {
		script.state = EnumScriptState::STOPPED;
		/* If the script is terminated and triggering is not automatic, then we don't reload the script */
		if (script.triggeringType == TRIGGER_AUTO_PERIODIC) {
			/*script.fscript.clear();
			script.fscript.seekg(0, std::ios::beg);*/
		}
		script.commandsPlayed = 0;
		script.currentLine = 0;
	}

	return true;
}

std::string ska::ScriptAutoSystem::interpret(ScriptComponent& script, Savegame& savegame, const std::string& cmd) {
	string cmdName;
	stringstream streamCmd;

	streamCmd << cmd;
	streamCmd >> cmdName;
	/* No tabulation */
	std::remove(cmdName.begin(), cmdName.end(), '\t');

	if (cmdName.empty()) {
		if (streamCmd.eof()) {
			stop(script);
		}
		return "";
	}

	if (m_commands.find(cmdName) != m_commands.end()) {
		std::unordered_map<std::string, std::string>& varMap = script.varMap;
		try {
			return m_commands[cmdName]->process(*this, script, streamCmd);
		}
		catch (ska::NumberFormatException nfe) {
			throw ska::ScriptException("Commande " + cmdName + " : " + std::string(nfe.what()));
		}
	}
	else {
		throw ska::ScriptUnknownCommandException("Impossible de trouver la commande " + cmdName + " dans le moteur de scripts.");
	}
}

std::string ska::ScriptAutoSystem::nextLine(ScriptComponent& script) {
	std::string line = script.file[script.currentLine];
	script.currentLine++;
	return line;
}

float ska::ScriptAutoSystem::getPriority(ScriptComponent& script, const unsigned int currentTimeMillis) {
	/* If the script cannot be played, we have a negative priority */
	if (!canBePlayed(script)) {
		return -1;
	}

	/* If the script is automatic (or still running or paused) and has a 0 script period, then it's max priority */
	if (script.scriptPeriod == 0) {
		return (std::numeric_limits<float>::max)();
	}

	/* In other ways this coeff variable make the priority calculation */
	const float coeff = ((float)currentTimeMillis - script.lastTimeStarted) / script.scriptPeriod;

	if (script.state != EnumScriptState::PAUSED) {
		return coeff;
	}
	else {
		/* A PAUSED script must gain rapidly high priority contrary to a STOPPED one.
		Then we use the exponential to simulate that */
		return ska::NumberUtils::exponential(coeff);
	}
}

ska::ScriptState ska::ScriptAutoSystem::manageCurrentState(ScriptComponent& script) {
	/* If too many commands have been played or if the script has been paused (m_active > 1),
	update the script status to PAUSED.
	If state is PAUSED and delay is passed or if state is STOPPED, runs the script */
	if (script.commandsPlayed == MAX_CONSECUTIVE_COMMANDS_PLAYED) {
		script.state = EnumScriptState::PAUSED;
		script.commandsPlayed = 0;
	} else if (transferActiveToDelay(script)) {
	} else if (script.state == EnumScriptState::PAUSED && ska::TimeUtils::getTicks() - script.lastTimeDelayed  > script.delay) {
		script.state = EnumScriptState::RUNNING;
		script.delay = 0;
	} else {
		script.state = EnumScriptState::RUNNING;
	}
	return script.state;
}

void ska::ScriptAutoSystem::stop(ScriptComponent& script) {
	/* kind of delete the script */
	script.state = EnumScriptState::STOPPED;
	script.triggeringType = 2;
}

ska::ScriptAutoSystem::~ScriptAutoSystem()
{
}
