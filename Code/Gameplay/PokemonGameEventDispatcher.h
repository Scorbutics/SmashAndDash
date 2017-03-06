#pragma once

#include "../ska/Core/GameEventDispatcher.h"

template <class ...ET>
class SEventDispatcher : 
	public ska::GameEventDispatcher,
	public ska::Observable<ET>... {
public:
	SEventDispatcher() = default;
	SEventDispatcher(const SEventDispatcher&) = delete;
	virtual ~SEventDispatcher() = default;

private:

};
