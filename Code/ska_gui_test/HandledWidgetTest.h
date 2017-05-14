#pragma once
#include "GUI/Components/HandledWidget.h"

class DisplayCounter {
public:
	DisplayCounter() {
		reset();
	}

	static void reset() {
		displayCounter = 0;
		instances.clear();
	}

	static unsigned int getDisplayCounter() {
		return displayCounter;
	}

	static auto& getInstances(){
		return instances;
	}

	virtual ~DisplayCounter() {
	}

protected:
	void increment() const {
		displayCounter++;
		instances.push_back(this);
	}

private:
	static unsigned int displayCounter;
	static std::vector<const DisplayCounter*> instances;
};

template <class ... EL>
struct HandledWidgetTest : public ska::HandledWidget<EL...>, public DisplayCounter {
	HandledWidgetTest() {
	}

	HandledWidgetTest(ska::Widget& parent) : ska::HandledWidget<EL...>(parent) {
	}

	HandledWidgetTest(ska::Widget& parent, ska::Point<int>& p) : ska::HandledWidget<EL...>(parent, p) {
	}

	void display() const override {
		DisplayCounter::increment();
	}

};
