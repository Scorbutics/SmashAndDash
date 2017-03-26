#pragma once

namespace ska {
	class HasLogic {
	public:
		HasLogic() {}
		virtual ~HasLogic() {}

		virtual void eventUpdate(bool movingDisallowed) = 0;
	};
}
