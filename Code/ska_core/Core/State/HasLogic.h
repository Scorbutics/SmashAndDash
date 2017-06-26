#pragma once

namespace ska {
	class HasLogic {
	public:
		HasLogic() {}
		virtual ~HasLogic() {}

		virtual void eventUpdate(unsigned int ellapsedTime) = 0;
	};
}
