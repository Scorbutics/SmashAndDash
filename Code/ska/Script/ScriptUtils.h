#pragma once
#include <string>
#include <unordered_map>
#include "../Script/ScriptComponent.h"


namespace ska {
	class Savegame;
	class ScriptAutoSystem;
	class ScriptUtils
	{
	public:
		ScriptUtils();
		static std::string getValueFromVarOrSwitchNumber(const Savegame& saveGame, const ScriptComponent& script, std::string varNumber);
		static std::string replaceVariablesByNumerics(const Savegame& saveGame, const ScriptComponent& script, const std::string& line);
		static std::string replaceVariablesByNumerics(const Savegame& saveGame, const ScriptComponent& script, const std::string& line, char varStartSymbol, char varEndSymbol);
		static std::string getFirstExpressionFromLine(ScriptAutoSystem& system, const std::string& line, ScriptComponent& script, size_t* outputCommandSize);
		static std::string getCommandCall(const std::string& s);
		static std::string getVariableKey(const std::string& v);
		static void setValueFromVarOrSwitchNumber(Savegame& saveGame, const std::string& scriptExtendedName, std::string varNumber, std::string value, std::unordered_map<std::string, std::string>& varMap);

		static std::string interpretVarName(const Savegame& saveGame, const ScriptComponent& script, const std::string& v);
		static bool isScriptActivated(const Savegame& saveGame, const std::string& script);
		~ScriptUtils();
	};
}
