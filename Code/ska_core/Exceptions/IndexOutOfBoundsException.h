#pragma once
#include "GenericException.h"

namespace ska {
	class IndexOutOfBoundsException : public GenericException {
	public:

		IndexOutOfBoundsException(std::string message) : IndexOutOfBoundsException(message, "IndexOutOfBoundsException") {
		}

		IndexOutOfBoundsException(std::string message, const std::string& type) : GenericException(message, type) {
		}

		virtual ~IndexOutOfBoundsException() {
		}
	};
}
