#pragma once
#include "Utils/Observer.h"
#include "../Events/KeyEvent.h"

namespace ska {
	using KeyObserver = Observer<KeyEvent>;
}
