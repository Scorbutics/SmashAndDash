#include "StringUtils.h"
#include "../Exceptions/NumberFormatException.h"
#include <string>
#include <sstream>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

ska::StringUtils::StringUtils()
{
}

std::string ska::StringUtils::intToStr(const int x) {
	std::ostringstream oss;
	oss << x;
	return oss.str();
}

bool ska::StringUtils::isInt(const std::string& s, const int base) {
	if (s.empty() || std::isspace(s[0])) {
		return false;
	}
	char * p;
	strtol(s.c_str(), &p, base);
	return (*p == 0);
}

int ska::StringUtils::strToInt(const std::string& str) {
	if (!isInt(str, 10)) {
		throw NumberFormatException(str);
	}
	return atoi(str.c_str());
}

bool ska::StringUtils::isDecimal(const std::string& s) {
	if (s.empty() || std::isspace(s[0])) {
		return false;
	}
	char * p1;
	char * p2;
	strtof(s.c_str(), &p1);
	strtol(s.c_str(), &p2, 10);
	return (*p1 == 0) && (*p2 == 0);
}

float ska::StringUtils::strToFloat(const std::string& str) {
	if (!isDecimal(str)) {
		throw NumberFormatException(str);
	}
	return atof(str.c_str());
}

std::string ska::StringUtils::uintToStr(const unsigned int x) {
	std::ostringstream oss;
	oss << x;
	return oss.str();
}

std::vector<std::string>& ska::StringUtils::split(const std::string &s, const char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


std::vector<std::string> ska::StringUtils::split(const std::string &s, const char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

// trim from start
std::string& ska::StringUtils::ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
std::string& ska::StringUtils::rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
std::string & ska::StringUtils::trim(std::string &s) {
	return ltrim(rtrim(s));
}

void ska::StringUtils::replaceAll(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
}

void ska::StringUtils::replaceAll(std::string& str, const char from, const char to) {
	replaceAll(str, std::string(1, from), std::string(1, to));
}

std::string ska::StringUtils::extractTo(const size_t start, const std::string& s, const char to)
{
	const std::string subString = s.substr(start);
	size_t pos = subString.find_first_of(to);
	std::string result = "";
	if (pos != std::string::npos) {
		result = subString.substr(0, pos - start);
	}

	return result;
}

std::wstring ska::StringUtils::toUTF8(const std::string& s) {
	if (s.empty()) {
		return L"";
	}

	auto sNat = s.c_str();
	std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>, wchar_t> converter;
	return converter.from_bytes(sNat);	
}

ska::StringUtils::~StringUtils()
{
}

std::string ska::StringUtils::toANSI(const std::wstring& ws) {
	if (ws.empty()) {
		return "";
	}

	auto sNat = ws.c_str();
	std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>, wchar_t> converter;
	return converter.to_bytes(sNat);
}
