#pragma once
#include "InputException.h"

namespace ska {
	class FileException : public InputException {
	public:
		FileException(std::string message) : InputException(message, "FileException") {
		}

		FileException(std::string message, const std::string& type) : InputException(message, type) {
		}

		virtual ~FileException() {
		}
	};
}
