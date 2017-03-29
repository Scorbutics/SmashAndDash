#pragma once
#include "../../Component.h"
#include "../../../Utils/TimeUtils.h"

namespace ska {
	class DeleterComponent: public Component {
	public:
		DeleterComponent();

		int delay;
		unsigned int startTime;
	};
}
