#include <limits>
#include "ScriptRefreshSystem.h"
#include "Utils/FileUtils.h"
#include "ECS/EntityManager.h"
#include "ECS/Basics/Physic/WorldCollisionComponent.h"
#include "ECS/Basics/Script/ScriptTriggerType.h"
#include "ECS/Basics/Script/ScriptPositionedGetter.h"
#include "Data/BlockContainer.h"

ska::ScriptRefreshSystem::ScriptRefreshSystem(EntityManager& entityManager, ScriptAutoSystem& scriptAutoSystem, const InputContextManager& icm, ScriptPositionedGetter& spg, BlockContainer& bc) :
ska::System<std::unordered_set<EntityId>, PositionComponent, DirectionalAnimationComponent, HitboxComponent, ScriptAwareComponent>(entityManager),
ScriptPositionSystemAccess(entityManager),
m_scriptPositionedGetter(spg), m_blockContainer(bc), m_icm(icm), m_scriptAutoSystem(scriptAutoSystem) {

}

void ska::ScriptRefreshSystem::refresh() {
	const InputActionContainer& iac = m_icm.getActions();
	EntityManager& entityManager = ScriptPositionSystemAccess::m_entityManager;
	std::vector<EntityId> toDelete;

	for (const EntityId& entityId : ska::System<std::unordered_set<EntityId>, PositionComponent, DirectionalAnimationComponent, HitboxComponent, ScriptAwareComponent>::m_processed) {
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

		//TODO Déclencher un évènement
		/* World based events */
		std::vector<ScriptSleepComponent*> worldScripts;
		std::vector<ScriptTriggerType> reasons;
		const unsigned int blockSize = m_blockContainer.getBlockSize();
		const Point<int> oldCenterPos = Point<int>(sac.lastBlockPos);

		worldScripts = m_scriptPositionedGetter.chipsetScript(oldCenterPos, centerPos, centerPos, EnumScriptTriggerType::AUTO, static_cast<const unsigned int>(-1));
		if (iac[DoAction]) {
			//clog << "Enter Pressed" << std::endl;
			auto tmp = m_scriptPositionedGetter.chipsetScript(oldCenterPos, frontPos, frontPos, EnumScriptTriggerType::ACTION, 0);
			worldScripts.insert(worldScripts.end(), tmp.begin(), tmp.end());

			auto tmp2 = m_scriptPositionedGetter.chipsetScript(oldCenterPos, frontPos, frontPos, EnumScriptTriggerType::ACTION, 1);
			worldScripts.insert(worldScripts.end(), tmp2.begin(), tmp2.end());
		}

		//TODO ajouter vérification de changement de bloc ? pour éviter de détecter tous les lancements d'évènements de collisions
		if (entityManager.hasComponent<WorldCollisionComponent>(entityId)) {
			const auto& wcc = entityManager.getComponent<WorldCollisionComponent>(entityId);
			//std::clog << "Block collision" << std::endl;
			if ((wcc.blockColPosX != wcc.lastBlockColPosX && wcc.blockColPosX != wcc.lastBlockColPosY) ||
				(wcc.blockColPosY != wcc.lastBlockColPosY && wcc.blockColPosY != wcc.lastBlockColPosX)) {
				
				auto tmp = m_scriptPositionedGetter.chipsetScript(oldCenterPos, frontPos, frontPos, EnumScriptTriggerType::TOUCH, 0);
				worldScripts.insert(worldScripts.end(), tmp.begin(), tmp.end());

				auto tmp2 = m_scriptPositionedGetter.chipsetScript(oldCenterPos, frontPos, frontPos, EnumScriptTriggerType::TOUCH, 1);
				worldScripts.insert(worldScripts.end(), tmp2.begin(), tmp2.end());
			}
		}

		/* If we are moving to another block, triggers a MOVE_OUT event on previous block and MOVE_IN on the next one */
		const bool sameBlockBot = m_blockContainer.isSameBlockId(centerPos, oldCenterPos, 0);
		const bool sameBlockMid = m_blockContainer.isSameBlockId(centerPos, oldCenterPos, 1);
		if (!sameBlockBot || !sameBlockMid) {
#ifdef SKA_DEBUG_GRAPHIC
			auto& dgc = entityManager.getComponent<DebugGraphicComponent>(entityId);
			dgc.typeMask = DebugGraphicType::WALK;
			entityManager.addComponent<DebugGraphicComponent>(entityId, dgc);
#endif
			auto tmpOut = m_scriptPositionedGetter.chipsetScript(oldCenterPos, centerPos, oldCenterPos, EnumScriptTriggerType::MOVE_OUT, !sameBlockBot ? 0 : 1);
			worldScripts.insert(worldScripts.end(), tmpOut.begin(), tmpOut.end());

			auto tmpIn = m_scriptPositionedGetter.chipsetScript(oldCenterPos, centerPos, centerPos, EnumScriptTriggerType::MOVE_IN, !sameBlockBot ? 0 : 1);
			worldScripts.insert(worldScripts.end(), tmpIn.begin(), tmpIn.end());
		}

		if (oldCenterPos / blockSize != centerPos / blockSize) {
			sac.lastBlockPos = centerPos;
		}

		//TODO réception d'un évènement
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
	const unsigned int blockSizeSquared = m_blockContainer.getBlockSize() * m_blockContainer.getBlockSize();

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
