#pragma once
#include "../../Component.h"
#include "Utils/TimeUtils.h"
namespace ska {
	class DeleterComponent: public Component {
	public:
		DeleterComponent() : delay(0), startTime(TimeUtils::getTicks()) {
		}

		int delay;
		unsigned int startTime;
	};
}
