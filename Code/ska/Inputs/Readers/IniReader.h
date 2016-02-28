#ifndef DEF_INIREADER
#define DEF_INIREADER

#include <unordered_map>
#include <iostream>
#include <list>
#include <string>
#include <sstream>

void IniListSet(std::list<std::string>& lines, std::string path, std::string content);
std::list<std::string>::iterator IniListSearchPath(std::list<std::string>& lines, std::string path);
std::list<std::string>::iterator IniListSearchBlock(std::list<std::string>& lines, std::string block);

namespace ska {
	class IniReader
	{
	public:
		IniReader(std::string inifilename);
		IniReader();
		~IniReader();

		void clear();
		bool load(std::string inifilename);
		void save(std::string inifilename);
		bool isLoaded();

		template<typename T>
		void set(std::string path, T value)
		{
			std::stringstream ss;
			ss << value;
			set(path, ss.str());
		}

		std::string getString(std::string path);
		int getInt(std::string path);
		bool getBoolean(std::string path);
		float getFloat(std::string path);
		bool get(std::string path);

	private:
		std::unordered_map<std::string, std::string> m_content;
		bool m_isLoaded;

	};

	template<>
	inline void IniReader::set<std::string>(std::string path, std::string value)
	{
		m_content[path] = value;
	}

	template<>
	inline void IniReader::set<bool>(std::string path, bool value)
	{
		IniReader::set<std::string>(path, (value ? "true" : "false"));
	}

}


#endif