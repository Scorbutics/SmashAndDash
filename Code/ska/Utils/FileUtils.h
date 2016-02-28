#pragma once
#include <string>

namespace ska {
	class FileUtils
	{
	private:
		FileUtils();

	public:
		~FileUtils();
		static std::string getCurrentDirectory();
	};
}
