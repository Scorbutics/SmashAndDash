#pragma once
#include <vector>
#include <string>
#include "../../Component.h"
#include "ScriptTriggerType.h"

namespace ska {
	class ScriptSleepComponent : public Component {
	public:
		std::string name;
		std::string context;
		ScriptTriggerType triggeringType;
		bool deleteEntityWhenFinished;
		int period;
		std::vector<std::string> args;

		ScriptSleepComponent();
		~ScriptSleepComponent() = default;

	};
}
