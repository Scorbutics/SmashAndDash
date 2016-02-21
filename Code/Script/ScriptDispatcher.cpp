#include <algorithm>
#include "ScriptDispatcher.h"
#include "../Gameplay\WGameCore.h"
#include "Commands\CommandMove.h"
#include "Commands\CommandMessage.h"
#include "Commands\CommandChoice.h"
#include "Commands\CommandEnd.h"
#include "Commands\CommandCalculate.h"
#include "Commands\CommandHeal.h"
#include "Commands\CommandStop.h"
#include "Commands\CommandIf.h"
#include "Commands\CommandElse.h"
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
#include "../Exceptions\ScriptUnknownCommandException.h"
#include "../Utils\ScriptUtils.h"
#include "../Exceptions\InvalidPathException.h"
#include "../Exceptions/NumberFormatException.h"
#include "../Utils\StringUtils.h"
#include "Commands\CommandElseEnd.h"
#include "Commands\CommandPlayerPresence.h"
#include "Commands\CommandLog.h"
#include "ScriptSymbolsConstants.h"

using namespace std;

//Par défaut, un script "permanent" se rafraîchit toutes les 1 ms
#define SCRIPT_DEFAULT_PERIOD 1


unordered_map<string, unique_ptr<Command>> create_map()
{
	unordered_map<string, unique_ptr<Command>> c;
	c["move"] = move(Command_ptr(new CommandMove()));
	c["message"] = move(Command_ptr(new CommandMessage()));
	c["choice"] = move(Command_ptr(new CommandChoice()));
	c[CommandEnd::getCmdName()] = move(Command_ptr(new CommandEnd()));
	c["end_script"] = move(Command_ptr(new CommandEndScript()));
	c["calculate"] = move(Command_ptr(new CommandCalculate()));
	c["heal"] = move(Command_ptr(new CommandHeal()));
	c["stop"] = move(Command_ptr(new CommandStop()));
	c["assign"] = move(Command_ptr(new CommandAssign()));
	c["random"] = move(Command_ptr(new CommandRandom()));
	c["direction"] = move(Command_ptr(new CommandDirection()));
	c["animation"] = move(Command_ptr(new CommandPlayAnimation()));
	c["crumbling"] = move(Command_ptr(new CommandPlayCrumbling()));
	c["shaking"] = move(Command_ptr(new CommandPlayShaking()));
	c["camera_translation"] = move(Command_ptr(new CommandTranslationCamera()));
	c["wait"] = move(Command_ptr(new  CommandWait()));
	c["stuck_char"] = move(Command_ptr(new CommandStuckCharacter()));
	c["hide_char"] = move(Command_ptr(new CommandHideCharacter()));
	c["follow_char"] = move(Command_ptr(new CommandFollow()));
	c["shop"] = move(Command_ptr(new CommandShop()));
	c["hide_gui"] = move(Command_ptr(new CommandHideGUI()));
	c["teleport"] = move(Command_ptr(new CommandTeleport()));
	c["script"] = move(Command_ptr(new CommandScript()));
	c["cinematic"] = move(Command_ptr(new CommandCinematic()));
	c["pokeball"] = move(Command_ptr(new CommandThrowPokebal()));
	c["kill"] = move(Command_ptr(new CommandKillEntity()));
	c["add_pokemon"] = move(Command_ptr(new CommandAddPokemon()));
	c["throw_pokemon"] = move(Command_ptr(new CommandPokemonOut()));
	c["player_presence"] = move(Command_ptr(new CommandPlayerPresence()));
	c["log"] = move(Command_ptr(new CommandLog()));

	c[ControlStatement::getCommandIf()] = move(Command_ptr(new CommandIf()));
	c[ControlStatement::getCommandElse()] = move(Command_ptr(new CommandElse()));
	//c[ControlStatement::getCommandElsif()] = move(Command_ptr(new CommandElsif()));
	c[ControlStatement::getCommandEndIf()] = move(Command_ptr(new CommandElseEnd()));
	return c;
}

unordered_map<string, unique_ptr<Command>> ScriptDispatcher::m_commands = create_map();

ScriptDispatcher::ScriptDispatcher()
{
}

IScript* ScriptDispatcher::addRunningScript(IScript* parent, string name, string args, int triggeringType, Uint32* period)
{
	WGameCore& wScreen = WGameCore::getInstance();
	World& w = wScreen.getWorld();
	string extendedName;
	ifstream fscript(name.c_str());
	
	/* The key musn't be considered as several arguments when passed to a command.
		Therefore, we need to replace each argument separator by something that's not part of the script syntax */
	string keyArgs = args;
	StringUtils::replaceAll(keyArgs, ScriptSymbolsConstants::ARGUMENT_SEPARATOR, ' ');

	const string& keyScript = name + "/\\" + keyArgs;
	extendedName = keyScript + "_" + wScreen.getWorld().getName();

	std::string validPath;
	if (fscript.fail()) {
		TCHAR NPath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, NPath);
		validPath = (string(NPath) + "\\" + name);
		fscript.open(validPath.c_str());
		if (fscript.fail()) {
			throw InvalidPathException("Impossible d'ouvrir le fichier script " + string(NPath) + "\\" + name);
		}

	} else {
		validPath = name;
	}

	

	if (m_scripts.find(keyScript) == m_scripts.end() || m_scripts[keyScript]->getCurrentState() == EnumScriptState::STOPPED) {
		if (!validPath.empty()) {
			m_scripts[keyScript] = (move(Script_ptr(new Script(triggeringType, period == NULL || *period == 0? SCRIPT_DEFAULT_PERIOD : *period, validPath, extendedName, keyScript, args))));
			ScriptDispatcher::setupScriptArgs(parent, m_scripts[keyScript].get(), args);
		} else {
			throw InvalidPathException("Le script de nom " + name + " est introuvable");
		}
		return m_scripts[keyScript].get();
	} else {
		return NULL;
	}
	
	
}

void ScriptDispatcher::kill(const std::string& keyScript) {
	if (m_scripts.find(keyScript) != m_scripts.end()) {
		commandInterpreter(m_scripts[keyScript].get(), CommandEnd::getCmdName());
	} else {
		throw InvalidPathException("Le script de clé " + keyScript + " est introuvable");
	}
}

void ScriptDispatcher::setupScriptArgs(IScript* parent, IScript* script, string& args) {
	unsigned int i = 0;
	vector<string> argsV = StringUtils::split(args, ScriptSymbolsConstants::ARGUMENT_SEPARATOR);
	for (string& curArg : argsV) {
		ScriptUtils::setValueFromVarOrSwitchNumber(script->getExtendedName(), "#arg" + StringUtils::intToStr(i) + "#", curArg, script->getVarMap());
		i++;
	}
}

void ScriptDispatcher::refresh()
{
	IScript* nextScript = getHighestPriorityScript();
	if (nextScript == NULL) {
		return;
	}

	try {
		nextScript->play();
	}
	catch (ScriptException e) {
		cerr << "ERREUR SCRIPT [" << nextScript->getExtendedName() << "] (l." << nextScript->getCurrentLine() << ") " << e.what() << endl;
	}

}

std::string ScriptDispatcher::commandInterpreter(IScript* script, const std::string& cmd)
{
	WGameCore& wScreen = WGameCore::getInstance();
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
		} catch (NumberFormatException nfe) {
			throw ScriptException("Commande " + cmdName + " : " + std::string(nfe.what()));
		}
	} else {
		throw ScriptUnknownCommandException("Impossible de trouver la commande " + cmdName + " dans le moteur de scripts.");
	}
	return "";
}

void ScriptDispatcher::clear() {
	if (!m_scripts.empty()) {
		m_scripts.clear();
	}
}

IScript* ScriptDispatcher::getHighestPriorityScript()
{
	float maxPriorityScriptValue = -1;
	IScript* maxPriorityScript = NULL;
	Uint32 currentTimeTicks = SDL_GetTicks();

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


ScriptDispatcher::~ScriptDispatcher()
{
}
