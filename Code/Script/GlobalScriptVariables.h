#pragma once

#include <unordered_map>
#include "../ska/Utils\Singleton_template.h"

class GlobalScriptVariables : public ska::Singleton<GlobalScriptVariables>
{
	friend class ska::Singleton<GlobalScriptVariables>;

	typedef std::string (GlobalScriptVariables::*script_method_t)(int, int); // function pointer type

	public:
		std::string returnValue(std::string key, const int idType, const int id);

	private:
		GlobalScriptVariables();
		~GlobalScriptVariables();
		void fillMap();

		std::string getCharacterDirection(int idType, int id);
		std::string getCharacterPosX(int idType, int id);
		std::string getCharacterPosY(int idType, int id);
		std::string getPokemonHp(int idType, int id);
		std::string getPokemonHpMax(int idType, int id);
		std::string getOpponentHp(int idType, int id);
		std::string getOpponentHpMax(int idType, int id);
		std::string getOpponentCaptureRate(int idType, int id);
			
		std::unordered_map<std::string, script_method_t> globalVarMap;
};

