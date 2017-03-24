#pragma once
#include "Utils/Observer.h"
#include "../Events/TimeEvent.h"

namespace ska {
	using TimeObserver = Observer<TimeEvent>;
}
