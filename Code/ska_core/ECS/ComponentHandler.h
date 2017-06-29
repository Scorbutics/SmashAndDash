#pragma once
#include <iostream>
#include <vector>
#include <typeinfo>
#include <string>
#include "ECSDefines.h"
#include "StaticCounterGlobal.h"
#include "ComponentSerializer.h"
#include "../Utils/Demangle.h"
#include "../Logging/Logger.h"

namespace ska {
	template <typename T>
	class ComponentHandler :
	    public StaticCounterGlobal,
        public ComponentSerializer {

	public:
		ComponentHandler(): m_mask(StaticCounterGlobal::increment()) {
			m_components.resize(SKA_ECS_MAX_ENTITIES);
			SKA_LOG_MESSAGE("Initializing component type ", getClassName(), " with mask ", m_mask);
		}

		unsigned int addEmpty(EntityId) override {
			/* We can add operations here depending of the ComponentHandler implementation */
			return m_mask;
		}

		unsigned int add(EntityId entityId, const T& comp) {
			m_components[entityId] = comp;
			return m_mask;
		}

		unsigned int remove(EntityId) override {
			/* We can add operations here depending of the ComponentHandler implementation */
			return m_mask;
		}

		T& getComponent(const EntityId id) {
			return m_components[id];
		}

		virtual std::string getComponentField(const EntityId id, const std::string& field) override {
			return m_components[id].serialize(m_components[id], field, getClassName());
		}

		unsigned int getMask() const {
			return m_mask;
		}

		virtual ~ComponentHandler() = default;

		static const std::string& getClassName() {
			static const auto fullClassName = std::string(ska::demangle(typeid(T).name()));
			static const auto startPos = fullClassName.find_last_of(':');
			static const auto& name = fullClassName.substr((startPos == std::string::npos ? -1 : startPos) + 1);
			return name;
		}

	private:
		std::vector<T> m_components;
		const unsigned int m_mask;
	};


}
