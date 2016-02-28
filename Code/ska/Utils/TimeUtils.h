#pragma once

namespace ska {
	class TimeUtils {
	private:
		TimeUtils();

	public:
		static unsigned int getTicks();
		~TimeUtils();
	};
}