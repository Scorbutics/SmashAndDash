#pragma once
#include "Core/Rectangle.h"
#include "Core/Draw/Drawable.h"

class IDialogMenu : virtual public ska::Drawable {
public:
	IDialogMenu() = default;
	virtual ~IDialogMenu() = default;
};
