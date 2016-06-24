#pragma once
#include <unordered_set>
#include "../ScriptAwareComponent.h"
#include "../../Physic/PositionComponent.h"
#include "../../Physic/MovementComponent.h"
#include "../ScriptSleepComponent.h"
#include "../../Inputs/InputContextManager.h"
#include "../../ECS/System.h"
#include "ScriptAutoSystem.h"
#include "../../World/World.h"

namespace ska {
	using ScriptPositionSystemAccess = System<std::unordered_set<EntityId>, PositionComponent, ScriptSleepComponent>;

	class ScriptRefreshSystem : 
		public System<std::unordered_set<EntityId>, PositionComponent, MovementComponent, DirectionalAnimationComponent, HitboxComponent, ScriptAwareComponent>,
		/* Allows easy access to each entity that contains ScriptSleepComponent and PositionComponent */
		public ScriptPositionSystemAccess {

	public:
		ScriptRefreshSystem(ScriptAutoSystem& scriptAutoSystem, const InputContextManager& icm, World& world, EntityManager& entityManager);
		void registerNamedScriptedEntity(const std::string& nameEntity, const EntityId entity);
		void clearNamedScriptedEntities();
		virtual ~ScriptRefreshSystem();
		virtual void update() override;
	protected:
		virtual void refresh() override;
	private:
		const EntityId findNearScriptComponentEntity(EntityManager& entityManager, const PositionComponent& entityPos, EntityId script) const;
		void startScript(const EntityId scriptEntity, const EntityId origin);

		World& m_world;
		const InputContextManager& m_icm;
		ScriptAutoSystem& m_scriptAutoSystem;

	};
	
}
