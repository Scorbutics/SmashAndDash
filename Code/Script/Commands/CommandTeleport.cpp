#include "CommandTeleport.h"
#include <string>
#include "../../ska/Exceptions/InvalidPathException.h"
#include "../../Gameplay\WGameCore.h"
#include "../../ska/World/World.h"
#include "../../Utils\IDs.h"
#include "../../ska/Utils\StringUtils.h"

using namespace std;

CommandTeleport::CommandTeleport(ska::EntityManager& entityManager) : AbstractFunctionCommand(entityManager)
{
}


CommandTeleport::~CommandTeleport()
{
}

int CommandTeleport::argumentsNumber() {
	return 4;
}

std::string CommandTeleport::execute(ska::ScriptComponent& script, std::vector<std::string>& args)
{
	WGameCore& wScreen = WGameCore::getInstance();
	ska::World& w = wScreen.getWorld();

	const std::string& mapName = args[0];	
	const std::string& id = args[1];
	const int x = ska::StringUtils::strToInt(args[2]);
	const int y = ska::StringUtils::strToInt(args[3]);

	ska::EntityId internalEntity = script.parent->getEntityFromName(id);
	/* A script HAS a position component */
	ska::PositionComponent& pc = m_entityManager.getComponent<ska::PositionComponent>(internalEntity);
	pc.x = w.getBlockSize() * x;
	pc.y = w.getBlockSize() * y;

	/* Hero use case */
	if (id == "0") {
		const std::string& fichier = args[0];
		const std::string buf = ".\\Levels"FILE_SEPARATOR""
			+ fichier
			+ ""FILE_SEPARATOR""
			+ fichier
			+ ".ini";

		ska::IniReader mapReader(buf);

		string chipsetName = mapReader.getString("Chipset file");

		if (chipsetName == "STRINGNOTFOUND" || chipsetName == "EMPTYDATA") {
			throw ska::InvalidPathException("Erreur : impossible de trouver le nom du chipset de la map de depart");
		}

		w.changeLevel(fichier, chipsetName);
	}

	return "";
}

void CommandTeleport::teleportHeroToMap(string param)
{
	WGameCore& wScreen = WGameCore::getInstance();
	ska::World& w = wScreen.getWorld();
	//Character* hero = wScreen.getHero();

	string fichier, fichier2, fichierD;
	int x = 1, y;


	fichier = ska::StringUtils::extractTo(0, param, '/');
	fichierD = ska::StringUtils::extractTo(fichier.size() + 1, param, '/');
	fichier2 = ska::StringUtils::extractTo(fichier.size() + fichierD.size() + 2, param, '/');
	x = ska::StringUtils::strToInt(ska::StringUtils::extractTo(fichier.size() + fichierD.size() + fichier2.size() + 3, param, ':'));

	// créer un flux de sortie
	ostringstream oss;
	oss << x; //on ecrit la valeur de l'entier x dedans
	string buf; //on le transforme en string
	buf = oss.str();
	oss.clear();
	y = atoi(param.substr(param.find_last_of(':') + 1, param.find_last_of('\0')).c_str());

	//wScreen.transition(0);

	buf = ".\\Levels"FILE_SEPARATOR""
		+ fichier.substr(0, fichier.find_last_of("."))
		+ ""FILE_SEPARATOR""
		+ fichier.substr(0, fichier.find_last_of("."))
		+ ".ini";

	ska::IniReader mapReader(buf);

	string chipsetName = mapReader.getString("Chipset file");

	if (chipsetName == "STRINGNOTFOUND" || chipsetName == "EMPTYDATA")
		cerr << "Erreur : impossible de trouver le nom du chipset de la map de depart" << endl;

	w.changeLevel(fichier, chipsetName);

	//hero->teleport(x*TAILLEBLOC - wScreen.getHero()->getWidth() / 2, y*TAILLEBLOC - wScreen.getHero()->getHeight() / 2);

}