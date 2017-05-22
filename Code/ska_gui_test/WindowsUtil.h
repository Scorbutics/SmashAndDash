#pragma once
#include "Inputs/InputContextManager.h"
#include "Core/BaseWindow.h"
#include "Data/Events/GameEventDispatcher.h"
#include "InputContextTest.h"

ska::GameEventDispatcher& GetGED();
ska::RawInputListener& GetRawInputListener();
ska::BaseWindow& GetWindow();
InputContextTest* GetICT();
ska::InputContextManager& GetICM();
