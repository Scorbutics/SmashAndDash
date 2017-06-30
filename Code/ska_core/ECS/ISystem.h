#pragma once
#include "../Core/Priorized.h"

namespace ska {
	class ISystem :
	    public Priorized {
	public:
		ISystem() = default;
		virtual ~ISystem() = default;

		virtual void update(unsigned int ellapsedTime) = 0;
	};

}
