#pragma once
#include <unordered_set>
#include "../ScriptAwareComponent.h"
#include "../../Physic/PositionComponent.h"
#include "../ScriptSleepComponent.h"
#include "../../Inputs/InputContextManager.h"
#include "../../ECS/System.h"
#include "ScriptAutoSystem.h"

namespace ska {
	using ScriptPositionSystemAccess = System<std::unordered_set<EntityId>, PositionComponent, ScriptSleepComponent>;

	class ScriptRefreshSystem : 
		public System<std::unordered_set<EntityId>, PositionComponent, ScriptAwareComponent>, 
		/* Allows easy access to each entity that contains ScriptSleepComponent and PositionComponent */
		public ScriptPositionSystemAccess {

	public:
		ScriptRefreshSystem(const InputContextManager& icm, const unsigned int wBlockSize, Savegame& saveGame, EntityManager& entityManager);
		virtual void refresh() override;
		const EntityId findNearScriptComponentEntity(EntityManager& entityManager, const PositionComponent& entityPos) const;
		void startScript(EntityManager& entityManager, const EntityId entity);
		virtual ~ScriptRefreshSystem();
	private:
		const InputContextManager& m_icm;
		ScriptAutoSystem m_scriptAutoSystem;
		const unsigned int m_blockSize;
	};
	
}
