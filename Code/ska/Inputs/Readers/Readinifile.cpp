#include <iostream>
#include <fstream>
#include <sstream>

#define MAXLENGTH 20

#include "Readinifile.h"


bool isReadable(const std::string& fileName) {
	std::ifstream f(fileName.c_str());
    return !f.fail();
}

int FindInFile(const std::string& stringFile, const std::string& s) {
	std::string content, line;
	std::stringstream streamContent;
    size_t pos = 0;


	std::ifstream f(stringFile.c_str(), std::ios::binary);
    if(f.fail())
    {
		std::cerr << "Erreur lors de l'ouverture du fichier " + stringFile;
        return -1;
    }


    streamContent << f.rdbuf();
    content = streamContent.str();

    pos = content.find(s);

	if (pos == std::string::npos)
        return -1; //pas trouvé
    else
        return (int)(pos + 1); //trouvé


}


std::string GetINIValueFromFile(const std::string& stringFile, const std::string& s, const std::string& from) {
	std::string line, data;
    int startPos = FindInFile(stringFile, from);
	std::ifstream f(stringFile.c_str(), std::ios::binary);

    unsigned int pos;

    if(f.fail())
		std::cerr << "Erreur lors de l'ouverture du fichier " + stringFile;

    if(startPos != -1)
        f.seekg(startPos + from.length());
    else
        return "STRINGNOTFOUND";

    do
    {
        getline(f, line);
        pos = (unsigned int)line.find(s);
		if (pos != std::string::npos)
        {
           data = line.substr(pos + s.length() + 1, line.length()-(pos+ s.length() + 1));
           if(data.size() != 0)
            return rtrim(data);
           else
            return "EMPTYDATA";
        }

	} while (line.find("[") == std::string::npos && !f.eof());

    return "STRINGNOTFOUND";

}

std::string SetINIValueToFile(const std::string& stringFile, const std::string& s, const std::string& to, const std::string& value) {
	std::string line, data;
    unsigned int pos;
    int startPos = FindInFile(stringFile, to);
	std::ifstream fr(stringFile.c_str(), std::ios::binary);


    if(fr.fail())
		std::cerr << "Erreur lors de l'ouverture du fichier " + stringFile;

    if(startPos != -1)
    {
        startPos += (int)to.length();
        fr.seekg(startPos);
    }
    else
        return "STRINGNOTFOUND";

    do
    {
        getline(fr, line);
        pos = (unsigned int)line.find(s);
		if (pos != std::string::npos)
            pos = ((int)fr.tellg()) -2; // se place avant le \n (qui fait 2 caractères sous windows)

	} while (line.find("[") == std::string::npos && !fr.eof() && pos == std::string::npos);
    fr.close();



	if (pos != std::string::npos)
    {
        ReplaceStringInFile(stringFile, pos, value, GetINIValueFromFile(stringFile, s, to));
        return "STRINGFOUND";
    }
    else
        return "STRINGNOTFOUND";

}



void ReplaceStringInFile(const std::string& fileName, int pos, const std::string& value, const std::string& oldValue) {
    ResetFile(fileName+"tmp");

    int fileLength = GetLengthOfFile(fileName);

	std::ofstream fw((fileName + "tmp").c_str(), std::ios::binary | std::ios::app);
    fw.seekp(pos);
	std::ifstream fr(fileName.c_str(), std::ios::binary);
	std::string line, buf = "";
	std::stringstream streamContent;
    streamContent << fr.rdbuf();

    if(fr.fail())
		std::cerr << "Erreur lors de l'ouverture en lecture du fichier " + fileName;

    buf = streamContent.str();
    buf = buf.substr(0, pos - oldValue.size());

    fr.seekg(pos);

    fw << buf << value;

    buf = streamContent.str();
    buf = buf.substr(pos, fileLength);

    fw << buf;

    fr.close();
    fw.close();
    remove(fileName.c_str());
    (void) rename((fileName+"tmp").c_str(), fileName.c_str());
}


void ResetFile(const std::string& fileName)
{
	std::ofstream fw((fileName).c_str(), std::ios::trunc);//Reset du fichier
    fw.close();
}


int GetLengthOfFile(const std::string& fileName)
{
    int x;
	std::ifstream fr(fileName.c_str(), std::ios::binary);
        // get length of file:
	fr.seekg(0, std::ios::end);
    x = (int)fr.tellg();
    return x;
}


// Supprime les espaces à droite de la chaîne.
std::string rtrim(const std::string& s)
{
	// Parcours à l'envers des caractères de la chaîne, jusqu'à trouver un
	// caractère qui n'est pas un caractère blanc.
	std::string::const_reverse_iterator it = s.rbegin();
	while (it != s.rend())
	{
		if (!isspace(*it)) break;
		++it;
	}
	// Différence entre l'itérateur de fin et celui tout juste trouvé.
	std::string::difference_type diff = s.rend() - it;
	// Effacement des caractères allant du (début + diff) à la fin.
	
	std::string result = s; 
	result.erase(s.begin() + diff, s.end());
	return result;
}

