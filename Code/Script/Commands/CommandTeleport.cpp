#include "CommandTeleport.h"
#include <string>
#include <map>
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\IDs.h"
#include "../../Utils\StringUtils.h"

using namespace std;

CommandTeleport::CommandTeleport()
{
}


CommandTeleport::~CommandTeleport()
{
}

string ExtractTo(int depart, std::string chaine, char jusquaCeCaractere, int *entier)
{
	size_t pos = chaine.substr(depart, chaine.size()).find_first_of(jusquaCeCaractere);
	string result = "ERROR";
	if (pos != string::npos)
	{
		result = chaine.substr(depart, pos);
		if (entier != NULL)
			*entier = atoi(result.c_str());
	}

	return result;
}

int CommandTeleport::argumentsNumber() {
	return 3;
}

bool CommandTeleport::execute(const std::string& extendedName, std::vector<std::string>& args, std::ofstream& scriptList, std::unordered_map<std::string, std::string>& varMap, std::ifstream& fscript, int& active, std::string* result)
{
	WGameCore& wScreen = WGameCore::getInstance();
	int id, number;
	string param;

	id = StringUtils::strToInt(args[0]);
	number = StringUtils::strToInt(args[1]);
	param = args[2];

	if (id != 0)
	{
		int x, y;
		x = atoi(param.substr(0, param.find_first_of(':')).c_str());
		y = atoi(param.substr(param.find_first_of(':') + 1, param.size()).c_str());

		wScreen.getEntityFactory().getNPC(id, number)->teleport(x*TAILLEBLOC, y*TAILLEBLOC);
	}
	else {
		teleportHeroToMap(param);
	}
	return true;
}

void CommandTeleport::teleportHeroToMap(string param)
{
	WGameCore& wScreen = WGameCore::getInstance();
	World& w = wScreen.getWorld();
	Character* hero = wScreen.getHero();

	string fichier, fichier2, fichierD;
	int x = 1, y;


	fichier = ExtractTo(0, param, '/', NULL);
	fichierD = ExtractTo((int)strlen(fichier.c_str()) + 1, param, '/', NULL);
	fichier2 = ExtractTo((int)strlen(fichier.c_str()) + (int)strlen(fichierD.c_str()) + 2, param, '/', NULL);
	ExtractTo((int)strlen(fichier.c_str()) + (int)strlen(fichierD.c_str()) + (int)strlen(fichier2.c_str()) + 3, param, ':', &x);

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

	IniReader mapReader(buf);

	string chipsetName = mapReader.getString("Chipset file");

	if (chipsetName == "STRINGNOTFOUND" || chipsetName == "EMPTYDATA")
		cerr << "Erreur : impossible de trouver le nom du chipset de la map de depart" << endl;

	w.changeLevel(fichier, chipsetName);

	hero->teleport(x*TAILLEBLOC - wScreen.getHero()->getWidth() / 2, y*TAILLEBLOC - wScreen.getHero()->getHeight() / 2);

}