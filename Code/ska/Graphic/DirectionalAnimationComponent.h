#pragma once
#include "../ECS/Component.h"
#include "../Utils/StringUtils.h"
namespace ska {
	class DirectionalAnimationComponent : public Component {
	public:
		DirectionalAnimationComponent() {
			static bool initialized = false;
			if (!initialized) {
				initialized = true;
				const std::string className = getClassName(this);
				addFieldSerializer(serializeDirection, "direction", className);
			}
			direction = 0;
		}

		int direction;

	protected:
		static std::string serializeDirection(const Component& component) {
			return StringUtils::intToStr(static_cast<const DirectionalAnimationComponent&>(component).direction);
		}

	private:
		static const std::string& getClassName(Component* c) {
			static const std::string fullClassName = std::string(typeid(*c).name());
			static const size_t startPos = fullClassName.find_last_of(':');
			static const std::string& name = fullClassName.substr((startPos == std::string::npos ? -1 : startPos) + 1);
			return name;
		}
	};
}