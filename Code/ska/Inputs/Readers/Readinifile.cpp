#include <iostream>
#include <fstream>
#include <sstream>

#define MAXLENGTH 20

#include "Readinifile.h"


bool isReadable(string fileName)
{
    ifstream f(fileName.c_str());
    return !f.fail();
}

int FindInFile(string stringFile, string s)
{
    string content, line;
    stringstream streamContent;
    size_t pos = 0;


    ifstream f(stringFile.c_str(), ios::binary);
    if(f.fail())
    {
        cerr << "Erreur lors de l'ouverture du fichier " + stringFile;
        return -1;
    }


    streamContent << f.rdbuf();
    content = streamContent.str();

    pos = content.find(s);

    if(pos == string::npos)
        return -1; //pas trouvé
    else
        return (int)(pos + 1); //trouvé


}

int FindInFile(ifstream fbinary, string s)
{
	string content, line;
	stringstream streamContent;
	size_t pos = 0;

	streamContent << fbinary.rdbuf();
	content = streamContent.str();

	pos = content.find(s);

	if(pos == string::npos)
		return -1; //pas trouvé
	else
		return (int)(pos + 1); //trouvé

}

string GetINIValueFromFile(string stringFile, string s, string from)
{
    string line, data;
    int startPos = FindInFile(stringFile, from);
    ifstream f(stringFile.c_str(), ios::binary);

    unsigned int pos;

    if(f.fail())
        cerr << "Erreur lors de l'ouverture du fichier " + stringFile;

    if(startPos != -1)
        f.seekg(startPos + from.length());
    else
        return "STRINGNOTFOUND";

    do
    {
        getline(f, line);
        pos = (unsigned int)line.find(s);
        if (pos != string::npos)
        {
           data = line.substr(pos + s.length() + 1, line.length()-(pos+ s.length() + 1));
           if(data.size() != 0)
            return rtrim(data);
           else
            return "EMPTYDATA";
        }

    } while(line.find("[") == string::npos && !f.eof());

    return "STRINGNOTFOUND";

}

string SetINIValueToFile(string stringFile, string s, string to, string value)
{
    string line, data;
    unsigned int pos;
    int startPos = FindInFile(stringFile, to);
    ifstream fr(stringFile.c_str(), ios::binary);


    if(fr.fail())
        cerr << "Erreur lors de l'ouverture du fichier " + stringFile;

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
        if (pos != string::npos)
            pos = ((int)fr.tellg()) -2; // se place avant le \n (qui fait 2 caractères sous windows)

    } while(line.find("[") == string::npos && !fr.eof() && pos == string::npos);
    fr.close();



    if(pos != string::npos)
    {
        ReplaceStringInFile(stringFile, pos, value, GetINIValueFromFile(stringFile, s, to));
        return "STRINGFOUND";
    }
    else
        return "STRINGNOTFOUND";

}



void ReplaceStringInFile(string fileName, int pos, string value, string oldValue)
{
    ResetFile(fileName+"tmp");

    int fileLength = GetLengthOfFile(fileName);

    ofstream fw((fileName+"tmp").c_str(), ios::binary | ios::app);
    fw.seekp(pos);
    ifstream fr(fileName.c_str(), ios::binary);
    string line, buf = "";
    stringstream streamContent;
    streamContent << fr.rdbuf();

    if(fr.fail())
        cerr << "Erreur lors de l'ouverture en lecture du fichier " + fileName;

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


int HigherInt(int a, int b)
{
    if(a > b)
        return a;
    else
        return b;
}

int SmallerInt(int a, int b)
{
    if(a < b)
        return a;
    else
        return b;
}

void ResetFile(string fileName)
{
    ofstream fw((fileName).c_str(), ios::trunc);//Reset du fichier
    fw.close();
}


int GetLengthOfFile(string fileName)
{
    int x;
    ifstream fr(fileName.c_str(), ios::binary);
        // get length of file:
    fr.seekg (0, ios::end);
    x = (int)fr.tellg();
    return x;
}


// Supprime les espaces à droite de la chaîne.
string& rtrim(string& s)
{
	// Parcours à l'envers des caractères de la chaîne, jusqu'à trouver un
	// caractère qui n'est pas un caractère blanc.
	string::reverse_iterator it = s.rbegin();
	while (it != s.rend())
	{
		if (!isspace(*it)) break;
		++it;
	}
	// Différence entre l'itérateur de fin et celui tout juste trouvé.
	string::difference_type diff = s.rend() - it;
	// Effacement des caractères allant du (début + diff) à la fin.
	s.erase(s.begin() + diff, s.end());
	return s;
}

