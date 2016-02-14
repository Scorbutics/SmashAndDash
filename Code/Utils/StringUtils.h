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
	~StringUtils();
};
