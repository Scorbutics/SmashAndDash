#pragma once
#include "GenericException.h"
namespace ska {
	class NumberFormatException : public GenericException {
	public:
		NumberFormatException(std::string message) : GenericException(message, "NumberFormatException") {
		}

		virtual ~NumberFormatException() {
		}
	};
}