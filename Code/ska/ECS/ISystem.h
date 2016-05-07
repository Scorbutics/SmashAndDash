#pragma once
#include <memory>
namespace ska {
	class ISystem {
	public:
		ISystem() = default;
		virtual ~ISystem() = default;

		virtual void update() = 0;
	};

}