#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <algorithm>
#include "IniReader.h"
#include "../../Logging/Logger.h"

void IniListSet(std::list<std::string>& lines, const std::string& path, const std::string& content);
std::list<std::string>::iterator IniListSearchPath(std::list<std::string>& lines, const std::string& path);
std::list<std::string>::iterator IniListSearchBlock(std::list<std::string>& lines, const std::string& block);

ska::IniReader::IniReader() {
	m_isLoaded = false;
}

ska::IniReader::IniReader(const std::string& inifilename) : 
IniReader() {
	load(inifilename);
}


bool ska::IniReader::load(const std::string& inifilename) {
	m_content.clear();
	
	std::ifstream iniFile(inifilename.c_str());
	std::string line;
	std::string currentBlock;
	std::string currentVar;
	std::string currentContent;

	if(iniFile.fail()) {
		m_isLoaded = false;
		return false;
	}

	do {

		getline(iniFile, line);

		auto posVar = line.find('=');

		//suppression des espaces non nécessaires si présents
		if (posVar == std::string::npos)
			line.erase( remove( line.begin(), line.end(), ' ' ), line.end() );
		else
			line.erase( remove( line.begin(), line.begin()+posVar, ' ' ), line.begin()+posVar );


		//pour être valide, on oblige le fichier ".ini" à contenir les blocs entre crochets en DEBUT de ligne (caractère numéro 0, d'où posBlockStart = 0)
		std::size_t posBlockStart = 0; 
		std::size_t posBlockEnd = line.find(']');
		posVar = line.find('=');

		if (line[posBlockStart] == '[' && posBlockEnd != std::string::npos) {
			currentBlock = line.substr(posBlockStart + 1, posBlockEnd - 1);
		} else if (posVar != std::string::npos) {
			currentVar = line.substr(0, posVar);
			currentContent = line.substr(posVar+1, line.length());

			if (currentBlock.empty()) {
				SKA_LOG_ERROR("Erreur (classe IniReader) : le fichier .ini " + inifilename + " contient une variable \"" + currentVar + "\" sans bloc entre crochets correspondant.");
			} else if (!currentContent.empty()) {
				m_content[currentBlock + " " + currentVar] = currentContent;
			}

		}


	} while (!iniFile.eof());


	m_isLoaded = true;
	return true;
}

void ska::IniReader::save(const std::string& inifilename) {
	std::ofstream iniFile(inifilename);
	std::list<std::string> lines;


	if(iniFile.fail()) {
		std::cerr << "Erreur (classe IniReader) : impossible de créer le fichier " + inifilename << std::endl;
		return;
	}


	//transcription de la map sous forme de liste
	for (auto it = m_content.begin(); it != m_content.end(); ++it) {
		IniListSet(lines, it->first, it->second);
	}


	//écriture de la liste dans le fichier
	for (std::list<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
		iniFile << *it << std::endl;
	}
}

bool ska::IniReader::exists(const std::string& path) const {
	return (m_content.find(path) != m_content.end());
}


bool ska::IniReader::isLoaded() const {
	return m_isLoaded;
}

void ska::IniReader::clear() {
	m_content.clear();
}


//Insère ou modifie (si le chemin existe déjà) le contenu content du chemin path dans la liste lines
void IniListSet(std::list<std::string>& lines, const std::string& path, const std::string& content) {
	std::string currentBlock;
	std::string blockPath, varPath;

	blockPath = path.substr(0, path.find(' '));
	varPath = path.substr(path.find(' ')+1, path.length());

	std::list<std::string>::iterator it = IniListSearchPath(lines, path);
	if(it == lines.end()) {
		it = IniListSearchBlock(lines, blockPath);
		if(it == lines.end()) {
				lines.push_back("[" + blockPath + "]");
				lines.push_back(varPath+"="+content);
			
		} else {
			++it;
			lines.insert(it, varPath+"="+content);
		}
		
	} else {
		*it = varPath+"="+content;
	}

}


//retourne l'iterateur dans la liste correspondant au chemin cherché, et lines.end() s'il n'est pas trouvé
std::list<std::string>::iterator IniListSearchPath(std::list<std::string>& lines, const std::string& path) {
	std::string blockPath, varPath;

	blockPath = path.substr(0, path.find(' '));
	varPath = path.substr(path.find(' '), path.length());

	for (std::list<std::string>::iterator it = IniListSearchBlock(lines, blockPath); it != lines.end(); ++it) {
		size_t posBlockStart = 0; 
		size_t posBlockEnd = it->find(']');
		size_t posValue = it->find('=');

		//Si on arrive au bloc suivant, alors on n'a rien trouvé !
		if ((*it)[posBlockStart] == '[' && posBlockEnd != std::string::npos) {
			return lines.end();
		}

		//sinon, on teste si la variable cherchée est trouvée
		if (varPath == it->substr(0, posValue)) {
			return it;
		}
		
	}

	//si on arrive à la fin de la liste, alors on n'a rien trouvé !
	return lines.end();
}

//retourne l'iterateur dans la liste correspondant au bloc cherché, et lines.end() s'il n'est pas trouvé
std::list<std::string>::iterator IniListSearchBlock(std::list<std::string>& lines, const std::string& block) {
	std::string currentBlock;

	for (std::list<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
		size_t posBlockStart = 0; 
		size_t posBlockEnd = it->find(']');

		//On vérifie que le chemin n'existe pas
		if ((*it)[posBlockStart] == '[' && posBlockEnd != std::string::npos) {
			currentBlock = it->substr(posBlockStart+1, posBlockEnd-1);
			if (currentBlock == block) {
				return it;
			}
		}
	}

	return lines.end();
}
