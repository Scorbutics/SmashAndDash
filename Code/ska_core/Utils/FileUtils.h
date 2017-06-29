#pragma once
#include "SkaConstants.h"
#include <cstdio>

namespace ska {

	template <class T>
	class FileUtilsTemplate {
	private:
		FileUtilsTemplate() = default;

	public:
		~FileUtilsTemplate() = default;

		static std::string getCurrentDirectory() {
			return T::getCurrentDirectory();
		}

		static void createDirectory(const std::string& directoryName) {
			T::createDirectory(directoryName);
		}

		static void removeFile(const std::string& filename) {
			remove(filename.c_str());
		}
	};

	
}

#if defined(SKA_PLATFORM_LINUX)
#include "./OSSpecific/FileUtilsUnix.h"
namespace ska {
	using FileUtils = FileUtilsTemplate<FileUtilsUnix>;
}
#elif defined(SKA_PLATFORM_WIN)
#include "./OSSpecific/FileUtilsWin.h"
namespace ska {
	using FileUtils = FileUtilsTemplate<FileUtilsWin>;
}
#endif
