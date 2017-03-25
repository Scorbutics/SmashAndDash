#pragma once
#include "ECS/Basics/Script/ScriptTriggerType.h"

//TODO voué à disparaître au profit d'un Event
namespace ska {
    class ScriptPositionedGetter {
    public:
        ScriptPositionedGetter() = default;
        virtual ~ScriptPositionedGetter() = default;
        virtual std::vector<ScriptSleepComponent*> chipsetScript(const ska::Point<int>& oldCenterPos, const ska::Point<int>& p, const ska::Point<int>& centerPos, const ScriptTriggerType& stt, unsigned int i) = 0;

    };
}

