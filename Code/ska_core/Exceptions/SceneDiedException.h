#pragma once
#include "GenericException.h"

namespace ska {
	class SceneDiedException : public GenericException {
	public:

		explicit SceneDiedException(std::string message) : GenericException(message, "SceneDiedException") {
		}

		virtual ~SceneDiedException() {
		}
	
	};
}
