#pragma once
#include "ECS/ECSDefines.h"

//TODO voué à disparaître au profit d'un système d'évènement spécifique aux scripts
namespace ska {
    class ScriptComponent;
    class ScriptRegisterer {
    public:
        ScriptRegisterer() = default;
        virtual ~ScriptRegisterer() = default;

        virtual void registerScript(ScriptComponent* parent, const EntityId scriptSleepEntity, const EntityId origin) = 0;
    };
}
