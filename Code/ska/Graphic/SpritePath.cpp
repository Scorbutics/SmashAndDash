#include <iostream>
#include <sstream>

#include "SpritePath.h"
#include "../Utils\SkaConstants.h"
#include "../Utils\StringUtils.h"

ska::SpritePath::SpritePath()
{
	
	m_paths.resize(SPRITEBANK_NUMBER);
	m_paths[SPRITEBANK_ANIMATION] = SPRITEBANK_PATH_ANIMATION;
	m_paths[SPRITEBANK_CHARSET] = SPRITEBANK_PATH_CHARSET;
	m_paths[SPRITEBANK_SKILL] = SPRITEBANK_PATH_SKILL;
	m_paths[SPRITEBANK_PARTICLE] = SPRITEBANK_PATH_PARTICLE;
	m_paths[SPRITEBANK_PHYSIC] = SPRITEBANK_PATH_PHYSIC;
	m_paths[SPRITEBANK_FACESET] = SPRITEBANK_PATH_FACESET;
	
}

std::string ska::SpritePath::getPath(unsigned int spriteType, int id)
{
	return m_paths[spriteType] + FILE_SEPARATOR + (id >= 0 ? "" : "pnj") + ska::StringUtils::intToStr(abs(id)) + ".png";
}

ska::SpritePath::~SpritePath(){}
