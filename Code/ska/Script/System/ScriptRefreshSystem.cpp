#include "ScriptRefreshSystem.h"
#include "../ScriptComponent.h"
#include "../../Utils/SkaConstants.h"
#include "../../Utils/StringUtils.h"
#include "../../Utils/FileUtils.h"
#include "../../ECS/EntityManager.h"
#include "../../Exceptions/InvalidPathException.h"

using namespace std;


ska::ScriptRefreshSystem::ScriptRefreshSystem(const InputContextManager& icm, const unsigned int wBlockSize, Savegame& saveGame, EntityManager& entityManager) :
System<std::unordered_set<EntityId>, PositionComponent, ScriptAwareComponent>(entityManager), 
ScriptPositionSystemAccess(entityManager),
m_icm(icm), m_blockSize(wBlockSize), m_scriptAutoSystem(entityManager, saveGame) {

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
				startScript(entityManager, scriptEntity);
			}
		} else {
			for (EntityId entityId : ScriptPositionSystemAccess::m_processed) {
				ScriptSleepComponent& scriptData = entityManager.getComponent<ScriptSleepComponent>(entityId);
				if (scriptData.triggeringType == SCRIPT_TRIGGER_AUTO) {
					PositionComponent& scriptPos = entityManager.getComponent<PositionComponent>(entityId);
					startScript(entityManager, entityId);
				}
			}

		}
		
	}

	m_scriptAutoSystem.refresh();

}

void ska::ScriptRefreshSystem::startScript(EntityManager& entityManager, const EntityId entity) {
	ScriptSleepComponent& scriptData = entityManager.getComponent<ScriptSleepComponent>(entity);

	ScriptComponent sc;
	string extendedName;
	ifstream fscript(scriptData.name.c_str());
	string keyArgs;

	for (string& arg : scriptData.args) {
		keyArgs += arg + " ";
	}

 	ska::StringUtils::rtrim(keyArgs);

	const string& keyScript = scriptData.name + "/\\" + keyArgs;
	extendedName = keyScript + "_" + scriptData.context;

	std::string validPath;
	if (fscript.fail()) {
		const std::string& currentDir = ska::FileUtils::getCurrentDirectory();
		validPath = (currentDir + "\\" + scriptData.name);
		fscript.open(validPath.c_str());
		if (fscript.fail()) {
			throw ska::InvalidPathException("Impossible d'ouvrir le fichier script " + currentDir + "\\" + scriptData.name);
		}
	} else {
		validPath = scriptData.name;
	}

	if (!validPath.empty()) {
		sc.fullPath = validPath;
		sc.extendedName = extendedName;
		sc.active = 0;
		sc.scriptPeriod = scriptData.period == 0 ? 1 : scriptData.period;
		sc.triggeringType = scriptData.triggeringType;
		sc.extraArgs = scriptData.args;
		sc.context = scriptData.context;
		sc.key = keyScript;

		/*m_scripts[keyScript] = (move(ScriptPtr(new Script(*this, triggeringType, period == NULL || *period == 0 ? SCRIPT_DEFAULT_PERIOD : *period, validPath, extendedName, context, keyScript, args)))); */
		m_scriptAutoSystem.registerScript(NULL, sc);
		entityManager.addComponent<ScriptComponent>(entity, sc);
	} else {
		throw ska::InvalidPathException("Le script de nom " + scriptData.name + " est introuvable");
	}

}

const ska::EntityId ska::ScriptRefreshSystem::findNearScriptComponentEntity(EntityManager& entityManager, const PositionComponent& entityPos) const {
	for (EntityId entityId : ScriptPositionSystemAccess::m_processed) {
		ScriptSleepComponent& scriptData = entityManager.getComponent<ScriptSleepComponent>(entityId);

		if (scriptData.triggeringType != SCRIPT_TRIGGER_ACTION) {
			continue;
		}
		PositionComponent& scriptPos = entityManager.getComponent<PositionComponent>(entityId);

		int varX = (scriptPos.x - entityPos.x);
		int varY = (scriptPos.y - entityPos.y);
		int varZ = (scriptPos.z - entityPos.z);
		unsigned int distanceSquared = varX * varX + varY * varY + varZ * varZ;
		if (distanceSquared < m_blockSize) {
			return entityId;
		}
	}
	return UINT_MAX;
}

ska::ScriptRefreshSystem::~ScriptRefreshSystem() {
}
