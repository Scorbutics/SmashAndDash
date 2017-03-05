#pragma once
#include <string>
#include <vector>

namespace ska {
	class StringUtils
	{
	private:
		StringUtils();

	public:
		static std::string intToStr(const int x);
		static int strToInt(const std::string& str);
		static unsigned int strToUint(const std::string& str);
		static float strToFloat(const std::string& str);
		static std::string uintToStr(const unsigned int x);
		static std::string floatToStr(const float f);

		static std::vector<std::string>& split(const std::string &s, const char delim, std::vector<std::string> &elems);
		static std::vector<std::string> split(const std::string &s, const char delim);
		static std::string ltrim(const std::string &s);
		static std::string rtrim(const std::string &s);
		static std::string trim(const std::string &s);

		static std::string extractTo(const size_t start, const std::string& s, const char to);

		static std::wstring toUTF8(const std::string& s);
		static std::string toANSI(const std::wstring& ws);

		static void replaceAll(std::string& str, const std::string& from, const std::string& to);
		static void replaceAll(std::string& str, const char from, const char to);
		static bool isInt(const std::string& s, const int base);
		static bool isDecimal(const std::string& s);
		~StringUtils();

	};
}
