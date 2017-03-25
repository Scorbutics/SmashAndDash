#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <unordered_map>
#include "ECS/Component.h"
#include "ScriptState.h"
#include "ECS/Basics/Script/ScriptTriggerType.h"
#include "ECS/ECSDefines.h"

namespace ska {
    class ScriptAutoSystem;
	class ScriptComponent : public Component {
		friend class ScriptAutoSystem;

	public:
		ScriptComponent() {
			state = EnumScriptState::STOPPED;
			lastTimeStarted = 0;
			commandsPlayed = 0;
			currentLine = 0;
			lastTimeDelayed = 0;
			delay = 0;
			entityId = std::numeric_limits<unsigned int>().max();
			active = 0;
			parent = nullptr;
		}

		bool deleteEntityWhenFinished;
		long scriptPeriod;
		std::string context;
		std::string extendedName;
		std::vector<std::string> extraArgs;
		ScriptTriggerType triggeringType;
		ScriptAutoSystem* parent;
		int active;
		std::unordered_map<std::string, std::string> varMap;
		std::string fullPath;
		std::string key;

		EntityId getEntity() const {
			return entityId;
		}

		EntityId getOrigin() const {
			return origin;
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
		EntityId entityId;
		EntityId origin;
	};
}
