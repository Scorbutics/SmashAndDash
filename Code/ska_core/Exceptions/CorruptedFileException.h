#pragma once
#include "FileException.h"

namespace ska {
	class CorruptedFileException : public FileException {
	public:
		CorruptedFileException(std::string message) : FileException(message, "CorruptedFileException") {
		}

		virtual ~CorruptedFileException() {
		}
	};
}
