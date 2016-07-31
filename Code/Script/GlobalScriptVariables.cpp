#include "GlobalScriptVariables.h"


#include <map>
#include "../Gameplay/Data/Statistics.h"
#include "../ska/Utils/StringUtils.h"
#include "../Utils/IDs.h"

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

std::string GlobalScriptVariables::getPokemonHp(int idType, int id)
{
	/*WGameCore& wScreen = WGameCore::getInstance();
	if (wScreen.getFight().getPokemon() != NULL) {
		return ska::StringUtils::intToStr(wScreen.getFight().getPokemon()->getHp());
	}*/
	return "";
}

std::string GlobalScriptVariables::getCharacterDirection(int idType, int id) {
	/*WGameCore& wScreen = WGameCore::getInstance();
	const Character* c = wScreen.getEntityFactory().getNPC(idType, id);
	if (c != NULL) {
		return ska::StringUtils::intToStr(c->getDirection());
	}*/
	return "";
}

std::string GlobalScriptVariables::getCharacterPosX(int idType, int id) {
	/*WGameCore& wScreen = WGameCore::getInstance();
	const Character* c = wScreen.getEntityFactory().getNPC(idType, id);
	if (c != NULL) {
		return ska::StringUtils::intToStr(c->getHitboxCenterPos().x / TAILLEBLOC);
	}*/
	return "";
}

std::string GlobalScriptVariables::getCharacterPosY(int idType, int id) {
	/*WGameCore& wScreen = WGameCore::getInstance();
	const Character* c = wScreen.getEntityFactory().getNPC(idType, id);
	if (c != NULL) {
		return ska::StringUtils::intToStr(c->getHitboxCenterPos().y / TAILLEBLOC);
	}*/
	return "";
}

std::string GlobalScriptVariables::getPokemonHpMax(int idType, int id)
{
	/*WGameCore& wScreen = WGameCore::getInstance();
	if (wScreen.getFight().getPokemon() != NULL) {
		return ska::StringUtils::intToStr(wScreen.getFight().getPokemon()->getStatistics()->getHpMax());
	}*/
	return "";
}

std::string GlobalScriptVariables::getOpponentHp(int idType, int id)
{
	/*WGameCore& wScreen = WGameCore::getInstance();
	if (wScreen.getFight().getOpponent() != NULL) {
		return ska::StringUtils::intToStr(wScreen.getFight().getOpponent()->getHp());
	}*/
	return "";
}

std::string GlobalScriptVariables::getOpponentHpMax(int idType, int id)
{
	/*WGameCore& wScreen = WGameCore::getInstance();
	if (wScreen.getFight().getOpponent() != NULL) {
		return ska::StringUtils::intToStr(wScreen.getFight().getOpponent()->getStatistics()->getHpMax());
	}*/
	return "";
}

std::string GlobalScriptVariables::getOpponentCaptureRate(int idType, int id)
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
