#pragma once
#include "../ECS/Component.h"
#include "../Utils/StringUtils.h"
#include "../Utils/PhysicUtils.h"
#include "../Graphic/Point.h"
#include "HitboxComponent.h"
#include "../Graphic/DirectionalAnimationComponent.h"
#include "../Utils/NumberUtils.h"

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

		PositionComponent(const Point<int>& p) {
			operator=(p);
		}

		static Point<int> getCenterPosition(const PositionComponent& pc, const HitboxComponent& hc) {
			Point<int> result;
			result.x = ((pc.x + hc.xOffset) + (pc.x + hc.xOffset + hc.width) + 1) / 2;
			result.y = ((pc.y + hc.yOffset) + (pc.y + hc.yOffset + hc.height) + 1) / 2;
			return result;
		}

		static Point<int> getFrontPosition(const PositionComponent& pc, const HitboxComponent& hc, const DirectionalAnimationComponent& dac) {
			Point<int> result;
			Rectangle absoluteHitbox;
			absoluteHitbox.x = (pc.x + hc.xOffset);
			absoluteHitbox.y = (pc.y + hc.yOffset);
			absoluteHitbox.w = hc.width;
			absoluteHitbox.h = hc.height;
			result.x = (absoluteHitbox.x + (absoluteHitbox.x + absoluteHitbox.w) + 1) / 2;
			result.y = (absoluteHitbox.y + (absoluteHitbox.y + absoluteHitbox.h) + 1) / 2;

			Force fDir = PhysicUtils::getMovement(dac.direction, NumberUtils::maximum((float) absoluteHitbox.w, (float) absoluteHitbox.h));
			Point<int> pos = NumberUtils::cartesian(fDir.getPower(), fDir.getAngle());
			return result + pos;
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
		void operator=(const Point<int>& p) {
			x = p.x;
			y = p.y;
			z = 0;
		}

		int x;
		int y;
		int z;
	};
}

