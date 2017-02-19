#pragma once

#include <unordered_map>
#include <iostream>
#include <list>
#include <string>
#include <sstream>

#include "../../Utils/StringUtils.h"
#include "../../Exceptions/InvalidPathException.h"
#include "../../Exceptions/IllegalArgumentException.h"

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
		bool isLoaded() const;
		bool exists(std::string path) const;

		template<typename T>
		void set(const std::string& path, const T& value) {
			std::stringstream ss;
			ss << value;
			set(path, ss.str());
		}
		
		template<typename T>
		inline T get(const std::string& path) const {
			/* unimplemented default */
			throw ska::IllegalArgumentException("Unsupported converted type for path " + path);
		}

	private:
		void existsOrThrow(const std::string& path) const {
			if (!exists(path)) {
				throw ska::InvalidPathException("Unable to find the ini path " + path);
			}
		}

		std::unordered_map<std::string, std::string> m_content;
		bool m_isLoaded;

	};


	/* set specializations */
	template<>
	inline void IniReader::set<std::string>(const std::string& path, const std::string& value) {
		m_content[path] = value;
	}

	template<>
	inline void IniReader::set<bool>(const std::string& path, const bool& value) {
		IniReader::set<std::string>(path, (value ? "true" : "false"));
	}

	/* get specializations */
	template<>
	inline std::string IniReader::get(const std::string& path) const {
		existsOrThrow(path);
		return m_content.at(path);
	}

	template<>
	inline int IniReader::get(const std::string& path) const {
		return ska::StringUtils::strToInt(get<std::string>(path));
	}

	template<>
	inline unsigned int IniReader::get(const std::string& path) const {
		return ska::StringUtils::strToUint(get<std::string>(path));
	}

	template<>
	inline bool IniReader::get(const std::string& path) const {
		return get<std::string>(path) == "true";
	}

	template<>
	inline float IniReader::get(const std::string& path) const {
		const std::string& s = get<std::string>(path);
		std::stringstream ss;
		float f;
		ss << s;
		ss >> f;
		return f;
	}

}
