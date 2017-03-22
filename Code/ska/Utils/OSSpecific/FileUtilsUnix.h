#pragma once
#include "../SkaConstants.h"
#if defined(SKA_PLATFORM_LINUX)
#include <string>

namespace ska {
	class FileUtilsUnix {
	private:
		FileUtilsUnix();

	public:
		~FileUtilsUnix();
		static std::string getCurrentDirectory();
		static void createDirectory(const std::string& directoryName);
	};
}
#endif
