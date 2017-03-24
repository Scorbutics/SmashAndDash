#pragma once
#include "InputException.h"

namespace ska {
	class InvalidPathException : public InputException {
	public:

		InvalidPathException(std::string message) : InputException(message, "InvalidPathException") {
		}

		virtual ~InvalidPathException() {
		}
	};
}
