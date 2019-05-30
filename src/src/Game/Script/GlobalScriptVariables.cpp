#include <map>
#include "Game/__internalConfig/LoggerConfig.h"
#include "GlobalScriptVariables.h"
#include "Game/Gameplay/Data/Statistics.h"
#include "Base/Values/Strings/StringUtils.h"
#include "Game/Utils/IDs.h"

GlobalScriptVariables::GlobalScriptVariables()
{
	fillMap();
}

void GlobalScriptVariables::fillMap()
{
	globalVarMap["personnage.pos.y"] = &GlobalScriptVariables::getCharacterPosY;
	globalVarMap["personnage.pos.x"] = &GlobalScriptVariables::getCharacterPosX;
	globalVarMap["personnage.direction"] = &GlobalScriptVariables::getCharacterDirection;
	globalVarMap["combat.pokemon.pvs"] = &GlobalScriptVariables::getPokemonHp;
	globalVarMap["combat.ennemi.pvs"] = &GlobalScriptVariables::getOpponentHp;
	globalVarMap["combat.pokemon.pvsmax"] = &GlobalScriptVariables::getPokemonHpMax;
	globalVarMap["combat.ennemi.pvsmax"] = &GlobalScriptVariables::getOpponentHpMax;
	globalVarMap["combat.ennemi.capturerate"] = &GlobalScriptVariables::getOpponentCaptureRate;
}

std::string GlobalScriptVariables::returnValue(std::string key, const int idType, const int id)
{
	if (globalVarMap.find(key) != globalVarMap.end()) {
		return (this->*globalVarMap[key])(idType, id);
	}
	return "";
}

std::string GlobalScriptVariables::getPokemonHp(int, int)
{
	/*WGameCore& wScreen = WGameCore::getInstance();
	if (wScreen.getFight().getPokemon() != NULL) {
		return ska::StringUtils::intToStr(wScreen.getFight().getPokemon()->getHp());
	}*/
	return "";
}

std::string GlobalScriptVariables::getCharacterDirection(int , int ) {
	/*WGameCore& wScreen = WGameCore::getInstance();
	const Character* c = wScreen.getEntityFactory().getNPC(idType, id);
	if (c != NULL) {
		return ska::StringUtils::intToStr(c->getDirection());
	}*/
	return "";
}

std::string GlobalScriptVariables::getCharacterPosX(int , int ) {
	/*WGameCore& wScreen = WGameCore::getInstance();
	const Character* c = wScreen.getEntityFactory().getNPC(idType, id);
	if (c != NULL) {
		return ska::StringUtils::intToStr(c->getHitboxCenterPos().x / TAILLEBLOC);
	}*/
	return "";
}

std::string GlobalScriptVariables::getCharacterPosY(int , int ) {
	/*WGameCore& wScreen = WGameCore::getInstance();
	const Character* c = wScreen.getEntityFactory().getNPC(idType, id);
	if (c != NULL) {
		return ska::StringUtils::intToStr(c->getHitboxCenterPos().y / TAILLEBLOC);
	}*/
	return "";
}

std::string GlobalScriptVariables::getPokemonHpMax(int , int )
{
	/*WGameCore& wScreen = WGameCore::getInstance();
	if (wScreen.getFight().getPokemon() != NULL) {
		return ska::StringUtils::intToStr(wScreen.getFight().getPokemon()->getStatistics()->getHpMax());
	}*/
	return "";
}

std::string GlobalScriptVariables::getOpponentHp(int , int )
{
	/*WGameCore& wScreen = WGameCore::getInstance();
	if (wScreen.getFight().getOpponent() != NULL) {
		return ska::StringUtils::intToStr(wScreen.getFight().getOpponent()->getHp());
	}*/
	return "";
}

std::string GlobalScriptVariables::getOpponentHpMax(int , int )
{
	/*WGameCore& wScreen = WGameCore::getInstance();
	if (wScreen.getFight().getOpponent() != NULL) {
		return ska::StringUtils::intToStr(wScreen.getFight().getOpponent()->getStatistics()->getHpMax());
	}*/
	return "";
}

std::string GlobalScriptVariables::getOpponentCaptureRate(int , int )
{
	/*WGameCore& wScreen = WGameCore::getInstance();
	if (wScreen.getFight().getOpponent() != NULL) {
		return ska::StringUtils::intToStr(wScreen.getFight().getOpponent()->getDescriptor()->getCaptureRate());
	}*/
	return "";
}

GlobalScriptVariables::~GlobalScriptVariables()
{
}
