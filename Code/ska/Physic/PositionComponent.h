#pragma once
#include "../ECS/Component.h"
#include "../Utils/StringUtils.h"
#include "../Utils/PhysicUtils.h"
#include "../Graphic/Point.h"
#include "HitboxComponent.h"
#include "../Graphic/DirectionalAnimationComponent.h"
#include "../Utils/NumberUtils.h"

namespace ska {
	class PositionComponent : public Component {
	public:
		PositionComponent() {
			static auto initialized = false;
			if (!initialized) {
				initialized = true;
				const auto className = ComponentHandler<std::remove_reference<decltype(*this)>::type>::getClassName();
				addFieldSerializer(serializeX, "x", className);
				addFieldSerializer(serializeY, "y", className);
				addFieldSerializer(serializeZ, "z", className);
			}
			x = y = z = 0;
		}

		PositionComponent(const Point<int>& p) {
			operator=(p);
		}

		static Point<int> getCenterPosition(const PositionComponent& pc, const HitboxComponent& hc) {
			Point<int> result;
			result.x = static_cast<int>(((pc.x + hc.xOffset) + (pc.x + hc.xOffset + hc.width) + 1) / 2);
			result.y = static_cast<int>(((pc.y + hc.yOffset) + (pc.y + hc.yOffset + hc.height) + 1) / 2);
			return result;
		}

		static Point<int> getFrontPosition(const PositionComponent& pc, const HitboxComponent& hc, const DirectionalAnimationComponent& dac) {
			Point<int> result;
			Rectangle absoluteHitbox;
			absoluteHitbox.x = (pc.x + hc.xOffset);
			absoluteHitbox.y = (pc.y + hc.yOffset);
			absoluteHitbox.w = static_cast<int>(hc.width);
			absoluteHitbox.h = static_cast<int>(hc.height);
			result.x = (absoluteHitbox.x + (absoluteHitbox.x + absoluteHitbox.w) + 1) / 2;
			result.y = (absoluteHitbox.y + (absoluteHitbox.y + absoluteHitbox.h) + 1) / 2;

			Force fDir = PhysicUtils::getMovement(dac.direction, NumberUtils::maximum(static_cast<float>(absoluteHitbox.w), static_cast<float>(absoluteHitbox.h)));
			Point<int> pos = NumberUtils::cartesian(fDir.getPower(), fDir.getAngle());
			return result + pos;
		}

	protected:
		static std::string serializeX(const Component& component) {
			return StringUtils::intToStr(static_cast<const PositionComponent&>(component).x);
		}
		
		static std::string serializeY(const Component& component) {
			return StringUtils::intToStr(static_cast<const PositionComponent&>(component).y);
		}

		static std::string serializeZ(const Component& component) {
			return StringUtils::intToStr(static_cast<const PositionComponent&>(component).z);
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

