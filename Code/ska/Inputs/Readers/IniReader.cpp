#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <algorithm>
#include "IniReader.h"

using namespace std;

ska::IniReader::IniReader()
{
	m_isLoaded = false;
}

ska::IniReader::IniReader(string inifilename)
{
	m_isLoaded = false;
	load(inifilename);
}

ska::IniReader::~IniReader()
{
}

bool ska::IniReader::load(string inifilename)
{
	m_content.clear();
	
	ifstream iniFile(inifilename.c_str());
	string line;
	string currentBlock;
	string currentVar;
	string currentContent;

	if(iniFile.fail())
	{
		m_isLoaded = false;
		return false;
	}

	do 
	{

		getline(iniFile, line);

		size_t posVar = line.find('=');

		//suppression des espaces non nécessaires si présents
		if(posVar == string::npos)
			line.erase( remove( line.begin(), line.end(), ' ' ), line.end() );
		else
			line.erase( remove( line.begin(), line.begin()+posVar, ' ' ), line.begin()+posVar );


		//pour être valide, on oblige le fichier ".ini" à contenir les blocs entre crochets en DEBUT de ligne (caractère numéro 0, d'où posBlockStart = 0)
		size_t posBlockStart = 0; 
		size_t posBlockEnd = line.find(']');
		posVar = line.find('=');

		if (line[posBlockStart] == '[' && posBlockEnd != string::npos)
			currentBlock = line.substr(posBlockStart+1, posBlockEnd-1);
		else if(posVar != string::npos)
		{
			currentVar = line.substr(0, posVar);
			currentContent = line.substr(posVar+1, line.length());

			if(currentBlock.empty())
				cerr << "Erreur (classe IniReader) : le fichier .ini " + inifilename + " contient une variable \""+ currentVar +"\" sans bloc entre crochets correspondant." << endl;
			else if(!currentContent.empty())
				m_content[currentBlock + " " + currentVar] = currentContent;

		}


	}while (!iniFile.eof());


	m_isLoaded = true;
	return true;
}

void ska::IniReader::save(string inifilename)
{
	ofstream iniFile(inifilename);
	list<string> lines;


	if(iniFile.fail())
	{
		cerr << "Erreur (classe IniReader) : impossible de créer le fichier " + inifilename << endl;
		return;
	}


	//transcription de la map sous forme de liste
	for(unordered_map<string, string>::iterator it = m_content.begin(); it != m_content.end(); ++it)
		IniListSet(lines, it->first, it->second);


	//écriture de la liste dans le fichier
	for(list<string>::iterator it = lines.begin(); it != lines.end(); ++it)
		iniFile << *it << endl;
}

/* TODO template pour get */

bool ska::IniReader::get(string path)
{
	return (m_content.find(path) != m_content.end());
}

string ska::IniReader::getString(string path)
{
	if(m_content.find(path) != m_content.end())
		return m_content[path];
	else
		return "STRINGNOTFOUND";
}

int ska::IniReader::getInt(string path)
{
	if(m_content.find(path) != m_content.end())
		return atoi(m_content[path].c_str());
	else
		return 0;
}

bool ska::IniReader::getBoolean(string path)
{
	if(m_content.find(path) != m_content.end())
		return (m_content[path] == "true");
	else
		return false;
}

bool ska::IniReader::isLoaded()
{
	return m_isLoaded;
}

void ska::IniReader::clear()
{
	m_content.clear();
}

float ska::IniReader::getFloat(std::string path)
{
	if(m_content.find(path) != m_content.end())
	{		
		stringstream ss;
		float f;
		ss << m_content[path];
		ss >> f;
		return f;
	}
	else
		return 0.;
}


//Insère ou modifie (si le chemin existe déjà) le contenu content du chemin path dans la liste lines
void IniListSet(list<string>& lines, string path, string content)
{
	string currentBlock;
	string blockPath, varPath;

	blockPath = path.substr(0, path.find(' '));
	varPath = path.substr(path.find(' ')+1, path.length());

	list<string>::iterator it = IniListSearchPath(lines, path);
	if(it == lines.end())
	{
		it = IniListSearchBlock(lines, blockPath);
		if(it == lines.end())
		{
				lines.push_back("[" + blockPath + "]");
				lines.push_back(varPath+"="+content);
			
		}
		else
		{
			it++;
			lines.insert(it, varPath+"="+content);
		}
		
	}
	else
	{
		*it = varPath+"="+content;
	}

}


//retourne l'iterateur dans la liste correspondant au chemin cherché, et lines.end() s'il n'est pas trouvé
list<string>::iterator IniListSearchPath(list<string>& lines, string path)
{
	string blockPath, varPath;

	blockPath = path.substr(0, path.find(' '));
	varPath = path.substr(path.find(' '), path.length());

	for(list<string>::iterator it = IniListSearchBlock(lines, blockPath); it != lines.end(); ++it)
	{
		size_t posBlockStart = 0; 
		size_t posBlockEnd = it->find(']');
		size_t posValue = it->find('=');

		//Si on arrive au bloc suivant, alors on n'a rien trouvé !
		if ((*it)[posBlockStart] == '[' && posBlockEnd != string::npos)
			return lines.end();

		//sinon, on teste si la variable cherchée est trouvée
		if(varPath == it->substr(0, posValue))
			return it;
		
	}

	//si on arrive à la fin de la liste, alors on n'a rien trouvé !
	return lines.end();
}

//retourne l'iterateur dans la liste correspondant au bloc cherché, et lines.end() s'il n'est pas trouvé
list<string>::iterator IniListSearchBlock(list<string>& lines, string block)
{
	string currentBlock;

	for(list<string>::iterator it = lines.begin(); it != lines.end(); ++it)
	{
		size_t posBlockStart = 0; 
		size_t posBlockEnd = it->find(']');

		//On vérifie que le chemin n'existe pas
		if ((*it)[posBlockStart] == '[' && posBlockEnd != string::npos)
		{
			currentBlock = it->substr(posBlockStart+1, posBlockEnd-1);
			if(currentBlock == block)
				return it;
		}
	}

	return lines.end();
}