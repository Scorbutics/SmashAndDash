#pragma once
#include "../SkaConstants.h"
#if defined(SKA_PLATFORM_WIN)
#include <string>

namespace ska {
	class FileUtilsWin {
	private:
		FileUtilsWin();

	public:
		~FileUtilsWin();
		static std::string getCurrentDirectory();
		static void createDirectory(const std::string& directoryName);
	};
}
#endif
