#include "../Gameplay/Character.h"
#include "CharacterDrawable.h"


CharacterDrawable::CharacterDrawable(Character* parent, int part) {
	m_parent = parent;
	m_part = part;
}

void CharacterDrawable::display() {
	m_parent->display(m_part);
}

CharacterDrawable::~CharacterDrawable()
{
}
