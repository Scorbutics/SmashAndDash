#pragma once

#include "ECS/ECSDefines.h"

namespace ska {

	struct GUIEvent {
		GUIEvent() {
		}

		void operator=(const GUIEvent&) = delete;
		virtual ~GUIEvent() = default;

	};
}
