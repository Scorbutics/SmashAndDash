#pragma once

#include "GameEventDispatcher.h"
#include "../../Utils/Observable.h"

namespace ska {
	template <class ...ET>
	class ExtensibleGameEventDispatcher :
		public ska::GameEventDispatcher,
		public ska::Observable<ET>... {
	public:
		ExtensibleGameEventDispatcher() = default;
		ExtensibleGameEventDispatcher(const ExtensibleGameEventDispatcher&) = delete;
		virtual ~ExtensibleGameEventDispatcher() = default;

	private:

	};

}
