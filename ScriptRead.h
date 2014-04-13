#ifndef DEF_SCRIPTREAD
#define DEF_SCRIPTREAD

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "LayerE.h"
#include "Layer.h"
#include "WGameCore.h"
#include "Character.h"



int ScriptRead(std::string name);
int GetValueFromVarOrSwitchNumber(std::string varNumber);
void SetValueFromVarOrSwitchNumber(std::string varNumber, std::string value);
//void ScriptsActiver(std::string worldName);
bool IsScriptActivated(const std::string& scriptName);
#endif
