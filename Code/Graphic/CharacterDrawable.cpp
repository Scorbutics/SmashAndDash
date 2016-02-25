#include "../Gameplay/Character.h"
#include "../Gameplay/WGameCore.h"
#include "CharacterDrawable.h"


CharacterDrawable::CharacterDrawable(Character* parent, int part) {
	m_parent = parent;
	m_part = part;
}

void CharacterDrawable::display() {
	m_parent->display(m_part);
}

int CharacterDrawable::getPriority() const  {
	return m_parent->getCenterPos().y + WGameCore::getInstance().getORel().y;
}

CharacterDrawable::~CharacterDrawable()
{
}
