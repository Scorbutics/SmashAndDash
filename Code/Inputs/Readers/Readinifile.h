#ifndef DEF_READINIFILE
#define DEF_READINIFILE

#include <iostream>

using namespace std;

//retourne la position de la chaine qu'on a trouvé dans le fichier (1ere occurence)
int FindInFile(string stringFile, string s);


//Retour : La valeur de la variable s du bloc from ou "EMPTYDATA" ou "STRINGNOTFOUND"
//stringFile : chaine contenant le nom du fichier ini
//s : variable dont on cherche la valeur
//from : bloc où on cherche la variable s
//Exemple d'utilisation : string enable = GetINIValueFromFile("lol.ini", "enable", "[Music]");
string GetINIValueFromFile(string stringFile, string s, string from);

//Retour : STRINGFOUND si le bloc "to" et la chaine de caractère "s" a été trouvé, STRINGNOTFOUND sinon
//stringFile : chaine contenant le nom du fichier ini
//s : variable dont on cherche à modifier la valeur
//to : bloc où on cherche la variable s
//Exemple d'utilisation : SetINIValueToFile("lol.ini", "enable", "[Music]", "true");
string SetINIValueToFile(string stringFile, string s, string to, string value);

//
bool isReadable(string fileName);

//
int GetLengthOfFile(string fileName);


//
void ResetFile(string fileName);

//
void ReplaceStringInFile(string fileName, int pos, string value, string oldValue);


//
int HigherInt(int a, int b);
int SmallerInt(int a, int b);

std::string& rtrim(std::string& s);
#endif
