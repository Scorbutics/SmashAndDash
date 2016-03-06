#pragma once
#include "../Core/Component/Component.h"
namespace ska {
	class PositionComponent : public Component
	{
	public:
		int x;
		int y;
		int z;

	};

	typedef std::unique_ptr<PositionComponent> PositionComponentPtr;
}
