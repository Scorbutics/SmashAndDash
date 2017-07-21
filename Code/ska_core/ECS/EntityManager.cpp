#include "EntityManager.h"
#include "../Exceptions/IllegalStateException.h"
#include "../Utils/StringUtils.h"

std::unordered_map<std::string, ska::ComponentSerializer*> ska::EntityManager::NAME_MAPPED_COMPONENT;

void ska::EntityManager::commonRemoveComponent(EntityId entity, ComponentSerializer& components) {
	auto removedComponentMask = components.remove(entity);
    m_componentMask[entity][removedComponentMask] = false;

    m_alteredEntities.insert(entity);
}

void ska::EntityManager::refresh() {
    for(const auto& entity : m_alteredEntities) {
        notifyObservers(COMPONENT_ALTER, m_componentMask[entity], entity);
    }
    m_alteredEntities.clear();
}


void ska::EntityManager::commonAddComponent(EntityId entity, const unsigned int componentMask) {
    m_componentMask[entity][componentMask] = true;

    m_alteredEntities.insert(entity);
}


const std::string ska::EntityManager::serializeComponent(const EntityId entityId, const std::string& component, const std::string& field) const {
    if (NAME_MAPPED_COMPONENT.find(component) != NAME_MAPPED_COMPONENT.end()) {
        return NAME_MAPPED_COMPONENT[component]->getComponentField(entityId, field);
    }
    return "";
}

void ska::EntityManager::removeComponent(const EntityId entity, const std::string& component) {
    if (NAME_MAPPED_COMPONENT.find(component) != NAME_MAPPED_COMPONENT.end()) {
	    auto components = NAME_MAPPED_COMPONENT[component];
        commonRemoveComponent(entity, *components);
    }
}

void ska::EntityManager::addComponent(const EntityId entity, const std::string& component) {
    if (NAME_MAPPED_COMPONENT.find(component) != NAME_MAPPED_COMPONENT.end()) {
        ComponentSerializer * components = NAME_MAPPED_COMPONENT[component];
        commonAddComponent(entity, components->addEmpty(entity));
    }
}


ska::EntityId ska::EntityManager::createEntity() {
    EntityId newId;
    if (static_cast<int>(m_entities.size() - m_deletedEntities.size()) >= SKA_ECS_MAX_ENTITIES) {
        throw IllegalStateException("Too many entities are currently in use. Unable to create a new one. "
            "Increase SKA_ECS_MAX_ENTITIES at compile time to avoid the problem.");
    }

    if (m_deletedEntities.empty()) {
        newId = static_cast<EntityId>(m_entities.size());
        m_entities.insert(newId);
    } else {
        newId = m_deletedEntities[m_deletedEntities.size() - 1];
        m_deletedEntities.pop_back();
        m_entities.insert(newId);
    }

    /* Reset all components */
    m_componentMask[newId] &= 0;

	m_alteredEntities.insert(newId);

    return newId;
}

void ska::EntityManager::removeEntity(EntityId entity) {
    if (m_entities.find(entity) == m_entities.end() || m_entities.count(entity) <= 0) {
        auto startMessage = ("Unable to delete entity #" + StringUtils::intToStr(static_cast<int>(entity)));
        throw IllegalArgumentException (startMessage + " : this entity doesn't exist or is already deleted");
    }

    m_entities.erase(entity);

    m_deletedEntities.push_back(entity);

    /* Reset all components */
    m_componentMask[entity] &= 0;

	m_alteredEntities.insert(entity);
}

void ska::EntityManager::removeEntities(const std::unordered_set<EntityId>& exceptions) {
	auto entities = m_entities;
    for (const auto& entity : entities) {
        if (exceptions.find(entity) == exceptions.end()) {
            removeEntity(entity);
        }
    }
}

void ska::EntityManager::refreshEntity(EntityId entity) {
    notifyObservers(COMPONENT_ALTER, m_componentMask[entity], entity);
}

void ska::EntityManager::refreshEntities() {
    for (const auto& entity : m_entities) {
		notifyObservers(COMPONENT_ALTER, m_componentMask[entity], entity);
    }
}
