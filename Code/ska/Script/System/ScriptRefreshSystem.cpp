#include "ScriptRefreshSystem.h"
#include "../ScriptComponent.h"
#include "../../Utils/SkaConstants.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/FileUtils.h"
#include "../../ECS/EntityManager.h"
#include "../../Exceptions/InvalidPathException.h"
#include "../ScriptTriggerType.h"

using namespace std;


ska::ScriptRefreshSystem::ScriptRefreshSystem(ScriptAutoSystem& scriptAutoSystem, const InputContextManager& icm, const unsigned int wBlockSize, EntityManager& entityManager) :
System<std::unordered_set<EntityId>, PositionComponent, ScriptAwareComponent>(entityManager), 
ScriptPositionSystemAccess(entityManager),
m_icm(icm), m_blockSize(wBlockSize), m_scriptAutoSystem(scriptAutoSystem) {

}

void ska::ScriptRefreshSystem::refresh() {
	const InputActionContainer& iac = m_icm.getActions();
	EntityManager& entityManager = ScriptPositionSystemAccess::m_entityManager;

	for (EntityId entityId : System<std::unordered_set<EntityId>, PositionComponent, ScriptAwareComponent>::m_processed) {
		ScriptAwareComponent& sac = entityManager.getComponent<ScriptAwareComponent>(entityId);
		const PositionComponent& pc = entityManager.getComponent<PositionComponent>(entityId);

		if (iac[InputAction::DoAction]) {
			EntityId scriptEntity = findNearScriptComponentEntity(entityManager, pc);
			if (scriptEntity != UINT_MAX) {
				startScript(scriptEntity, entityId);
			}
		} else {
			for (EntityId targets : ScriptPositionSystemAccess::m_processed) {
				ScriptSleepComponent& scriptData = entityManager.getComponent<ScriptSleepComponent>(targets);
				if (scriptData.triggeringType == EnumScriptTriggerType::AUTO) {
					PositionComponent& scriptPos = entityManager.getComponent<PositionComponent>(targets);
					startScript(targets, entityId);
				}
			}

		}
		
	}

	m_scriptAutoSystem.refresh();

}

void ska::ScriptRefreshSystem::registerNamedScriptedEntity(const std::string& nameEntity, const EntityId entity) {
	m_scriptAutoSystem.registerNamedScriptedEntity(nameEntity, entity);
}

void ska::ScriptRefreshSystem::startScript(const EntityId scriptEntity, const EntityId origin) {
	m_scriptAutoSystem.registerScript(NULL, scriptEntity, origin);
}

const ska::EntityId ska::ScriptRefreshSystem::findNearScriptComponentEntity(EntityManager& entityManager, const PositionComponent& entityPos) const {
	const unsigned int blockSizeSquared = m_blockSize * m_blockSize;
	for (EntityId entityId : ScriptPositionSystemAccess::m_processed) {
		ScriptSleepComponent& scriptData = entityManager.getComponent<ScriptSleepComponent>(entityId);

		if (scriptData.triggeringType != EnumScriptTriggerType::ACTION) {
			continue;
		}
		PositionComponent& scriptPos = entityManager.getComponent<PositionComponent>(entityId);

		int varX = (scriptPos.x - entityPos.x);
		int varY = (scriptPos.y - entityPos.y);
		int varZ = (scriptPos.z - entityPos.z);
		unsigned int distanceSquared = varX * varX + varY * varY + varZ * varZ;
		if (distanceSquared < blockSizeSquared) {
			return entityId;
		}
	}
	return UINT_MAX;
}

ska::ScriptRefreshSystem::~ScriptRefreshSystem() {
}
