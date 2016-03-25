#pragma once
#include <vector>
#include <string>
#include "../ECS/Component.h"
#define SCRIPT_TRIGGER_ACTION 0
#define SCRIPT_TRIGGER_AUTO 1

namespace ska {
	class ScriptSleepComponent : public Component {
	public:
		std::string name;
		std::string context;
		int triggeringType;
		int period;
		std::vector<std::string> args;
	};
}