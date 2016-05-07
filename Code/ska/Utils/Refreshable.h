#pragma once

namespace ska {
	class Refreshable {
	public:
		Refreshable() {}
		virtual void refresh() = 0;
		virtual ~Refreshable() {}
	};
}