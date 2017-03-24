#include "../SkaConstants.h"
#if defined(SKA_PLATFORM_WIN)

#include <Windows.h>
#include "FileUtilsWin.h"
#include "../../Exceptions/FileException.h"
#include "../../Exceptions/InvalidPathException.h"

BOOL DirectoryExists(LPCTSTR szPath);

BOOL DirectoryExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}


ska::FileUtilsWin::FileUtilsWin() {
}

std::string ska::FileUtilsWin::getCurrentDirectory() {
	CHAR NPath[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, NPath);
	return std::string(NPath);
}

void ska::FileUtilsWin::createDirectory(const std::string& directoryName) {
	if (!DirectoryExists(directoryName.c_str())) {
		if (CreateDirectoryA(directoryName.c_str(), 0) == 0) {
			const DWORD& errorCode = GetLastError();
			if (errorCode == ERROR_PATH_NOT_FOUND) {
				throw InvalidPathException("Unable to create directory " + directoryName);
			}
			else if (errorCode == ERROR_ALREADY_EXISTS) {
				throw FileException("Directory already exists : " + directoryName);
			}
			else {
				throw FileException("Unknown error during creation of the directory " + directoryName);
			}
		}
	}

}

ska::FileUtilsWin::~FileUtilsWin() {
}


#endif
