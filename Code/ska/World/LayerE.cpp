#include <iostream>
#include <string>
#include <fstream>
#include <SDL2/SDL.h>
#include <sstream>

#include "../Utils/StringUtils.h"
#include "../Utils/SkaConstants.h"
#include "LayerE.h"
#include "../Script/ScriptDispatcher.h"
#include "World.h"

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
	/*WGameCore& wScreen = WGameCore::getInstance();
	World& w = wScreen.getWorld();

	if (wScreen.getFight().isFighting()) {
		return;
	}*/
		

	ska::Rectangle posCentre;
	int i = 0;

	/*posCentre.x = wScreen.getHero()->getHitboxCenterPos().x;
	posCentre.y = wScreen.getHero()->getHitboxCenterPos().y;
	posCentre.x /= TAILLEBLOC;
	posCentre.y /= TAILLEBLOC;
	*/
	for (i = i; i < getNbrLignes(); i++)
	{
		if (getBlocX(i) == posCentre.x && getBlocY(i) == posCentre.y && (getTrigger(i) == 1))
		{

			if (getAction(i) == "teleport")
			{
				//CommandTeleport::teleportHeroToMap(w.getLayerEvent()->getParam(i));
			}
			else if (getAction(i) == "script")
			{
				ifstream fscript(getParam(i).c_str());
				char buf;
				fscript.seekg(-1, ios::end);
				fscript >> buf;
				fscript.seekg(0, ios::beg);
				fscript.close();

				if (buf == '£')
				{
					ofstream fwriteScript(getParam(i).c_str());
					fwriteScript.seekp(-1, ios::end);
					fwriteScript << "";
					fwriteScript.close();
				}
				Uint32 scriptPeriod = 1000;
				ScriptDispatcher::getInstance().addRunningScript(NULL, getParam(i), m_world.getName(), vector<string>(), 2, &scriptPeriod);
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
    if(ligne < m_nbrLignes && ligne > 0)
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




int LireFEvent(ifstream *flux, char to)
{
    char a = 0;
    /* char buffer[5] = {0}; //On fixe ainsi le maximum de la taille de la map, qui sera 10000x10000... Autant dire qu'on a de quoi faire ! */
	string buffer; /*			^ N'importe quoi ce code et ce com' ! Utiliser une string à la place... */

    do
    {
        a = flux->get();

        if(a == to)
        {
            if(buffer[0] != '!')
                return ska::StringUtils::strToInt(buffer);
            else
                return INT_MIN;

        }
        else if(flux->eof())
			return INT_MIN;
        else
        {
            buffer += a;
        }
    }while(a != to);

	return INT_MIN;
}



void ChargerEvent(string *m_nomFichier,  int *m_nbrLignes, vector<int> &m_coordBX, vector<int> &m_coordBY, vector<int> &m_ID, vector<int> &m_trigger, vector<string> &m_param, vector<int> &m_solide, vector<string> &m_action, vector<string> &m_path)
{
	/* TODO Tout ceci est horrible. On dirait du C avec du C++. A réécrire. */

    char a = 0;
    int j = 0, i = 0;

    ifstream flux;

    string folder = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR"";
    string buffer;
    ostringstream oss;

    buffer = m_nomFichier->substr(0, m_nomFichier->find_last_of('E'));

    oss << buffer;
    oss << ""FILE_SEPARATOR"" + *m_nomFichier;
    folder += oss.str();

    flux.open(folder.c_str());

    if(flux.fail())    //On teste si le fichier s'est bien ouvert.
    {
        cerr << "Erreur (classe LayerE) : Impossible d'ouvrir le fichier event demandé: " << folder.c_str() << endl;
        exit(1);
    }


    for(i = 0; i < *m_nbrLignes; i++)
    {
        m_coordBX.pop_back();
        m_coordBY.pop_back();
        m_ID.pop_back();
        m_trigger.pop_back();
        m_param.pop_back();
        m_solide.pop_back();
        m_action.pop_back();
        m_path.pop_back();
    }

    i=0;

    *m_nbrLignes = 0;

        do{
            a = flux.get();
            if (a == '\n')
            {
                *m_nbrLignes = *m_nbrLignes + 1;
            }
        }while(!flux.eof());




    a = 0;
    j = 1;
    flux.clear();
    flux.seekg(ios::beg);

    while(flux.get() != '\n');


    m_coordBX.resize(*m_nbrLignes + 1);
    m_coordBY.resize(*m_nbrLignes + 1);
    m_ID.resize(*m_nbrLignes + 1);
    m_trigger.resize(*m_nbrLignes + 1);
    m_param.resize(*m_nbrLignes + 1);
    m_solide.resize(*m_nbrLignes + 1);
    m_action.resize(*m_nbrLignes + 1);
    m_path.resize(*m_nbrLignes + 1);

        do
        {
            i = LireFEvent(&flux, ':');
            m_coordBX[j] = i;
            i = LireFEvent(&flux, ' ');
            m_coordBY[j] = i;
            i = LireFEvent(&flux, ' ');
			if (i == INT_MIN)
                m_ID[j] = 0;
            else
                m_ID[j] = i;

            i = LireFEvent(&flux, ' ');
            m_solide[j] = i;
            i = LireFEvent(&flux, ' ');
            m_trigger[j] = i;

            if(m_coordBX[j] == -1 || m_coordBY[j] == -1 || m_trigger[j] == -1 || m_solide[j] == -1 )
            {
                cerr << "Erreur (classe LayerE) : Erreur lors de la lecture du fichier evenements. Ligne : " << j << endl;
            }

            flux >> m_path[j];
            flux >> m_action[j];

            flux.get();

           do
            {

                a = flux.get();
                if(a != '\n'&& !flux.eof())
                {
                    m_param[j] += a;
                }
                else if(flux.eof())
                {
                    cerr << "Erreur (classe LayerE) : Impossible de lire le parametre de l'evenement numero " << j << endl;
                    exit(0);
                }




            } while(a != '\n' && !flux.eof());

            j++;

        }while(j < *m_nbrLignes && !flux.eof());  //Tant qu'on n'est pas a la fin, on lit


}

