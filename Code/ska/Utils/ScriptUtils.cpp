#include <sstream>
#include "ScriptUtils.h"
#include "FormalCalculation\FormalCalculator.h"
#include "../Script/ScriptSymbolsConstants.h"
#include "StringUtils.h"
#include "SkaConstants.h"
#include "../Script/System/ScriptAutoSystem.h"
#include "../Script/ScriptDispatcher.h"
#include "../Exceptions/NumberFormatException.h"
#include "../Exceptions/ScriptSyntaxError.h"

using namespace std;

ska::ScriptUtils::ScriptUtils()
{
}

/* R�cup�re la valeur une variable LOCALE (dans varMap) */
string ska::ScriptUtils::getValueFromVarOrSwitchNumber(const ska::Savegame& saveGame, const ScriptComponent& script, string varNumber)
{
	int num = -1;
	//WGameCore& wScreen = WGameCore::getInstance();

	if (varNumber[0] == '{' && varNumber[varNumber.size() - 1] == '}')
	{
		if (saveGame.getGameSwitch(ska::StringUtils::strToInt(varNumber.substr(1, varNumber.size() - 2)) - 1)) {
			return "1";
		} else {
			return "0";
		}
	}
	else if (varNumber[0] == '[' && varNumber[varNumber.size() - 1] == ']')
	{
		string v = varNumber.substr(1, varNumber.size() - 2);
		string key = getVariableKey(v);
		if (!key.empty())
		{
			if (script.varMap.find(key) != script.varMap.end()) {
				return script.varMap.at(key);
			} else {
				return "";
			}
				
		}
		else {
			unsigned int varNum;
			try {
				varNum = ska::StringUtils::strToInt(varNumber.substr(1, varNumber.size() - 2));
			}
			catch (ska::NumberFormatException nfe) {
				return interpretVarName(saveGame, script, varNumber.substr(1, varNumber.size() - 2));
			}

			return ska::StringUtils::intToStr(saveGame.getGameVariable(varNum - 1));
		}
			
	} else if (varNumber[0] == '#' && varNumber[varNumber.size() - 1] == '#') {
		const std::string& key = varNumber + script.extendedName;
		if (script.varMap.find(key) != script.varMap.end()) {
			return script.varMap.at(key);
		}
		else {
			return "";
		}
	}
	else if (varNumber == "true") {
		return "1";
	} else if (varNumber == "false") {
		return "0";
	}
	
	return varNumber;
}

string ska::ScriptUtils::replaceVariablesByNumerics(const ska::Savegame& saveGame, const ScriptComponent& script, const string& line, char varStartSymbol, char varEndSymbol)
{
	string it = line;
	size_t posLeft, posRight;

	while ((posLeft = it.find_first_of(varStartSymbol)) != string::npos)
	{
		if ((posRight = it.substr(posLeft+1).find_first_of(varEndSymbol)) != string::npos)
		{
			posRight += posLeft + 1;

			string var = it.substr(posLeft, posRight - posLeft + 1);
			string varValue = ScriptUtils::getValueFromVarOrSwitchNumber(saveGame, script, var);

			it = it.substr(0, posLeft) + varValue + it.substr(posRight + 1, it.size());
		}
		else
		{
			FormalCalculator::calculSyntaxError(line);
			return "";
		}
	}

	return it;

}

string ska::ScriptUtils::replaceVariablesByNumerics(const ska::Savegame& saveGame, const ScriptComponent& script, const string& line)
{
	string& it = replaceVariablesByNumerics(saveGame, script, line, ScriptSymbolsConstants::VARIABLE_LEFT, ScriptSymbolsConstants::VARIABLE_RIGHT);
	it = replaceVariablesByNumerics(saveGame, script, it, ScriptSymbolsConstants::ARG, ScriptSymbolsConstants::ARG);
	return it;
}

/*
Retourne la premi�re expression sur "line" apr�s calculs et formattage
Ex : si line = " [|bidule|] %random 100% [|chance|] %truc 200% " , la fonction va calculer "random 100" et renvoyer le r�sultat de ce calcul.
Ex : si line = " [|bidule|] %random %truc 200%% [|chance|]" , la fonction va calculer "random %truc 200%", lui-m�me va rappeler cette fonction et renvoyer le r�sultat de ce calcul total.
*/
std::string ska::ScriptUtils::getFirstExpressionFromLine(ScriptAutoSystem& system, const std::string& line, ScriptComponent& script, size_t* outputCommandSize)
{
	size_t indexFirstChar;
	for (indexFirstChar = 0; line[indexFirstChar] != ScriptSymbolsConstants::METHOD && line[indexFirstChar] != '\n' && indexFirstChar < line.size(); indexFirstChar++);

	if (line[indexFirstChar] == '\n' || line.size() <= indexFirstChar) {
		if (outputCommandSize != NULL) {
			*outputCommandSize = 0;
		}
		return line;
	}
		

	const string& formattedLine = line.substr(indexFirstChar, line.size());
	const string& commandCall = ScriptUtils::getCommandCall(formattedLine);
	string valeur;

	if (outputCommandSize != NULL) {
		if (!commandCall.empty()) {
			*outputCommandSize = commandCall.size() + indexFirstChar + 2; //2 = les deux symboles '%' de la commande
		} else {
			*outputCommandSize = 0;
		}
	}

	if (!commandCall.empty()) {
		std::string result = system.interpret(script, system.getSavegame(), commandCall);
		if (!result.empty()) {
			valeur = result;
		} else {
			valeur = formattedLine.substr(0, formattedLine.find_first_of(' '));
		}
	} else {
		valeur = formattedLine.substr(0, formattedLine.find_first_of(' '));
	}

	return valeur;
}

string ska::ScriptUtils::getCommandCall(const string& s)
{
	if (!s.empty() && s[0] == ScriptSymbolsConstants::METHOD)
	{
		size_t posEndSubCmd = s.find_last_of(ScriptSymbolsConstants::METHOD);
		if (posEndSubCmd != std::string::npos)
			return s.substr(1, posEndSubCmd - 1);
	}

	return "";
}

string ska::ScriptUtils::getVariableKey(const string& v)
{
	size_t pipePos = v.find_first_of('|');
	if (pipePos == 0 && v.find_last_of('|') == v.size() - 1)
	{
		//variable temporaire => varMap
		string key = "[" + v.substr(1, v.size() - 2) + "]";
		return key;
	}

	return "";
}

void ska::ScriptUtils::setValueFromVarOrSwitchNumber(ska::Savegame& saveGame, const string& scriptExtendedName, string varNumber, string value, std::unordered_map<std::string, std::string>& varMap)
{
	//WGameCore& wScreen = WGameCore::getInstance();

	if (value.empty())
		return;

	if (varNumber[0] == '{' && varNumber[varNumber.size() - 1] == '}')
	{
		if (value == "true")
			saveGame.setGameSwitch(atoi(varNumber.substr(1, varNumber.size() - 2).c_str()) - 1, true);
		else
			saveGame.setGameSwitch(atoi(varNumber.substr(1, varNumber.size() - 2).c_str()) - 1, false);
	}
	else if (varNumber[0] == '[' && varNumber[varNumber.size() - 1] == ']')
	{
		string v = varNumber.substr(1, varNumber.size() - 2);
		string key = getVariableKey(v);
		if (!key.empty())
		{
			varMap[key] = value;
			return;
		}

	}
	else if (varNumber[0] == '#' && varNumber[varNumber.size() - 1] == '#') {
		varMap[varNumber + scriptExtendedName] = value;
	}
	
	saveGame.setGameVariable(atoi(varNumber.substr(1, varNumber.size() - 2).c_str()) - 1, atoi(value.c_str()));

}

/* R�cup�re la valeur d'une variable GLOBALE en utilisant potentiellement des sous-variables locales en param�tres */
std::string ska::ScriptUtils::interpretVarName(const ska::Savegame& saveGame, const ScriptComponent& script, std::string& v)
{
	/*
	_variable_ : variable "constante" (int�gr�e au jeu)
	_variable param1 param2_ : variable int�gr�e au jeu avec param�tres
	#variable# : num�ro d'argument de script en cours entre symboles di�se
	|variable| : variable utilisateur (cr��e en script et utilis�e en script, morte � la fin du script)
	*/

	stringstream ss;
	string cmds[2];

	if (v[0] == '_' && v[v.size()-1] == '_')
	{
		ss << v.substr(1, v.size()-2);

		unsigned int i;
		for (i = 0; ss >> cmds[i] && i < 2; i++);

		if (!ss.eof()) {
			throw ska::ScriptSyntaxError("Error while interpreting global var (not enough arguments) : " + v);
		}

		return script.parent->map(cmds[0], getValueFromVarOrSwitchNumber(saveGame, script, cmds[1]));
	} 

	return getValueFromVarOrSwitchNumber(saveGame, script, v);
}

bool ska::ScriptUtils::isScriptActivated(const ska::Savegame& saveGame, const string& scriptName)
{
	string s;
	//WGameCore& wScreen = WGameCore::getInstance();
	ifstream scriptList(("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + saveGame.getSaveName() + FILE_SEPARATOR"scripts.data").c_str(), ios::app);
	ifstream tmpScriptList(("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + saveGame.getSaveName() + FILE_SEPARATOR"tmpscripts.data").c_str(), ios::app);

	if (scriptList.fail())
		return true;

	do
	scriptList >> s;
	while (s != scriptName && !scriptList.eof());


	if (s == scriptName)
		return false;
	else
	{
		s = "";
		if (tmpScriptList.fail())
			return true;

		do
		tmpScriptList >> s;
		while (s != scriptName && !tmpScriptList.eof());

		return !(s == scriptName);
	}
}

ska::ScriptUtils::~ScriptUtils()
{
}
