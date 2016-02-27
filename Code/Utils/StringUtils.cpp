#include "StringUtils.h"
#include "../Exceptions/NumberFormatException.h"
#include <string>
#include <sstream>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

using namespace std;

StringUtils::StringUtils()
{
}

string StringUtils::intToStr(int x)
{
	// créer un flux de sortie
	ostringstream oss;
	string buf;

	oss << x; //on ecrit la valeur de l'entier x dedans
	buf = oss.str(); //on le transforme en string

	oss.clear();
	return buf;
}

bool StringUtils::isInt(const std::string& s, int base) {
	if (s.empty() || std::isspace(s[0])) return false;
	char * p;
	strtol(s.c_str(), &p, base);
	return (*p == 0);
}

int StringUtils::strToInt(const std::string& str) {
	if (!isInt(str, 10)) {
		throw NumberFormatException(str);
	}
	return atoi(str.c_str());
}

string StringUtils::uintToStr(unsigned int x)
{
	ostringstream oss;
	string buf;

	oss << x; //on ecrit la valeur de l'entier x dedans
	buf = oss.str(); //on le transforme en string

	oss.clear();
	return buf;
}

vector<string>& StringUtils::split(const std::string &s, char delim, vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


vector<string> StringUtils::split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

// trim from start
std::string& StringUtils::ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
std::string& StringUtils::rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
std::string & StringUtils::trim(std::string &s) {
	return ltrim(rtrim(s));
}

void StringUtils::replaceAll(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
}

void StringUtils::replaceAll(std::string& str, const char from, const char to) {
	replaceAll(str, string(1, from), string(1, to));
}

string StringUtils::extractTo(int depart, std::string chaine, char jusquaCeCaractere, int *entier)
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

StringUtils::~StringUtils()
{
}
