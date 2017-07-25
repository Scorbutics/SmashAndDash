#include <limits>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "Utils/StringUtils.h"
#include "Utils/SkaConstants.h"
#include "LayerE.h"
#include "Layer.h"
#include "World.h"
#include "Exceptions/CorruptedFileException.h"
#include "Exceptions/NumberFormatException.h"
#include "Logging/Logger.h"

//Constructeur ouvrant un monde déjà créé
ska::LayerE::LayerE(World& world) : m_world(world) {
    m_nbrLignes = 0;
}


void ska::LayerE::refresh(unsigned int) {
	/* TODO Chipset relative scripts */
}

int ska::LayerE::getBlocX(int ligne) const {
	return (ligne < m_nbrLignes && ligne >= 0) ? m_coordBX[ligne] : -1;
}

int ska::LayerE::getBlocY(int ligne) const {
	return (ligne < m_nbrLignes && ligne >= 0) ? m_coordBY[ligne] : -1;
}

int ska::LayerE::getID(int ligne) const {
	return (ligne <= m_nbrLignes && ligne >= 0) ? m_ID[ligne] : 0;
}

std::string ska::LayerE::getParam(int ligne) const {
	return (ligne < m_nbrLignes && ligne >= 0) ? m_param[ligne] : "";
}

int ska::LayerE::getTrigger(int ligne) const {
	return (ligne < m_nbrLignes && ligne >= 0) ? m_trigger[ligne] : -1;
}

int ska::LayerE::getSolide(int ligne) const {
	return (ligne < m_nbrLignes && ligne >= 0) ? m_solide[ligne] : -1;
}

std::string ska::LayerE::getAction(int ligne) const
{
	if (ligne < m_nbrLignes && ligne >= 0) {
		return m_action[ligne];
	} else if (ligne != 0) {
		SKA_LOG_ERROR("Erreur (classe LayerE) : Buffer Overflow lors de la tentative d'accès à la ligne ", ligne, " dans le fichier évènement ", m_nomFichier);
    }
    return "";

}

std::string ska::LayerE::getPath(int ligne) const
{
	if (ligne < m_nbrLignes && ligne >= 0) {
		return m_path[ligne];
	} else if (ligne != 0) {
		SKA_LOG_ERROR("Erreur (classe LayerE) : Buffer Overflow lors de la tentative d'accès à la ligne ", ligne, " dans le fichier évènement ", m_nomFichier);
    }
    return "";

}

int ska::LayerE::getNbrLignes() const {
    return m_nbrLignes;
}

void ska::LayerE::changeLevel(const std::string& fichier) {
    m_nomFichier = fichier;
	const std::string folder = "." FILE_SEPARATOR "Levels" FILE_SEPARATOR "" + m_nomFichier.substr(0, m_nomFichier.find_last_of('E')) + "" FILE_SEPARATOR "" + m_nomFichier;
	std::ifstream flux(folder.c_str());
	std::stringstream ss;
	if (flux.fail()) {
		throw CorruptedFileException("Erreur (classe LayerE) : Impossible d'ouvrir le fichier event demandé: " + folder);
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
	getline(flux, line);

	int i = 0;
	try {
		while (getline(flux, line)) {

			i++;
			std::size_t nextIndex = 0;
			ss.clear();

			const std::string x = StringUtils::extractTo(nextIndex, line, ':');
			m_coordBX.push_back(StringUtils::strToInt(x));
			nextIndex += x.size() + 1;

			ss << line.substr(nextIndex);
			std::string y;
			ss >> y;
			m_coordBY.push_back(StringUtils::strToInt(y));

			std::string id;
			ss >> id;
			if (id != "!") {
				m_ID.push_back(StringUtils::strToInt(id));
			}
			else {
				m_ID.push_back(std::numeric_limits<int>().min());
			}

			std::string solide;
			ss >> solide;
			m_solide.push_back(StringUtils::strToInt(solide));

			std::string trigger;
			ss >> trigger;
			m_trigger.push_back(StringUtils::strToInt(trigger));

			std::string path;
			ss >> path;
			m_path.push_back(path);

			std::string action;
			ss >> action;
			m_action.push_back(action);

			std::string param;
			getline(ss, param);
			m_param.push_back(StringUtils::ltrim(param));

		}
	}
	catch (NumberFormatException& nfe) {
		throw CorruptedFileException("Erreur (classe LayerE) : Erreur lors de la lecture du fichier evenements (ligne : " + StringUtils::intToStr(i) + ")\n" + std::string(nfe.what()));
	}
	m_nbrLignes = i;
}
