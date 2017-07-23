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
	struct InputKeyEvent;
	class ScriptPositionedGetter;
    class BlockContainer;

	using ScriptPositionSystemAccess = System<std::unordered_set<EntityId>, RequiredComponent<PositionComponent, ScriptSleepComponent>, PossibleComponent<>>;
	using ScriptRefreshSystemBase = System<std::unordered_set<EntityId>, RequiredComponent<PositionComponent, DirectionalAnimationComponent, HitboxComponent, ScriptAwareComponent>, PossibleComponent<WorldCollisionComponent>>;
	class ScriptRefreshSystem :
		public ScriptRefreshSystemBase,
		/* Allows easy access to each entity that contains ScriptSleepComponent and PositionComponent */
		public ScriptPositionSystemAccess,
		public Observer<InputKeyEvent> {

	public:
		ScriptRefreshSystem(EntityManager& entityManager, GameEventDispatcher& ged, ScriptAutoSystem& scriptAutoSystem, ScriptPositionedGetter& spg, BlockContainer& bc);
		void registerNamedScriptedEntity(const std::string& nameEntity, const EntityId entity);
		void clearNamedScriptedEntities();
		virtual ~ScriptRefreshSystem();
		virtual void update(unsigned int ellapsedTime) override;
	protected:
		virtual void refresh(unsigned int ellapsedTime) override;
	private:
		bool onKeyEvent(InputKeyEvent & ike);
		EntityId findNearScriptComponentEntity(const PositionComponent& entityPos, EntityId script) const;
		void startScript(const EntityId scriptEntity, const EntityId origin);

		GameEventDispatcher& m_eventDispatcher;
		ScriptPositionedGetter& m_scriptPositionedGetter;
		BlockContainer& m_blockContainer;
		ScriptAutoSystem& m_scriptAutoSystem;
		bool m_action;

	};

}
