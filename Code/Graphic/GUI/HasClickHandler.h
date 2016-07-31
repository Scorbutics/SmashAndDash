#pragma once

#include <functional>

namespace ska {
	template<typename T>
	struct Point;
}

class HasClickHandler {
public:
	HasClickHandler() = default;

	virtual void setClickHandler(std::function<void(void)> const& action) = 0;
	virtual void click(const ska::Point<int>& clickPos) = 0;
	virtual ~HasClickHandler() = default;
};