#pragma once

namespace ska {
	class Ticked {
		public:
		Ticked() = default;
		virtual ~Ticked() = default;
		
		virtual float ticksWanted() const = 0;
	};
}