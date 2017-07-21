#pragma once
#include <unordered_set>
#include "ECS/Basics/Script/ScriptAwareComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Script/ScriptSleepComponent.h"
#include "Inputs/InputContextManager.h"
#include "ECS/System.h"
#include "ScriptAutoSystem.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"

namespace ska {
    class ScriptPositionedGetter;
    class BlockContainer;

	using ScriptPositionSystemAccess = System<std::unordered_set<EntityId>, RequiredComponent<PositionComponent, ScriptSleepComponent>, PossibleComponent<>>;
	using ScriptRefreshSystemBase = System<std::unordered_set<EntityId>, RequiredComponent<PositionComponent, DirectionalAnimationComponent, HitboxComponent, ScriptAwareComponent>, PossibleComponent<WorldCollisionComponent>>;
	class ScriptRefreshSystem :
		public ScriptRefreshSystemBase,
		/* Allows easy access to each entity that contains ScriptSleepComponent and PositionComponent */
		public ScriptPositionSystemAccess {

	public:
		ScriptRefreshSystem(EntityManager& entityManager, ScriptAutoSystem& scriptAutoSystem, const InputContextManager& icm, ScriptPositionedGetter& spg, BlockContainer& bc);
		void registerNamedScriptedEntity(const std::string& nameEntity, const EntityId entity);
		void clearNamedScriptedEntities();
		virtual ~ScriptRefreshSystem();
		virtual void update(unsigned int ellapsedTime) override;
	protected:
		virtual void refresh(unsigned int ellapsedTime) override;
	private:
		EntityId findNearScriptComponentEntity(const PositionComponent& entityPos, EntityId script) const;
		void startScript(const EntityId scriptEntity, const EntityId origin);

		ScriptPositionedGetter& m_scriptPositionedGetter;
		BlockContainer& m_blockContainer;
		const InputContextManager& m_icm;
		ScriptAutoSystem& m_scriptAutoSystem;

	};

}
