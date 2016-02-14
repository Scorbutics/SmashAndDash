#pragma once
#include <string>
#include <unordered_map>
class ScriptUtils
{
public:
	ScriptUtils();
	static int getValueFromVarOrSwitchNumber(const std::string& scriptExtendedName, std::string varNumber, std::unordered_map<std::string, std::string>& varMap);
	static std::string replaceVariablesByNumerics(const std::string& scriptExtendedName, const std::string& line, std::unordered_map<std::string, std::string>& varMap);
	static std::string replaceVariablesByNumerics(const std::string& scriptExtendedName, const std::string& line, std::unordered_map<std::string, std::string>& varMap, char varStartSymbol, char varEndSymbol);
	static std::string getFirstExpressionFromLine(const std::string& line, const std::string& extendedName, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result, size_t* outputCommandSize);
	static std::string getCommandCall(const std::string& s);
	static std::string getVariableKey(const std::string& v);
	static void setValueFromVarOrSwitchNumber(const std::string& scriptExtendedName, std::string varNumber, std::string value, std::unordered_map<std::string, std::string>& varMap);

	static std::string interpretVarName(const std::string& scriptExtendedName, std::string& v, std::unordered_map<std::string, std::string>& varMap);
	static bool isScriptActivated(const std::string& scriptName);
	~ScriptUtils();
};

