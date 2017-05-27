#pragma once

namespace ska {
	class Refreshable {
	public:
		Refreshable() {}
		virtual void refresh(unsigned int ellapsedTime) = 0;
		virtual ~Refreshable() {}
	};
}
