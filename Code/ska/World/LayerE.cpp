#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "../Utils/StringUtils.h"
#include "../Utils/SkaConstants.h"
#include "LayerE.h"
#include "../Script/ScriptDispatcher.h"
#include "World.h"
#include "../Exceptions/CorruptedFileException.h"
#include "../Exceptions/NumberFormatException.h"

using namespace std;

int LireFEvent(ifstream *flux, char jusquaCeCaractere);
void ChargerEvent(string *m_nomFichier,  int *m_nbrLignes, vector<int> &m_coordBX, vector<int> &m_coordBY, vector<int> &m_ID, vector<int> &m_trigger, vector<string> &m_param, vector<int> &m_solide, vector<string> &m_action, vector<string> &m_path);

ska::LayerE::LayerE(ska::World& world, string nomFichier) : m_world(world) //Constructeur ouvrant un monde déjà créé
{

    m_nomFichier = nomFichier;
    m_nbrLignes = 0;

    ChargerEvent(&m_nomFichier, &m_nbrLignes, m_vcoordBX, m_vcoordBY, m_vID, m_vtrigger, m_vparam, m_vsolide, m_vaction, m_vpath);
}


void ska::LayerE::refresh() {

	/*TODO*/

	/*WGameCore& wScreen = WGameCore::getInstance();
	World& w = wScreen.getWorld();

	if (wScreen.getFight().isFighting()) {
		return;
	}*/
		

	ska::Rectangle posCentre = { 0 };

	/*posCentre.x = wScreen.getHero()->getHitboxCenterPos().x;
	posCentre.y = wScreen.getHero()->getHitboxCenterPos().y;
	posCentre.x /= TAILLEBLOC;
	posCentre.y /= TAILLEBLOC;
	*/
	for (int i = 0; i < m_nbrLignes; i++) {
		if (m_vcoordBX[i] == posCentre.x && m_vcoordBY[i] == posCentre.y && m_vtrigger[i] == 1) {
			if (m_vaction[i] == "teleport") {
				//CommandTeleport::teleportHeroToMap(m_vparam[i]);
			} else if (m_vaction[i] == "script") {
				ifstream fscript(m_vparam[i]);
				char buf;
				fscript.seekg(-1, ios::end);
				fscript >> buf;
				fscript.seekg(0, ios::beg);
				fscript.close();

				if (buf == '£')
				{
					ofstream fwriteScript(m_vparam[i]);
					fwriteScript.seekp(-1, ios::end);
					fwriteScript << "";
					fwriteScript.close();
				}
				unsigned int scriptPeriod = 1000;
				/*ScriptDispatcher::getInstance().addRunningScript(NULL, getParam(i), m_world.getName(), vector<string>(), 2, &scriptPeriod);*/
			}
		}
	}
	
}

int ska::LayerE::getBlocX(int ligne)
{
    if(ligne < m_nbrLignes && ligne > 0)
        return m_vcoordBX[ligne];
    else
        return -1;
}

int ska::LayerE::getBlocY(int ligne)
{
    if(ligne < m_nbrLignes && ligne > 0)
        return m_vcoordBY[ligne];
    else
        return -1;
}

int ska::LayerE::getID(int ligne)
{
    if(ligne <= m_nbrLignes && ligne > 0)
        return m_vID[ligne];
    else
        return 0;
}

string ska::LayerE::getParam(int ligne)
{
    if(ligne < m_nbrLignes && ligne > 0)
        return m_vparam[ligne];
    else
        return "Erreur";

}

int ska::LayerE::getTrigger(int ligne)
{
    if(ligne < m_nbrLignes && ligne > 0)
        return m_vtrigger[ligne];
    else
        return -1;
}

int ska::LayerE::getSolide(int ligne)
{
    if(ligne < m_nbrLignes && ligne > 0)
        return m_vsolide[ligne];
    else
        return -1;
}

string ska::LayerE::getAction(int ligne)
{
    if(ligne < m_nbrLignes && ligne > 0)
        return m_vaction[ligne];
    else if (ligne != 0)
    {
        cerr << "Erreur (classe LayerE) : Buffer Overflow lors de la tentative d'accès à la ligne " << ligne << " dans le fichier évènement " << m_nomFichier << endl;
        return "Erreur";
    }
    else
        return "Erreur";

}

string ska::LayerE::getPath(int ligne)
{
    if(ligne < m_nbrLignes && ligne > 0)
        return m_vpath[ligne];
    else if (ligne != 0)
    {
        cerr << "Erreur (classe LayerE) : Buffer Overflow lors de la tentative d'accès à la ligne " << ligne << " dans le fichier évènement " << m_nomFichier << endl;
        return "Erreur";
    }
    else
        return "Erreur";

}

int ska::LayerE::getNbrLignes()
{
    return m_nbrLignes;
}

void ska::LayerE::changeLevel(string fichier)
{
    m_nomFichier = fichier;
    ChargerEvent(&m_nomFichier, &m_nbrLignes, m_vcoordBX, m_vcoordBY, m_vID, m_vtrigger, m_vparam, m_vsolide, m_vaction, m_vpath);
}


void ChargerEvent(string *m_nomFichier,  int *m_nbrLignes, vector<int> &m_coordBX, vector<int> &m_coordBY, vector<int> &m_ID, vector<int> &m_trigger, vector<string> &m_param, vector<int> &m_solide, vector<string> &m_action, vector<string> &m_path) {
    const string folder = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"" + m_nomFichier->substr(0, m_nomFichier->find_last_of('E')) + ""FILE_SEPARATOR"" + *m_nomFichier;
	ifstream flux(folder.c_str());
	stringstream ss;
    if(flux.fail()) {
        throw ska::CorruptedFileException("Erreur (classe LayerE) : Impossible d'ouvrir le fichier event demandé: " + folder);
    }

	m_coordBX.clear();
	m_coordBY.clear();
	m_ID.clear();
	m_trigger.clear();
	m_param.clear();
	m_solide.clear();
	m_action.clear();
	m_path.clear();

    
	std::string line;

	/* Ignore first line */
	std::getline(flux, line);

	int i = 0;
	try {
		while (std::getline(flux, line)) {
			
			i++;
			size_t nextIndex = 0;
			ss.clear();

			const std::string x = ska::StringUtils::extractTo(nextIndex, line, ':');
			m_coordBX.push_back(ska::StringUtils::strToInt(x));
			nextIndex += x.size() + 1;
			
			ss << line.substr(nextIndex);
			std::string y;
			ss >> y;
			m_coordBY.push_back(ska::StringUtils::strToInt(y));
			
			std::string id;
			ss >> id;
			if (id != "!") {
				m_ID.push_back(ska::StringUtils::strToInt(id));
			} else {
				m_ID.push_back(INT_MIN);
			}

			std::string solide;
			ss >> solide;
			m_solide.push_back(ska::StringUtils::strToInt(solide));

			std::string trigger;
			ss >> trigger;
			m_trigger.push_back(ska::StringUtils::strToInt(trigger));

			std::string path;
			ss >> path;
			m_path.push_back(path);
			
			std::string action;
			ss >> action;
			m_action.push_back(action);

			std::string param;
			std::getline(ss, param);
			m_param.push_back(ska::StringUtils::ltrim(param));

		}
	} catch (ska::NumberFormatException& nfe) {
		throw ska::CorruptedFileException("Erreur (classe LayerE) : Erreur lors de la lecture du fichier evenements (ligne : " + ska::StringUtils::intToStr(i) + ")\n" + std::string(nfe.what()));
	}
	*m_nbrLignes = i;
}

