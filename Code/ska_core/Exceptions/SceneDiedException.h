#pragma once
#include "GenericException.h"

namespace ska {
	class SceneDiedException : public GenericException {
	public:

		SceneDiedException(std::string message) : GenericException(message, "SceneDiedException") {
		}

		virtual ~SceneDiedException() {
		}
	
	};
}
