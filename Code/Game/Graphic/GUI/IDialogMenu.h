#pragma once
#include "Rectangle.h"
#include "Draw/Drawable.h"

class IDialogMenu : virtual public ska::Drawable {
public:
	IDialogMenu() = default;
	virtual ~IDialogMenu() = default;
};
