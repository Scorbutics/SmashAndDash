#pragma once
#include "GenericException.h"

namespace ska {
	class StateDiedException : public GenericException {
	public:

		explicit StateDiedException(std::string message) : GenericException(message, "StateDiedException") {
		}

		virtual ~StateDiedException() {
		}
	
	};
}
