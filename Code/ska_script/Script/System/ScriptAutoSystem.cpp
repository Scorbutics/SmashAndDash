#include <iostream>
#include <algorithm>
#include <sstream>
#include "ScriptAutoSystem.h"
#include "Exceptions/ScriptDiedException.h"
#include "Exceptions/ScriptUnknownCommandException.h"
#include "Exceptions/IllegalArgumentException.h"
#include "../ScriptUtils.h"
#include "Exceptions/InvalidPathException.h"
#include "Exceptions/NumberFormatException.h"
#include "Exceptions/ScriptSyntaxError.h"
#include "Utils/StringUtils.h"
#include "Utils/FileUtils.h"
#include "Utils/TimeUtils.h"
#include "Utils/NumberUtils.h"
#include "Utils/SkaConstants.h"
#include "ECS/Basics/Script/ScriptTriggerType.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"


//Par défaut, un script "permanent" se rafraîchit toutes les 1 ms
#define SCRIPT_DEFAULT_PERIOD 1
#define MAX_CONSECUTIVE_COMMANDS_PLAYED 5


ska::ScriptAutoSystem::ScriptAutoSystem(EntityManager& entityManager, World& w, const ScriptCommandHelper& sch, Savegame& saveGame) : System(entityManager),
m_saveGame(saveGame),
m_world(w) {
	sch.setupCommands(w, m_commands);
}

const std::string ska::ScriptAutoSystem::map(const std::string& key, const std::string& id) const {
	std::vector<std::string> keys = StringUtils::split(key, '.');
	if (keys.size() != 2) {
		throw ScriptSyntaxError("Error during recuperation of the global variable " + key);
	}

	if (m_namedScriptedEntities.find(id) != m_namedScriptedEntities.end()) {
		EntityId entity = m_namedScriptedEntities.at(id);
		return m_componentAccessor.serialize(entity, keys[0], keys[1]);
	}

	return "";
}

void ska::ScriptAutoSystem::removeComponent(const std::string& componentName, const std::string& id) const {
	if (m_namedScriptedEntities.find(id) != m_namedScriptedEntities.end()) {
		EntityId entity = m_namedScriptedEntities.at(id);
		//const auto idInt = StringUtils::strToInt(id);
		m_componentAccessor.remove(entity, componentName);
		//m_entityManager.refreshEntity(idInt);
	}
}

void ska::ScriptAutoSystem::restoreComponent(const std::string& componentName, const std::string& id) const {
	if (m_namedScriptedEntities.find(id) != m_namedScriptedEntities.end()) {
		EntityId entity = m_namedScriptedEntities.at(id);
		//const auto idInt = StringUtils::strToInt(id);
		m_componentAccessor.add(entity, componentName);
		//m_entityManager.refreshEntity(idInt);
	}
}

/*m_scripts[keyScript] = (move(ScriptPtr(new Script(*this, triggeringType, period == NULL || *period == 0 ? SCRIPT_DEFAULT_PERIOD : *period, validPath, extendedName, context, keyScript, args)))); */
void ska::ScriptAutoSystem::registerScript(ScriptComponent*, const EntityId scriptSleepEntity, const EntityId origin) {

	const auto& scriptCPtr = m_componentPossibleAccessor.get<ScriptSleepComponent>(scriptSleepEntity);
	if (scriptCPtr == nullptr) {
		throw IllegalArgumentException("The script entity to register has no ScriptSleepComponent");
	}

	/* If the script is already running, return */
	if (m_componentPossibleAccessor.get<ScriptComponent>(scriptSleepEntity) != nullptr) {
		return;
	}

	ScriptSleepComponent& scriptData = *scriptCPtr;

	std::string extendedName;
	std::string validPath;
	std::string keyArgs;

	for (const auto& arg : scriptData.args) {
		keyArgs += arg + " ";
	}

	StringUtils::rtrim(keyArgs);

	const std::string& keyScript = scriptData.name + "/\\" + keyArgs;
	extendedName = keyScript + "_" + scriptData.context;

	const std::string& currentDir = FileUtils::getCurrentDirectory();
	validPath = (currentDir + FILE_SEPARATOR "" + scriptData.name);

	ScriptComponent sc;
	if (m_cache.find(validPath) == m_cache.end()) {
		std::ifstream fscript(scriptData.name.c_str());
		if (fscript.fail()) {
			fscript.open(validPath.c_str());
			if (fscript.fail()) {
				throw InvalidPathException("Impossible d'ouvrir le fichier script " + currentDir + "\\" + scriptData.name);
			}
		} else {
			validPath = scriptData.name;
		}

		if (validPath.empty()) {
			throw InvalidPathException("Le script de nom " + scriptData.name + " est introuvable");
		}

		sc.active = 0;
		sc.parent = this;
		sc.fullPath = validPath;
		sc.key = keyScript;
		sc.origin = origin;

		std::ifstream scriptFile(sc.fullPath);
		if (scriptFile.fail()) {
			throw InvalidPathException("Impossible d'ouvrir le fichier script " + sc.fullPath);
		}

		for (std::string line; getline(scriptFile, line);) {
			sc.file.push_back(line);
		}

		m_cache.insert(make_pair(sc.fullPath, sc));
	} else {
		sc = m_cache[validPath];
	}

	sc.extendedName = extendedName;
	sc.scriptPeriod = scriptData.period == 0 ? 1 : scriptData.period;
	sc.extraArgs = scriptData.args;
	sc.context = scriptData.context;
	sc.triggeringType = EnumScriptTriggerType::AUTO;
	sc.entityId = scriptSleepEntity;
	sc.deleteEntityWhenFinished = scriptData.deleteEntityWhenFinished;

	/* Setup next args for the future script */
	unsigned int i = 0;
	for (const auto& curArg : sc.extraArgs) {
		ScriptUtils::setValueFromVarOrSwitchNumber(m_saveGame, sc.extendedName, "#arg" + StringUtils::intToStr(i) + "#", curArg, sc.varMap);
		i++;
	}

	m_componentAccessor.add<ScriptComponent>(scriptSleepEntity, sc);
}

void ska::ScriptAutoSystem::registerNamedScriptedEntity(const std::string& nameEntity, const EntityId entity) {
	m_namedScriptedEntities[nameEntity] = entity;
}

void ska::ScriptAutoSystem::clearNamedScriptedEntities() {
	m_namedScriptedEntities.clear();
}

ska::EntityId ska::ScriptAutoSystem::getEntityFromName(const std::string& nameEntity) {
	return m_namedScriptedEntities[nameEntity];
}

bool ska::ScriptAutoSystem::eof(ScriptComponent& script) {
	return script.file.size() <= script.currentLine;
}

ska::Savegame& ska::ScriptAutoSystem::getSavegame() {
	return m_saveGame;
}

void ska::ScriptAutoSystem::refresh(unsigned int) {
	auto nextScript = getHighestPriorityScript();
	if (nextScript == nullptr) {
		return;
	}

	try {
		play(*nextScript, m_saveGame);
	} catch (ScriptDiedException sde) {
		const std::string& entityScriptId = sde.getScript();
		if (entityScriptId.empty()) {
			killAndSave(*nextScript, m_saveGame);
		} else {
			if (StringUtils::isInt(entityScriptId, 10)) {
				EntityId scriptEntity = StringUtils::strToInt(entityScriptId);
				const auto& scriptCPtr = m_componentPossibleAccessor.get<ScriptComponent>(scriptEntity);
				if (scriptCPtr == nullptr) {
					SKA_LOG_ERROR("ERREUR SCRIPT [", nextScript->extendedName, "] (l.", nextScript->currentLine, ") ", sde.what(), " Script not found with id : ", entityScriptId);
				} else {
					killAndSave(*scriptCPtr, m_saveGame);
				}
			} else {
				SKA_LOG_ERROR("ERREUR SCRIPT [", nextScript->extendedName, "] (l.", nextScript->currentLine, ") ", sde.what(), " This is not an integer id : ", entityScriptId);
			}
		}

	} catch (ScriptException e) {
		SKA_LOG_ERROR("ERREUR SCRIPT [", nextScript->extendedName, "] (l.", nextScript->currentLine, ") ", e.what());
	}

}

void ska::ScriptAutoSystem::killAndSave(ScriptComponent& script, const Savegame&) const {
	//string& tmpScritFileName = ("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Saves" FILE_SEPARATOR + savegame.getSaveName() + FILE_SEPARATOR "tmpscripts.data");
	//std::ofstream scriptList;
	/*scriptList.open(tmpScritFileName.c_str(), ios::app);
	if (!scriptList.fail()) {
		scriptList << script.extendedName << endl;
		scriptList.close();
	}*/

	if (!script.deleteEntityWhenFinished) {
		m_componentAccessor.remove<ScriptComponent>(script.entityId);
	} else {
		removeEntity(script.entityId);
	}
	script.state = EnumScriptState::DEAD;
}

ska::ScriptComponent* ska::ScriptAutoSystem::getHighestPriorityScript() {
	float maxPriorityScriptValue = -1;
	ScriptComponent* maxPriorityScript = nullptr;
	auto currentTimeTicks = TimeUtils::getTicks();

	const auto& processed = getEntities();
	for (auto entityId : processed) {
		auto& script = m_componentAccessor.get<ScriptComponent>(entityId);
		auto currentVal = getPriority(script, currentTimeTicks);
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
	auto cannotBePlayed =
		EnumScriptState::RUNNING == script.state
		|| EnumScriptState::DEAD == script.state
		|| script.active > 0
		|| (TimeUtils::getTicks() - script.lastTimeDelayed) <= script.delay
		|| !((script.triggeringType == EnumScriptTriggerType::AUTO && script.state == EnumScriptState::STOPPED) || (script.state != EnumScriptState::STOPPED))
		|| eof(script);

	return !cannotBePlayed;
}


/* When possible, transfers the value of m_active containing a time to wait to m_delay */
bool ska::ScriptAutoSystem::transferActiveToDelay(ScriptComponent& script) {
	if (script.active > 1) {
		script.state = EnumScriptState::PAUSED;
		script.delay = script.active;
		script.lastTimeDelayed = TimeUtils::getTicks();
		script.active = 0;
		return true;
	}
	return false;
}

bool ska::ScriptAutoSystem::play(ScriptComponent& script, Savegame& savegame) {

	if (!canBePlayed(script)) {
		return false;
	}

	script.lastTimeStarted = TimeUtils::getTicks();

	/* Update status once */
	if (manageCurrentState(script) == EnumScriptState::PAUSED) {
		return false;
	}

	script.parent = this;

	/* Read commands */
	while (!eof(script)) {
		const auto cmd = nextLine(script);
		if (cmd != "") {
			script.lastResult = interpret(script, savegame, cmd);
			/* We need to "manageCurrentState" to keep a valid state for the script at each command except the last one (when scriptStop is true) */
			if (script.state == EnumScriptState::STOPPED || manageCurrentState(script) == EnumScriptState::PAUSED) {
				break;
			}
			script.commandsPlayed++;
		}
	}


	/*  If loop is exited it means that it's terminated or the script is stopped/paused.
	That's why we rewind m_fscript.
	(with a paused script here, it means that the script will resume next time we play it) */
	if (eof(script)) {
		script.state = EnumScriptState::STOPPED;
		/* If the script is terminated and triggering is not automatic, then we don't reload the script */
		if (script.triggeringType == EnumScriptTriggerType::AUTO) {
			script.currentLine = 0;
			/*script.fscript.clear();
			script.fscript.seekg(0, std::ios::beg);*/
		}
		script.commandsPlayed = 0;
	}

	return true;
}

std::string ska::ScriptAutoSystem::interpret(ScriptComponent& script, Savegame&, const std::string& cmd) {
	std::string cmdName;
	std::stringstream streamCmd;

	streamCmd << cmd;
	streamCmd >> cmdName;
	/* No tabulation */
	remove(cmdName.begin(), cmdName.end(), '\t');

	if (cmdName.empty()) {
		if (streamCmd.eof()) {
			stop(script);
		}
		return "";
	}

	if (m_commands.find(cmdName) != m_commands.end()) {
		//auto& varMap = script.varMap;
		try {
			return m_commands[cmdName]->process(*this, script, streamCmd);
		} catch (const NumberFormatException& nfe) {
			throw ScriptException("Commande " + cmdName + " : " + std::string(nfe.what()));
		}
	}
	else {
		throw ScriptUnknownCommandException("Impossible de trouver la commande " + cmdName + " dans le moteur de scripts.");
	}
}

void ska::ScriptAutoSystem::registerCommand(const std::string& cmdName, CommandPtr& cmd) {
	m_commands[cmdName] = move(cmd);
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
		return NumberUtils::exponential(coeff);
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
	} else if (script.state == EnumScriptState::PAUSED && TimeUtils::getTicks() - script.lastTimeDelayed  > script.delay) {
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
	script.triggeringType = EnumScriptTriggerType::NONE;
}

ska::ScriptAutoSystem::~ScriptAutoSystem()
{
}
