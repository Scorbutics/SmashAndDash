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
#include "Commands\CommandAssign.h"
#include "Commands\CommandRandom.h"
#include "Commands\CommandDirection.h"
#include "Commands\CommandPlayAnimation.h"
#include "Commands\CommandPlayCrumbling.h"
#include "Commands\CommandPlayShaking.h"
#include "Commands\CommandTranslationCamera.h"
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
#include "Commands\CommandGet.h"
#include "Commands\CommandElseEnd.h"
#include "Commands\CommandPlayerPresence.h"
#include "Commands\CommandLog.h"

using namespace std;

//Par défaut, un script "permanent" se rafraîchit toutes les 1 ms
#define SCRIPT_DEFAULT_PERIOD 1


unordered_map<string, unique_ptr<Command>> create_map()
{
	unordered_map<string, unique_ptr<Command>> c;
	c["bouger"] = move(Command_ptr(new CommandMove()));
	c["message"] = move(Command_ptr(new CommandMessage()));
	c["choix"] = move(Command_ptr(new CommandChoice()));
	c["fin"] = move(Command_ptr(new CommandEnd()));
	c["calculer"] = move(Command_ptr(new CommandCalculate()));
	c["soigner"] = move(Command_ptr(new CommandHeal()));
	c["stop"] = move(Command_ptr(new CommandStop()));
	c["si"] = move(Command_ptr(new CommandIf()));
	c["sinon"] = move(Command_ptr(new CommandElse()));
	c["finsi"] = move(Command_ptr(new CommandElseEnd()));
	c["affecter"] = move(Command_ptr(new CommandAssign()));
	c["random"] = move(Command_ptr(new CommandRandom()));
	c["obtenir"] = move(Command_ptr(new CommandGet()));
	c["direction"] = move(Command_ptr(new CommandDirection()));
	c["animation"] = move(Command_ptr(new CommandPlayAnimation()));
	c["eboulement"] = move(Command_ptr(new CommandPlayCrumbling()));
	c["tremblement"] = move(Command_ptr(new CommandPlayShaking()));
	c["translation_camera"] = move(Command_ptr(new CommandTranslationCamera()));
	c["attendre"] = move(Command_ptr(new  CommandWait()));
	c["bloquer_perso"] = move(Command_ptr(new CommandStuckCharacter()));
	c["cacher_perso"] = move(Command_ptr(new CommandHideCharacter()));
	c["suivre_perso"] = move(Command_ptr(new CommandFollow()));
	c["shop"] = move(Command_ptr(new CommandShop()));
	c["cacher_interface"] = move(Command_ptr(new CommandHideGUI()));
	c["teleporter"] = move(Command_ptr(new CommandTeleport()));
	c["script"] = move(Command_ptr(new CommandScript()));
	c["cinematique"] = move(Command_ptr(new CommandCinematic()));
	c["lancer_pokeball"] = move(Command_ptr(new CommandThrowPokebal()));
	c["tuer"] = move(Command_ptr(new CommandKillEntity()));
	c["ajouter_pokemon"] = move(Command_ptr(new CommandAddPokemon()));
	c["sortir_pokemon"] = move(Command_ptr(new CommandPokemonOut()));
	c["presence_joueur"] = move(Command_ptr(new CommandPlayerPresence()));
	c["log"] = move(Command_ptr(new CommandLog()));
	return c;
}

unordered_map<string, unique_ptr<Command>> ScriptDispatcher::commands = create_map();

ScriptDispatcher::ScriptDispatcher()
{
}

int ScriptDispatcher::addRunningScript(string name, string args, int triggeringType, Uint32* period)
{
	WGameCore& wScreen = WGameCore::getInstance();
	World& w = wScreen.getWorld();
	string extendedName;
	ifstream fscript(name.c_str());
	const string& keyScript = name + "/\\" + args;
	extendedName = keyScript + "_" + wScreen.getWorld().getName();
	

	std::string validPath;
	if (fscript.fail())
	{
		TCHAR NPath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, NPath);
		validPath = (string(NPath) + "\\" + name);
		fscript.open(validPath.c_str());
		if (fscript.fail()) {
			cerr << "ERREUR : Impossible d'ouvrir " + string(NPath) + "\\" + name << endl;
			fscript.close();
			return 1;
		}

	}
	else {
		validPath = name;
	}

	

	if (m_scripts.find(keyScript) == m_scripts.end() || m_scripts[keyScript]->getCurrentState() == EnumScriptState::STOPPED) {
		if (!validPath.empty()) {
			m_scripts[keyScript] = (move(Script_ptr(new Script(triggeringType, period == NULL || *period == 0? SCRIPT_DEFAULT_PERIOD : *period, validPath, extendedName, args))));
			ScriptDispatcher::setupScriptArgs(m_scripts[keyScript].get(), extendedName, args);
		}
		else {
			throw InvalidPathException("Le script de nom " + name + " est introuvable");
		}
		return 0;
	} else {
		return 1;
	}
	
	
}

void ScriptDispatcher::setupScriptArgs(IScript* script, const string& extendedName, string& args) {
	unsigned int i = 0;
	stringstream ss;
	string curArg;
	ss << args;
	while (ss >> curArg) {
		ScriptUtils::setValueFromVarOrSwitchNumber(extendedName, "#arg" + StringUtils::intToStr(i) + "#", curArg, script->getVarMap());
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
		cerr << "Erreur survenue lors de l'execution du script : " << e.what() << endl;
	}

}

bool ScriptDispatcher::commandInterpreter(const std::string& extendedName, const std::string& cmd, std::ifstream& fscript, std::unordered_map<std::string, std::string>& varMap, int& active, std::string* result)
{
	WGameCore& wScreen = WGameCore::getInstance();
	ofstream scriptList;
	string cmdName;
	stringstream streamCmd;

	if (result != NULL) {
		*result = "";
	}

	streamCmd << cmd;
	streamCmd >> cmdName;
	/* No tabulation */
	std::remove(cmdName.begin(), cmdName.end(), '\t');

	if (cmdName.empty()) {
		return false;
	}

	if (commands.find(cmdName) != commands.end()) {
		try {
			return commands[cmdName]->process(extendedName, streamCmd, scriptList, varMap, fscript, active, result);
		} catch (NumberFormatException nfe) {
			throw ScriptException("Script " + extendedName + " : Une erreur est survenue à l'exécution de la commande " + cmdName + " dans le moteur de scripts : " + nfe.what());
		}
	} else {
		throw ScriptUnknownCommandException("Script " + extendedName + " : Impossible de trouver la commande " + cmdName + " dans le moteur de scripts.");
	}

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
