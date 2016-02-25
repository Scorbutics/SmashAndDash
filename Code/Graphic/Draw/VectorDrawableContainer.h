#pragma once
#include <vector>
#include <functional>

#include "DrawableContainer.h"

class VectorDrawableContainer :
	public DrawableContainer
{
public:
	VectorDrawableContainer();
	virtual void push(Drawable& d) override;
	virtual void draw() override;
	virtual ~VectorDrawableContainer();

private:
	/* TODO : smart pointers ? */
	std::vector<Drawable*> m_data;
};

