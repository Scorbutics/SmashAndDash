#include "FileUtils.h"

#include <Windows.h>

ska::FileUtils::FileUtils()
{
}

std::string ska::FileUtils::getCurrentDirectory() {
	TCHAR NPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, NPath);
	return std::string(NPath);
}

ska::FileUtils::~FileUtils()
{
}
