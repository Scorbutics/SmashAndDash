#pragma once
#include "Inputs/InputContextManager.h"
#include "Data/Events/GameEventDispatcher.h"
#include "InputContextTest.h"

ska::GameEventDispatcher& GetGED();
ska::RawInputListener& GetRawInputListener();
InputContextTest* GetICT();
ska::InputContextManager& GetICM();
