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

using namespace std;


ska::ScriptRefreshSystem::ScriptRefreshSystem(ScriptAutoSystem& scriptAutoSystem, const InputContextManager& icm, World& world, EntityManager& entityManager) :
System<std::unordered_set<EntityId>, PositionComponent, DirectionalAnimationComponent, HitboxComponent, ScriptAwareComponent>(entityManager),
ScriptPositionSystemAccess(entityManager),
m_icm(icm), m_world(world), m_scriptAutoSystem(scriptAutoSystem) {
	
}

void ska::ScriptRefreshSystem::refresh() {
	const InputActionContainer& iac = m_icm.getActions();
	EntityManager& entityManager = ScriptPositionSystemAccess::m_entityManager;
	std::vector<EntityId> toDelete;

	for (EntityId entityId : System<std::unordered_set<EntityId>, PositionComponent, DirectionalAnimationComponent, HitboxComponent, ScriptAwareComponent>::m_processed) {
		ScriptAwareComponent& sac = entityManager.getComponent<ScriptAwareComponent>(entityId);
		const PositionComponent& pc = entityManager.getComponent<PositionComponent>(entityId);
		const ska::HitboxComponent& hc = entityManager.getComponent<HitboxComponent>(entityId);
		const ska::DirectionalAnimationComponent& dac = entityManager.getComponent<DirectionalAnimationComponent>(entityId);
		
		const ska::Point<int>& frontPos = ska::PositionComponent::getFrontPosition(pc, hc, dac);
		const ska::Point<int>& centerPos = ska::PositionComponent::getCenterPosition(pc, hc);

		for (EntityId targets : ScriptPositionSystemAccess::m_processed) {
			ScriptSleepComponent& scriptData = entityManager.getComponent<ScriptSleepComponent>(targets);
			
			EntityId scriptEntity;
			
			switch (scriptData.triggeringType) {
			case EnumScriptTriggerType::AUTO:
				startScript(targets, entityId);
				toDelete.push_back(targets);
				break;

			case EnumScriptTriggerType::ACTION:
				if (!iac[InputAction::DoAction]) {
					break;
				}
				
			case EnumScriptTriggerType::MOVE_IN:
				scriptEntity = findNearScriptComponentEntity(entityManager, pc, targets);
				if (scriptEntity != UINT_MAX) {
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

		worldScripts = m_world.chipsetScript(centerPos, EnumScriptTriggerType::AUTO);
		if (iac[InputAction::DoAction]) {
			//clog << "Enter Pressed" << std::endl;
			std::vector<ScriptSleepComponent*>& tmp = m_world.chipsetScript(frontPos, EnumScriptTriggerType::ACTION);
			worldScripts.insert(worldScripts.end(), tmp.begin(), tmp.end());
		}


		if (entityManager.hasComponent<ska::WorldCollisionComponent>(entityId)) {
			const ska::WorldCollisionComponent& wcc = entityManager.getComponent<ska::WorldCollisionComponent>(entityId);
			//clog << "Block collision" << std::endl;
			if (wcc.blockColPosX != wcc.lastBlockColPosX && wcc.blockColPosX != wcc.lastBlockColPosY ||
				wcc.blockColPosY != wcc.lastBlockColPosY && wcc.blockColPosY != wcc.lastBlockColPosX) {

				std::vector<ScriptSleepComponent*>& tmp = m_world.chipsetScript(frontPos, EnumScriptTriggerType::TOUCH);
				worldScripts.insert(worldScripts.end(), tmp.begin(), tmp.end());
			}
		}

		/* If we are moving to another block, triggers a MOVE_OUT event on previous block and MOVE_IN on the next one */
		const ska::Point<int> oldCenterPos = ska::Point<int>(sac.lastBlockPos);
		
		//TODO Other layers
		const bool sameBlock = m_world.isSameBlockId(centerPos, oldCenterPos, 0);
		if (!sameBlock) {
#ifndef NDEBUG
			auto& dgc = entityManager.getComponent<DebugGraphicComponent>(entityId);
			dgc.typeMask = DebugGraphicType::WALK;
			entityManager.addComponent<DebugGraphicComponent>(entityId, dgc);
#endif
			std::vector<ScriptSleepComponent*>& tmpOut = m_world.chipsetScript(oldCenterPos, EnumScriptTriggerType::MOVE_OUT);
			worldScripts.insert(worldScripts.end(), tmpOut.begin(), tmpOut.end());

			std::vector<ScriptSleepComponent*>& tmpIn = m_world.chipsetScript(centerPos, EnumScriptTriggerType::MOVE_IN);
			worldScripts.insert(worldScripts.end(), tmpIn.begin(), tmpIn.end());
		}

		if (oldCenterPos / blockSize != centerPos / blockSize) {
			sac.lastBlockPos = centerPos;
		}

		for (const ScriptSleepComponent* ssc : worldScripts) {
			if (ssc != nullptr) {
				ska::EntityId script = entityManager.createEntity();
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
	System<std::unordered_set<EntityId>, PositionComponent, DirectionalAnimationComponent, HitboxComponent, ScriptAwareComponent>::update();
}

void ska::ScriptRefreshSystem::registerNamedScriptedEntity(const std::string& nameEntity, const EntityId entity) {
	m_scriptAutoSystem.registerNamedScriptedEntity(nameEntity, entity);
}

void ska::ScriptRefreshSystem::clearNamedScriptedEntities() {
	m_scriptAutoSystem.clearNamedScriptedEntities();
}

void ska::ScriptRefreshSystem::startScript(const EntityId scriptEntity, const EntityId origin) {
	m_scriptAutoSystem.registerScript(NULL, scriptEntity, origin);
}

const ska::EntityId ska::ScriptRefreshSystem::findNearScriptComponentEntity(EntityManager& entityManager, const PositionComponent& entityPos, EntityId script) const {
	const unsigned int blockSizeSquared = m_world.getBlockSize() * m_world.getBlockSize();
	
	ScriptSleepComponent& scriptData = entityManager.getComponent<ScriptSleepComponent>(script);
	PositionComponent& scriptPos = entityManager.getComponent<PositionComponent>(script);

	int varX = (scriptPos.x - entityPos.x);
	int varY = (scriptPos.y - entityPos.y);
	int varZ = (scriptPos.z - entityPos.z);
	unsigned int distanceSquared = varX * varX + varY * varY + varZ * varZ;
	if (distanceSquared < blockSizeSquared) {
		return script;
	}
	
	return UINT_MAX;
}

ska::ScriptRefreshSystem::~ScriptRefreshSystem() {
}
