#pragma once
#include <vector>
#include <string>
#include "../ECS/Component.h"
#define SCRIPT_TRIGGER_ACTION 2
#define SCRIPT_TRIGGER_AUTO 1

namespace ska {
	class ScriptSleepComponent : public Component {
	public:
		std::string name;
		std::string context;
		int triggeringType;
		int period;
		std::vector<std::string> args;

		ScriptSleepComponent() {
			triggeringType = 0;
			period = 0;
		}

		ScriptSleepComponent(const ScriptSleepComponent& ssc) {
			operator=(ssc);
		}

		void operator=(const ScriptSleepComponent& ssc) {
			name = ssc.name;
			context = ssc.context;
			triggeringType = ssc.triggeringType;
			period = ssc.period;
			args = ssc.args;
		}
	};
}