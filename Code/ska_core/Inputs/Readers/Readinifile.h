#pragma once
#include <iostream>

//retourne la position de la chaine qu'on a trouvé dans le fichier (1ere occurence)
int FindInFile(const std::string& stringFile, const std::string& s);


//Retour : La valeur de la variable s du bloc from ou "EMPTYDATA" ou "STRINGNOTFOUND"
//stringFile : chaine contenant le nom du fichier ini
//s : variable dont on cherche la valeur
//from : bloc où on cherche la variable s
//Exemple d'utilisation : string enable = GetINIValueFromFile("lol.ini", "enable", "[Music]");
std::string GetINIValueFromFile(const std::string& stringFile, const std::string& s, const std::string& from);

//Retour : STRINGFOUND si le bloc "to" et la chaine de caractère "s" a été trouvé, STRINGNOTFOUND sinon
//stringFile : chaine contenant le nom du fichier ini
//s : variable dont on cherche à modifier la valeur
//to : bloc où on cherche la variable s
//Exemple d'utilisation : SetINIValueToFile("lol.ini", "enable", "[Music]", "true");
std::string SetINIValueToFile(const std::string& stringFile, const std::string& s, const std::string& to, const std::string& value);
bool isReadable(const std::string& fileName);
int GetLengthOfFile(const std::string& fileName);
void ResetFile(const std::string& fileName);
void ReplaceStringInFile(const std::string& fileName, std::size_t pos, const std::string& value, const std::string& oldValue);

std::string rtrim(const std::string& s);

