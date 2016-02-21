#pragma once
#include <string>
#include <vector>
class StringUtils
{
public:
	StringUtils();
	static std::string intToStr(int x);
	static int strToInt(const std::string& str);
	static std::string uintToStr(unsigned int x);

	static std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);
	static std::vector<std::string> split(const std::string &s, char delim);
	static std::string &ltrim(std::string &s);
	static std::string &rtrim(std::string &s);
	static std::string &trim(std::string &s);
	static void replaceAll(std::string& str, const std::string& from, const std::string& to);
	static void replaceAll(std::string& str, const char from, const char to);
	static bool isInt(const std::string& s, int base);
	~StringUtils();
};

