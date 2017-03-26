#pragma once
#include "GenericException.h"
namespace ska {
	class NumberFormatException : public GenericException {
	public:
		explicit NumberFormatException(std::string message) : GenericException(message, "NumberFormatException") {
		}

		virtual ~NumberFormatException() {
		}
	};
}
