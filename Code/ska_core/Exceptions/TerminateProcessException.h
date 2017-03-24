#pragma once
#include "GenericException.h"

namespace ska {
	class TerminateProcessException : public GenericException {
	public:

		TerminateProcessException(std::string message) : TerminateProcessException(message, "TerminateProcessException") {
		}

		TerminateProcessException(std::string message, const std::string& type) : GenericException(message, type) {
		}

		virtual ~TerminateProcessException() {
		}
	};
}
