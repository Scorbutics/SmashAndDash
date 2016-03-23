#include "ScriptAutoSystem.h"
#include "../Exceptions/ScriptDiedException.h"
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


ska::ScriptAutoSystem::ScriptAutoSystem()
{
}


ska::ScriptAutoSystem::ScriptAutoSystem(ska::Savegame& saveGame) : m_saveGame(saveGame){

}



ska::IScript* ska::ScriptAutoSystem::addRunningScript(IScript* parent, const string& name, const string& context, const vector<string>& args, const int triggeringType, const unsigned int* period) {
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

	}
	else {
		validPath = name;
	}



	if (m_scripts.find(keyScript) == m_scripts.end() || m_scripts[keyScript]->getCurrentState() == EnumScriptState::STOPPED) {
		if (!validPath.empty()) {
			m_scripts[keyScript] = (move(ScriptPtr(new Script(*this, triggeringType, period == NULL || *period == 0 ? SCRIPT_DEFAULT_PERIOD : *period, validPath, extendedName, context, keyScript, args))));
			ska::ScriptDispatcher::setupScriptArgs(parent, m_scripts[keyScript].get(), args);
		}
		else {
			throw ska::InvalidPathException("Le script de nom " + name + " est introuvable");
		}
		return m_scripts[keyScript].get();
	}
	else {
		return NULL;
	}


}

ska::Savegame& ska::ScriptAutoSystem::getSavegame() {
	return m_saveGame;
}

void ska::ScriptAutoSystem::setupScriptArgs(IScript* parent, IScript* script, const vector<string>& args) {
	unsigned int i = 0;
	for (const string& curArg : args) {
		ScriptUtils::setValueFromVarOrSwitchNumber(m_saveGame, script->getExtendedName(), "#arg" + ska::StringUtils::intToStr(i) + "#", curArg, script->getVarMap());
		i++;
	}
}

void ska::ScriptAutoSystem::refresh()
{
	IScript* nextScript = getHighestPriorityScript();
	if (nextScript == NULL) {
		return;
	}

	try {
		nextScript->play(m_saveGame);
	}
	catch (ska::ScriptDiedException sde) {
		const std::string& scriptName = sde.getScript();
		if (scriptName.empty()) {
			nextScript->killAndSave(m_saveGame);
		}
		else {
			if (m_scripts.find(scriptName) == m_scripts.end()) {
				cerr << "ERREUR SCRIPT [" << nextScript->getExtendedName() << "] (l." << nextScript->getCurrentLine() << ") " << sde.what() << " Script not found : " << scriptName << endl;
			}
			else {
				m_scripts[scriptName]->killAndSave(m_saveGame);
			}
		}

	}
	catch (ska::ScriptException e) {
		cerr << "ERREUR SCRIPT [" << nextScript->getExtendedName() << "] (l." << nextScript->getCurrentLine() << ") " << e.what() << endl;
	}

}

std::string ska::ScriptAutoSystem::commandInterpreter(IScript* script, const std::string& cmd) {

	return "";
}

void ska::ScriptAutoSystem::clear() {
	if (!m_scripts.empty()) {
		m_scripts.clear();
	}
}

ska::IScript* ska::ScriptAutoSystem::getHighestPriorityScript()
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

ska::ScriptAutoSystem::~ScriptAutoSystem()
{
}
