#pragma once
#include <unordered_set>
#include "ECS/Basics/Script/ScriptAwareComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "Inputs/InputContextManager.h"
#include "ECS/System.h"
#include "ScriptAutoSystem.h"

namespace ska {
    class ScriptPositionedGetter;
    class BlockContainer;

	using ScriptPositionSystemAccess = System<std::unordered_set<EntityId>, PositionComponent, ScriptSleepComponent>;

	class ScriptRefreshSystem :
		public System<std::unordered_set<EntityId>, PositionComponent, DirectionalAnimationComponent, HitboxComponent, ScriptAwareComponent>,
		/* Allows easy access to each entity that contains ScriptSleepComponent and PositionComponent */
		public ScriptPositionSystemAccess {

	public:
		ScriptRefreshSystem(EntityManager& entityManager, ScriptAutoSystem& scriptAutoSystem, const InputContextManager& icm, ScriptPositionedGetter& spg, BlockContainer& bc);
		void registerNamedScriptedEntity(const std::string& nameEntity, const EntityId entity);
		void clearNamedScriptedEntities();
		virtual ~ScriptRefreshSystem();
		virtual void update() override;
	protected:
		virtual void refresh() override;
	private:
		EntityId findNearScriptComponentEntity(EntityManager& entityManager, const PositionComponent& entityPos, EntityId script) const;
		void startScript(const EntityId scriptEntity, const EntityId origin);

		ScriptPositionedGetter& m_scriptPositionedGetter;
		BlockContainer& m_blockContainer;
		const InputContextManager& m_icm;
		ScriptAutoSystem& m_scriptAutoSystem;

	};

}
