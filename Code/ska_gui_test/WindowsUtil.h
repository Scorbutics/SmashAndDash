#pragma once
#include "Inputs/InputContextManager.h"
#include "Core/BaseWindow.h"
#include <Data/Events/GameEventDispatcher.h>

ska::GameEventDispatcher& GetGED();

ska::BaseWindow& GetWindow();

ska::InputContextManager& GetICM();