#include "FileUtils.h"

#include <Windows.h>

ska::FileUtils::FileUtils()
{
}

//TODO : PORTABILITE !!!
std::string ska::FileUtils::getCurrentDirectory() {
	CHAR NPath[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, NPath);
	return std::string(NPath);
}

ska::FileUtils::~FileUtils()
{
}
