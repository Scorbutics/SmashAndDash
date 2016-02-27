#pragma once
#include <SDL2\SDL_stdinc.h>
#include <unordered_map>
#include <string>
#include "../Script\ScriptState.h"

class IScript
{
public:
	IScript(){}
	virtual bool play() = 0;
	virtual float getPriority(Uint32 currentTimeMillis) = 0;
	virtual ScriptState getCurrentState() = 0;
	virtual std::unordered_map<std::string, std::string>& getVarMap() = 0;
	virtual unsigned int getCurrentLine() = 0;
	virtual std::string& getExtendedName() = 0;
	virtual void delay(unsigned int d) = 0;
	virtual void stop() = 0;
	virtual std::string getLastResult() = 0;
	virtual std::string& getKey() = 0;
	virtual std::string nextLine() = 0;
	virtual bool eof() = 0;
	~IScript(){}
	


};

typedef std::unique_ptr<IScript> IScript_ptr;
