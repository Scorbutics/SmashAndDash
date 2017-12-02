#include "CommandTeleport.h"
#include "Script/ScriptComponent.h"
#include "Script/System/ScriptAutoSystem.h"
#include "Exceptions/InvalidPathException.h"
#include "World/World.h"
#include "../../Utils/IDs.h"
#include "Utils/StringUtils.h"

#include "../../Gameplay/State/StateToMapSwitcher.h"
#include "../../Gameplay/Data/MapEvent.h"

CommandTeleport::CommandTeleport(const ska::World& w, ska::EntityManager& entityManager, PokemonGameEventDispatcher& ged) :
AbstractFunctionCommand(entityManager),
m_world(w),
m_ged(ged) {
}


CommandTeleport::~CommandTeleport() {
}

int CommandTeleport::argumentsNumber() {
	return 4;
}

std::string CommandTeleport::execute(ska::ScriptComponent& script, std::vector<std::string>& args) {
	const auto& mapName = args[0];
	const auto& id = args[1];
	const auto x = ska::StringUtils::strToInt(args[2]);
	const auto y = ska::StringUtils::strToInt(args[3]);

	auto internalEntity = script.parent->getEntityFromName(id);
	/* A script HAS a position component */
	auto& pc = m_entityManager.getComponent<ska::PositionComponent>(internalEntity);
	pc.x = m_world.getBlockSize() * x;
	pc.y = m_world.getBlockSize() * y;

	/* Hero use case */
	if (id == "0") {
		const auto& fichier = mapName;
		const auto buf = ".\\Levels" FILE_SEPARATOR ""
			+ fichier
			+ "" FILE_SEPARATOR ""
			+ fichier
			+ ".ini";

		ska::IniReader mapReader(buf);

		const auto chipsetName = mapReader.get<std::string>("Chipset file");

		if (chipsetName == "STRINGNOTFOUND" || chipsetName == "EMPTYDATA") {
			throw ska::InvalidPathException("Erreur : impossible de trouver le nom du chipset de la map de depart");
		}

		MapEvent me(MapEvent::MAP);
		me.chipsetName = chipsetName;
		me.mapName = mapName;
		m_ged.ska::Observable<MapEvent>::notifyObservers(me);
	}

	return "";
}

void CommandTeleport::teleportHeroToMap(ska::World& w, std::string param) {

	std::string fichier, fichier2, fichierD;
	int x = 1/*, y*/;


	fichier = ska::StringUtils::extractTo(0, param, '/');
	fichierD = ska::StringUtils::extractTo(fichier.size() + 1, param, '/');
	fichier2 = ska::StringUtils::extractTo(fichier.size() + fichierD.size() + 2, param, '/');
	x = ska::StringUtils::strToInt(ska::StringUtils::extractTo(fichier.size() + fichierD.size() + fichier2.size() + 3, param, ':'));

	// créer un flux de sortie
	std::ostringstream oss;
	oss << x; //on ecrit la valeur de l'entier x dedans
	std::string buf; //on le transforme en string
	buf = oss.str();
	oss.clear();
	//y = atoi(param.substr(param.find_last_of(':') + 1, param.find_last_of('\0')).c_str());

	//wScreen.transition(0);

	buf = ".\\Levels" FILE_SEPARATOR ""
		+ fichier.substr(0, fichier.find_last_of("."))
		+ "" FILE_SEPARATOR ""
		+ fichier.substr(0, fichier.find_last_of("."))
		+ ".ini";

	ska::IniReader mapReader(buf);

	std::string chipsetName = mapReader.get<std::string>("Chipset file");

	if (chipsetName == "STRINGNOTFOUND" || chipsetName == "EMPTYDATA") {
		SKA_STATIC_LOG_ERROR(CommandTeleport)("Erreur : impossible de trouver le nom du chipset de la map de depart");
	}

	w.load(fichier, chipsetName);

}
