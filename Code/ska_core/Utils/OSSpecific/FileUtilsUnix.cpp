#include "../SkaConstants.h"
#if defined(SKA_PLATFORM_LINUX)

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "FileUtilsUnix.h"
#include "../../Exceptions/FileException.h"
#include "../../Exceptions/InvalidPathException.h"

ska::FileUtilsUnix::FileUtilsUnix() {
}

std::string ska::FileUtilsUnix::getCurrentDirectory() {
   char cwd[2048];
   if (getcwd(cwd, sizeof(cwd)) == NULL) {
       throw FileException("Unknown error while getting current directory");
   }
   return std::string(cwd);
}

void ska::FileUtilsUnix::createDirectory(const std::string& directoryName) {
	struct stat st = { 0 };
	int status;

	if (stat(directoryName.c_str(), &st) == -1) {
		status = mkdir(directoryName.c_str(), 0700);
		if(status == -1) {
			throw FileException("Unknown error during creation of the directory " + directoryName);
		}
	}
}

ska::FileUtilsUnix::~FileUtilsUnix() {
}


#endif
