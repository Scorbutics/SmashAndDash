#pragma once
#include "ECS/ECSDefines.h"

//TODO vou� � dispara�tre au profit d'un syst�me d'�v�nement sp�cifique aux scripts
namespace ska {
    class ScriptComponent;
    class ScriptRegisterer {
    public:
        ScriptRegisterer() = default;
        virtual ~ScriptRegisterer() = default;

        virtual void registerScript(ScriptComponent* parent, const EntityId scriptSleepEntity, const EntityId origin) = 0;
    };
}
