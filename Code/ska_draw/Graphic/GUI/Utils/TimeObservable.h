#pragma once
#include "Utils/Observable.h"
#include "../Events/TimeEvent.h"

namespace ska {
	using TimeObservable = Observable<TimeEvent>;
}
