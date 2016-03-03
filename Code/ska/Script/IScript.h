#pragma once
#include <unordered_map>
#include <string>
#include "ScriptState.h"
#include "../Data/Savegame.h"

namespace ska {
	class ScriptDispatcher;
	class IScript
	{
	public:
		IScript(){}
		virtual ScriptDispatcher& getParent() = 0;
		virtual bool play(Savegame& savegame) = 0;
		virtual float getPriority(const unsigned int currentTimeMillis) = 0;
		virtual ScriptState getCurrentState() const = 0;
		virtual std::unordered_map<std::string, std::string>& getVarMap() = 0;
		virtual unsigned int getCurrentLine() const = 0;
		virtual void delay(const unsigned int d) = 0;
		virtual void stop() = 0;
		virtual void killAndSave(const Savegame& savegame) = 0;
		virtual std::string getContext() const = 0;
		virtual std::string getExtendedName() const = 0;
		virtual std::string getLastResult() const = 0;
		virtual std::string getKey() const = 0;
		virtual std::string nextLine() = 0;
		virtual std::string interpret(Savegame& savegame, const std::string& cmd) = 0;
		virtual bool eof() const = 0;
		~IScript(){}
	};

	typedef std::unique_ptr<IScript> IScriptPtr;
}
