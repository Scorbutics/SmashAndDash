#include <limits>
#include "ScriptRefreshSystem.h"
#include "../ScriptComponent.h"
#include "../../Utils/SkaConstants.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/FileUtils.h"
#include "../../Utils/TimeUtils.h"
#include "../../ECS/EntityManager.h"
#include "../../Exceptions/InvalidPathException.h"
#include "../../Physic/WorldCollisionComponent.h"
#include "../ScriptTriggerType.h"
#include "../../Graphic/DebugGraphicComponent.h"


ska::ScriptRefreshSystem::ScriptRefreshSystem(ScriptAutoSystem& scriptAutoSystem, const InputContextManager& icm, World& world, EntityManager& entityManager) :
ska::System<std::unordered_set<EntityId>, PositionComponent, DirectionalAnimationComponent, HitboxComponent, ScriptAwareComponent>(entityManager),
ScriptPositionSystemAccess(entityManager),
m_world(world), m_icm(icm), m_scriptAutoSystem(scriptAutoSystem) {

}

void ska::ScriptRefreshSystem::refresh() {
	const InputActionContainer& iac = m_icm.getActions();
	EntityManager& entityManager = ScriptPositionSystemAccess::m_entityManager;
	std::vector<EntityId> toDelete;

	for (EntityId entityId : ska::System<std::unordered_set<EntityId>, PositionComponent, DirectionalAnimationComponent, HitboxComponent, ScriptAwareComponent>::m_processed) {
		ScriptAwareComponent& sac = entityManager.getComponent<ScriptAwareComponent>(entityId);
		const PositionComponent& pc = entityManager.getComponent<PositionComponent>(entityId);
		const HitboxComponent& hc = entityManager.getComponent<HitboxComponent>(entityId);
		const DirectionalAnimationComponent& dac = entityManager.getComponent<DirectionalAnimationComponent>(entityId);

		const Point<int>& frontPos = PositionComponent::getFrontPosition(pc, hc, dac);
		const Point<int>& centerPos = PositionComponent::getCenterPosition(pc, hc);

		for (EntityId targets : ScriptPositionSystemAccess::m_processed) {
			ScriptSleepComponent& scriptData = entityManager.getComponent<ScriptSleepComponent>(targets);

			EntityId scriptEntity;

			switch (scriptData.triggeringType) {
			case EnumScriptTriggerType::AUTO:
				startScript(targets, entityId);
				toDelete.push_back(targets);
				break;

			case EnumScriptTriggerType::ACTION:
				if (!iac[DoAction]) {
					break;
				}

			case EnumScriptTriggerType::MOVE_IN:
				scriptEntity = findNearScriptComponentEntity(entityManager, pc, targets);
				if (scriptEntity != std::numeric_limits<unsigned int>().max()) {
					startScript(scriptEntity, entityId);
				}
				break;
			default:
				break;
			}

		}

		/* World based events */
		std::vector<ScriptSleepComponent*> worldScripts;
		std::vector<ScriptTriggerType> reasons;
		const unsigned int blockSize = m_world.getBlockSize();
		const Point<int> oldCenterPos = Point<int>(sac.lastBlockPos);

		worldScripts = m_world.chipsetScript(oldCenterPos, centerPos, centerPos, EnumScriptTriggerType::AUTO, static_cast<const unsigned int>(-1));
		if (iac[DoAction]) {
			//clog << "Enter Pressed" << std::endl;
			auto tmp = m_world.chipsetScript(oldCenterPos, frontPos, frontPos, EnumScriptTriggerType::ACTION, 0);
			worldScripts.insert(worldScripts.end(), tmp.begin(), tmp.end());

			auto tmp2 = m_world.chipsetScript(oldCenterPos, frontPos, frontPos, EnumScriptTriggerType::ACTION, 1);
			worldScripts.insert(worldScripts.end(), tmp2.begin(), tmp2.end());
		}


		if (entityManager.hasComponent<WorldCollisionComponent>(entityId)) {
			const WorldCollisionComponent& wcc = entityManager.getComponent<WorldCollisionComponent>(entityId);
			//clog << "Block collision" << std::endl;
			if ((wcc.blockColPosX != wcc.lastBlockColPosX && wcc.blockColPosX != wcc.lastBlockColPosY) ||
				(wcc.blockColPosY != wcc.lastBlockColPosY && wcc.blockColPosY != wcc.lastBlockColPosX)) {

				auto tmp = m_world.chipsetScript(oldCenterPos, frontPos, frontPos, EnumScriptTriggerType::TOUCH, 0);
				worldScripts.insert(worldScripts.end(), tmp.begin(), tmp.end());

				auto tmp2 = m_world.chipsetScript(oldCenterPos, frontPos, frontPos, EnumScriptTriggerType::TOUCH, 1);
				worldScripts.insert(worldScripts.end(), tmp2.begin(), tmp2.end());
			}
		}

		/* If we are moving to another block, triggers a MOVE_OUT event on previous block and MOVE_IN on the next one */
		const bool sameBlockBot = m_world.isSameBlockId(centerPos, oldCenterPos, 0);
		const bool sameBlockMid = m_world.isSameBlockId(centerPos, oldCenterPos, 1);
		if (!sameBlockBot || !sameBlockMid) {
#ifdef SKA_DEBUG_GRAPHIC
			auto& dgc = entityManager.getComponent<DebugGraphicComponent>(entityId);
			dgc.typeMask = DebugGraphicType::WALK;
			entityManager.addComponent<DebugGraphicComponent>(entityId, dgc);
#endif
			auto tmpOut = m_world.chipsetScript(oldCenterPos, centerPos, oldCenterPos, EnumScriptTriggerType::MOVE_OUT, !sameBlockBot ? 0 : 1);
			worldScripts.insert(worldScripts.end(), tmpOut.begin(), tmpOut.end());

			auto tmpIn = m_world.chipsetScript(oldCenterPos, centerPos, centerPos, EnumScriptTriggerType::MOVE_IN, !sameBlockBot ? 0 : 1);
			worldScripts.insert(worldScripts.end(), tmpIn.begin(), tmpIn.end());
		}

		if (oldCenterPos / blockSize != centerPos / blockSize) {
			sac.lastBlockPos = centerPos;
		}

		for (const ScriptSleepComponent* ssc : worldScripts) {
			if (ssc != nullptr) {
				EntityId script = entityManager.createEntity();
				entityManager.addComponent<PositionComponent>(script, pc);
				entityManager.addComponent<ScriptSleepComponent>(script, *ssc);
				entityManager.getComponent<ScriptSleepComponent>(script).deleteEntityWhenFinished = true;
				startScript(script, entityId);
				if (ssc->triggeringType == EnumScriptTriggerType::AUTO) {
					toDelete.push_back(script);
				}
			}
		}

	}

	for (EntityId targets : toDelete) {
		entityManager.removeComponent<ScriptSleepComponent>(targets);
	}


	m_scriptAutoSystem.update();

}

void ska::ScriptRefreshSystem::update() {
	ska::System<std::unordered_set<EntityId>, PositionComponent, DirectionalAnimationComponent, HitboxComponent, ScriptAwareComponent>::update();
}

void ska::ScriptRefreshSystem::registerNamedScriptedEntity(const std::string& nameEntity, const EntityId entity) {
	m_scriptAutoSystem.registerNamedScriptedEntity(nameEntity, entity);
}

void ska::ScriptRefreshSystem::clearNamedScriptedEntities() {
	m_scriptAutoSystem.clearNamedScriptedEntities();
}

void ska::ScriptRefreshSystem::startScript(const EntityId scriptEntity, const EntityId origin) {
	m_scriptAutoSystem.registerScript(nullptr, scriptEntity, origin);
}

ska::EntityId ska::ScriptRefreshSystem::findNearScriptComponentEntity(EntityManager& entityManager, const PositionComponent& entityPos, EntityId script) const {
	const unsigned int blockSizeSquared = m_world.getBlockSize() * m_world.getBlockSize();

	//ScriptSleepComponent& scriptData = entityManager.getComponent<ScriptSleepComponent>(script);
	PositionComponent& scriptPos = entityManager.getComponent<PositionComponent>(script);

	int varX = (scriptPos.x - entityPos.x);
	int varY = (scriptPos.y - entityPos.y);
	int varZ = (scriptPos.z - entityPos.z);
	unsigned int distanceSquared = varX * varX + varY * varY + varZ * varZ;
	if (distanceSquared < blockSizeSquared) {
		return script;
	}

	return std::numeric_limits<unsigned int>().max();
}

ska::ScriptRefreshSystem::~ScriptRefreshSystem() {
}
