#pragma once
#include "Utils/Observable.h"
#include "../Events/HoverEvent.h"
#include "../Events/ClickEvent.h"

namespace ska {
	using ClickObservable = Observable<ClickEvent>;
	using HoverObservable = Observable<HoverEvent>;

	class MouseObservable : public ClickObservable, public HoverObservable {
	public:
		MouseObservable() = default;
		virtual ~MouseObservable() = default;
	};
}
