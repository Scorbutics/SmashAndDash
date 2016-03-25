#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "../ECS/Component.h"
#include "ScriptState.h"

namespace ska {
	class ScriptComponent : public Component {
		friend class ScriptAutoSystem;

	public:
		ScriptComponent::ScriptComponent() {
			state = EnumScriptState::STOPPED;
			lastTimeStarted = 0;
			commandsPlayed = 0;
			currentLine = 0;
			lastTimeDelayed = 0;
			delay = 0;
			active = 0;
		}

		ScriptComponent::ScriptComponent(const ska::ScriptComponent & sc) {
			operator=(sc);
		}

		long scriptPeriod;
		std::string context;
		std::string extendedName;
		std::vector<std::string> extraArgs;
		int triggeringType;
		ScriptAutoSystem* parent;
		int active;
		std::unordered_map<std::string, std::string> varMap;
		std::string fullPath;
		std::string key;

		void operator=(const ScriptComponent& sc) {
			/* Private state */
			state = sc.state;
			lastTimeStarted = sc.lastTimeStarted;
			commandsPlayed = sc.commandsPlayed;
			currentLine = sc.currentLine;
			fullPath = sc.fullPath;
			key = sc.key;
			lastResult = sc.lastResult;
			lastTimeDelayed = sc.lastTimeDelayed;
			file = sc.file;

			/* Public state */
			scriptPeriod = sc.scriptPeriod;
			context = sc.context;
			extendedName = sc.extendedName;
			extraArgs = sc.extraArgs;
			triggeringType = sc.triggeringType;
			delay = sc.delay;
			parent = sc.parent;
			active = sc.active;
			varMap = sc.varMap;
		}

	private:
		/* Manage states in order to dispatch all different scripts */
		ScriptState state;
		unsigned int lastTimeStarted;
		unsigned int commandsPlayed;
		unsigned int currentLine;
		std::string lastResult;
		unsigned int lastTimeDelayed;
		std::vector<std::string> file;
		unsigned int delay;
	};
}