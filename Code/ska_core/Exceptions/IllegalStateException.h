#pragma once
#include "GenericException.h"

namespace ska {
	class IllegalStateException : public GenericException {
	public:

		IllegalStateException(std::string message) : IllegalStateException(message, "IllegalStateException") {
		}

		IllegalStateException(std::string message, const std::string& type) : GenericException(message, type) {
		}

		virtual ~IllegalStateException() {
		}
	};
}
