#pragma once
#include "../../../Utils/Observer.h"
#include "../../../Utils/Observable.h"
#include "ClickEvent.h"
#include "HoverEvent.h"


namespace ska {
	using ClickObservable = Observable<ClickEvent>;
	using HoverObservable = Observable<HoverEvent>;

	class MouseObservable : public ClickObservable, public HoverObservable {
	public:
		MouseObservable() = default;
		virtual ~MouseObservable() = default;
	};
}