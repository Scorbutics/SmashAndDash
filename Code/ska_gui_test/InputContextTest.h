#pragma once
#include "Inputs/InputAction.h"
#include "Inputs/MouseInput.h"
#include "Inputs/KeyInput.h"
#include "Inputs/InputContext.h"

struct InputContextTest : public ska::InputContext {

public:
	void queryActions(ska::InputActionContainer& actions) override;
	void queryRanges(ska::InputRangeContainer& ranges) override;
	void queryToggles(ska::InputToggleContainer& toggles) override;
	const ska::WindowInput& queryWindowData() override;

	const std::wstring& queryText() override {
		return str;
	}

	~InputContextTest() override = default;

	ska::WindowInput wi;
	ska::MouseInput mouseKeys;
	ska::KeyInput keys;
	std::wstring str;

protected:
	void buildCodeMap(std::unordered_map<int, ska::InputAction>& codeMap, std::unordered_map<int, ska::InputToggle>& toggles) override;
};
