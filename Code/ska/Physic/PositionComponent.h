#pragma once
#include "../ECS/Component.h"
#include "../Utils/StringUtils.h"

namespace ska {
	class PositionComponent : public ska::Component {
	public:
		PositionComponent() {
			static bool initialized = false;
			if (!initialized) {
				initialized = true;
				const std::string className = getClassName(this);
				addFieldSerializer(PositionComponent::serializeX, "x", className);
				addFieldSerializer(PositionComponent::serializeY, "y", className);
				addFieldSerializer(PositionComponent::serializeZ, "z", className);
			}
			x = y = z = 0;
		}

	protected:
		static const std::string serializeX(const Component& component) {
			return StringUtils::intToStr(static_cast<const PositionComponent&>(component).x);
		}
		
		static const std::string serializeY(const Component& component) {
			return StringUtils::intToStr(static_cast<const PositionComponent&>(component).y);
		}

		static const std::string serializeZ(const Component& component) {
			return StringUtils::intToStr(static_cast<const PositionComponent&>(component).z);
		}
	private:
		static const std::string& getClassName(Component* c) {
			static const std::string fullClassName = std::string(typeid(*c).name());
			static const size_t startPos = fullClassName.find_last_of(':');
			static const std::string& name = fullClassName.substr((startPos == std::string::npos ? -1 : startPos) + 1);
			return name;
		}
	public:
		int x;
		int y;
		int z;
	};
}

