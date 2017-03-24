#pragma once
#include <string>
#include "GenericException.h"

namespace ska {
	class IllegalArgumentException : public GenericException {
	public:

		IllegalArgumentException(std::string message) : IllegalArgumentException(message, "IllegalArgumentException") {
		}

		IllegalArgumentException(std::string message, const std::string& type) : GenericException(message, type) {
		}

		virtual ~IllegalArgumentException() throw() {
		}
	};
}
