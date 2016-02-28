#include <algorithm>
#include <iostream>
#include "ScriptDispatcher.h"

#include "Command\CommandIf.h"
#include "Command\CommandElse.h"
#include "Command\CommandElseEnd.h"
#include "../Exceptions/ScriptDiedException.h"
/*
#include "../Gameplay\WGameCore.h"
#include "Commands\CommandMove.h"
#include "Commands\CommandMessage.h"
#include "Commands\CommandChoice.h"
#include "Commands\CommandEnd.h"
#include "Commands\CommandCalculate.h"
#include "Commands\CommandHeal.h"
#include "Commands\CommandStop.h"
#include "Commands\CommandEndScript.h"
#include "Commands\CommandAssign.h"
#include "Commands\CommandRandom.h"
#include "Commands\CommandDirection.h"
#include "Commands\CommandPlayAnimation.h"
#include "Commands\CommandPlayCrumbling.h"
#include "Commands\CommandPlayShaking.h"
#include "Commands\CommandTranslationCamera.h"
#include "Commands\CommandElsif.h"
#include "Commands\CommandWait.h"
#include "Commands\CommandStuckCharacter.h"
#include "Commands\CommandHideCharacter.h"
#include "Commands\CommandFollow.h"
#include "Commands\CommandShop.h"
#include "Commands\CommandHideGUI.h"
#include "Commands\CommandTeleport.h"
#include "Commands\CommandScript.h"
#include "Commands\CommandCinematic.h"
#include "Commands\CommandThrowPokebal.h"
#include "Commands\CommandKillEntity.h"
#include "Commands\CommandAddPokemon.h"
#include "Commands\CommandPokemonOut.h"
#include "Commands\CommandPlayerPresence.h"
#include "Commands\CommandLog.h"
*/
#include "../Exceptions\ScriptUnknownCommandException.h"
#include "../Utils\ScriptUtils.h"
#include "../Exceptions\InvalidPathException.h"
#include "../Exceptions/NumberFormatException.h"
#include "../Utils\StringUtils.h"
#include "../Utils/FileUtils.h"
#include "../Utils/TimeUtils.h"
#include "ScriptSymbolsConstants.h"

using namespace std;

//Par défaut, un script "permanent" se rafraîchit toutes les 1 ms
#define SCRIPT_DEFAULT_PERIOD 1


/*unordered_map<string, unique_ptr<ska::Command>> create_map()
{
	unordered_map<string, unique_ptr<ska::Command>> c;
	c["move"] = move(ska::CommandPtr(new CommandMove()));
	c["message"] = move(ska::CommandPtr(new CommandMessage()));
	c["choice"] = move(ska::CommandPtr(new CommandChoice()));
	c[CommandEnd::getCmdName()] = move(ska::CommandPtr(new CommandEnd()));
	c["end_script"] = move(ska::CommandPtr(new CommandEndScript()));
	c["calculate"] = move(ska::CommandPtr(new CommandCalculate()));
	c["heal"] = move(ska::CommandPtr(new CommandHeal()));
	c["stop"] = move(ska::CommandPtr(new CommandStop()));
	c["assign"] = move(ska::CommandPtr(new CommandAssign()));
	c["random"] = move(ska::CommandPtr(new CommandRandom()));
	c["direction"] = move(ska::CommandPtr(new CommandDirection()));
	c["animation"] = move(ska::CommandPtr(new CommandPlayAnimation()));
	c["crumbling"] = move(ska::CommandPtr(new CommandPlayCrumbling()));
	c["shaking"] = move(ska::CommandPtr(new CommandPlayShaking()));
	c["camera_translation"] = move(ska::CommandPtr(new CommandTranslationCamera()));
	c["wait"] = move(ska::CommandPtr(new  CommandWait()));
	c["stuck_char"] = move(ska::CommandPtr(new CommandStuckCharacter()));
	c["hide_char"] = move(ska::CommandPtr(new CommandHideCharacter()));
	c["follow_char"] = move(ska::CommandPtr(new CommandFollow()));
	c["shop"] = move(ska::CommandPtr(new CommandShop()));
	c["hide_gui"] = move(ska::CommandPtr(new CommandHideGUI()));
	c["teleport"] = move(ska::CommandPtr(new CommandTeleport()));
	c["script"] = move(ska::CommandPtr(new CommandScript()));
	c["cinematic"] = move(ska::CommandPtr(new CommandCinematic()));
	c["pokeball"] = move(ska::CommandPtr(new CommandThrowPokebal()));
	c["kill"] = move(ska::CommandPtr(new CommandKillEntity()));
	c["add_pokemon"] = move(ska::CommandPtr(new CommandAddPokemon()));
	c["throw_pokemon"] = move(ska::CommandPtr(new CommandPokemonOut()));
	c["player_presence"] = move(ska::CommandPtr(new CommandPlayerPresence()));
	c["log"] = move(ska::CommandPtr(new CommandLog()));
	

	return c;
}*/

ska::ScriptDispatcher::ScriptDispatcher(ska::Savegame& saveGame) : m_saveGame(saveGame){
	m_commands[ska::ControlStatement::getCommandIf()] = move(ska::CommandPtr(new CommandIf()));
	m_commands[ska::ControlStatement::getCommandElse()] = move(ska::CommandPtr(new CommandElse()));
	m_commands[ska::ControlStatement::getCommandEndIf()] = move(ska::CommandPtr(new CommandElseEnd()));
}

ska::IScript* ska::ScriptDispatcher::addRunningScript(IScript* parent, const string& name, const string& context, const vector<string>& args, const int triggeringType, const unsigned int* period) {
	string extendedName;
	ifstream fscript(name.c_str());
	string keyArgs;
	
	for (const string& arg : args) {
		keyArgs += arg + " ";
	}
	ska::StringUtils::rtrim(keyArgs);

	const string& keyScript = name + "/\\" + keyArgs;
	extendedName = keyScript + "_" + context;

	std::string validPath;
	if (fscript.fail()) {
		const std::string& currentDir = ska::FileUtils::getCurrentDirectory();
		validPath = (currentDir + "\\" + name);
		fscript.open(validPath.c_str());
		if (fscript.fail()) {
			throw ska::InvalidPathException("Impossible d'ouvrir le fichier script " + currentDir + "\\" + name);
		}

	} else {
		validPath = name;
	}

	

	if (m_scripts.find(keyScript) == m_scripts.end() || m_scripts[keyScript]->getCurrentState() == EnumScriptState::STOPPED) {
		if (!validPath.empty()) {
			m_scripts[keyScript] = (move(ScriptPtr(new Script(triggeringType, period == NULL || *period == 0? SCRIPT_DEFAULT_PERIOD : *period, validPath, extendedName, keyScript, args))));
			ska::ScriptDispatcher::setupScriptArgs(parent, m_scripts[keyScript].get(), args);
		} else {
			throw ska::InvalidPathException("Le script de nom " + name + " est introuvable");
		}
		return m_scripts[keyScript].get();
	} else {
		return NULL;
	}
	
	
}

void ska::ScriptDispatcher::addCommand(const std::string& key, ska::CommandPtr& cmd) {
	m_commands[key] = move(cmd);
}

void ska::ScriptDispatcher::setupScriptArgs(IScript* parent, IScript* script, const vector<string>& args) {
	unsigned int i = 0;
	for (const string& curArg : args) {
		ScriptUtils::setValueFromVarOrSwitchNumber(m_saveGame, script->getExtendedName(), "#arg" + ska::StringUtils::intToStr(i) + "#", curArg, script->getVarMap());
		i++;
	}
}

void ska::ScriptDispatcher::refresh()
{
	IScript* nextScript = getHighestPriorityScript();
	if (nextScript == NULL) {
		return;
	}

	try {
		nextScript->play();
	} catch (ska::ScriptException e) {
		cerr << "ERREUR SCRIPT [" << nextScript->getExtendedName() << "] (l." << nextScript->getCurrentLine() << ") " << e.what() << endl;
	}

}

std::string ska::ScriptDispatcher::commandInterpreter(IScript* script, const std::string& cmd) {
	ofstream scriptList;
	string cmdName;
	stringstream streamCmd;

	streamCmd << cmd;
	streamCmd >> cmdName;
	/* No tabulation */
	std::remove(cmdName.begin(), cmdName.end(), '\t');

	if (cmdName.empty()) {
		if (streamCmd.eof()) {
			script->stop();
		}
		return "";
	}

	if (m_commands.find(cmdName) != m_commands.end()) {
		std::unordered_map<std::string, std::string>& varMap = script->getVarMap();
		try {
			return m_commands[cmdName]->process(script, streamCmd, scriptList);
		}
		catch (ska::ScriptDiedException sde) {
			script->kill(m_saveGame);
		} catch (ska::NumberFormatException nfe) {
			throw ska::ScriptException("Commande " + cmdName + " : " + std::string(nfe.what()));
		}
	} else {
		throw ska::ScriptUnknownCommandException("Impossible de trouver la commande " + cmdName + " dans le moteur de scripts.");
	}
	return "";
}

void ska::ScriptDispatcher::clear() {
	if (!m_scripts.empty()) {
		m_scripts.clear();
	}
}

ska::IScript* ska::ScriptDispatcher::getHighestPriorityScript()
{
	float maxPriorityScriptValue = -1;
	IScript* maxPriorityScript = NULL;
	unsigned int currentTimeTicks = ska::TimeUtils::getTicks();

	for (auto& script : m_scripts) {
		float currentVal = script.second->getPriority(currentTimeTicks);
		if (maxPriorityScriptValue < currentVal) {
			maxPriorityScriptValue = currentVal;
			maxPriorityScript = script.second.get();
		}
	}

	/* maxPriorityScriptValue < 0 means no script in an "OK state" found */
	return maxPriorityScriptValue > 0 ? maxPriorityScript : NULL;
}


ska::ScriptDispatcher::~ScriptDispatcher()
{
}
