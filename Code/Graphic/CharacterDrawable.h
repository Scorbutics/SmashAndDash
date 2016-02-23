#pragma once
#include "Drawable.h"

class Character;

class CharacterDrawable : public Drawable
{
public:
	CharacterDrawable(Character* parent, int part);
	virtual void display() override;
	virtual ~CharacterDrawable();
private:
	Character* m_parent;
	int m_part;
};

