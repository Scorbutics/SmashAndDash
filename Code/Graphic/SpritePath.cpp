#include <iostream>
#include <sstream>

#include "SpritePath.h"
#include "Utils\IDs.h"
#include "Utils\StringUtils.h"

using namespace std;

SpritePath::SpritePath()
{
	
	m_paths.resize(SPRITEBANK_NUMBER);
	m_paths[SPRITEBANK_ANIMATION] = SPRITEBANK_PATH_ANIMATION;
	m_paths[SPRITEBANK_CHARSET] = SPRITEBANK_PATH_CHARSET;
	m_paths[SPRITEBANK_SKILL] = SPRITEBANK_PATH_SKILL;
	m_paths[SPRITEBANK_PARTICLE] = SPRITEBANK_PATH_PARTICLE;
	m_paths[SPRITEBANK_PHYSIC] = SPRITEBANK_PATH_PHYSIC;
	m_paths[SPRITEBANK_FACESET] = SPRITEBANK_PATH_FACESET;
	
}

std::string SpritePath::getPath(unsigned int spriteType, int id)
{
	return m_paths[spriteType] + FILE_SEPARATOR + (id >= 0 ? "" : "pnj") + StringUtils::intToStr(abs(id)) + ".png";
}

SpritePath::~SpritePath(){}





/*
Texture* SpriteBank::setChipset(string name, bool colorKey)
{
	m_chipset = unique_ptr<Texture>(new Texture());
    if(colorKey)
        m_chipset->load(name, T_RED, T_GREEN, T_BLUE);
    else
        m_chipset->load(name);

    return &(*m_chipset);
}

Texture* SpriteBank::getChipset()
{
	return &(*m_chipset);
}

Texture* SpriteBank::getBar()
{
    return &(*m_bar);
}

Texture* SpriteBank::getBarContent()
{
    return &(*m_barContent);
}

Texture* SpriteBank::setBar(string style)
{
	m_bar = unique_ptr<Texture>(new Texture());
    m_bar->load("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"" + style, T_RED, T_GREEN, T_BLUE);

    return &(*m_bar);
}

Texture* SpriteBank::setBarContent(string style)
{
	m_barContent = unique_ptr<Texture>(new Texture());
    m_barContent->load("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"" + style, T_RED, T_GREEN, T_BLUE);

    return &(*m_barContent);
}

void SpriteBank::add(unsigned int spritebankType, int id, bool colorKey, int alpha)
{
	stringstream ss;
	if(id >= 0 || (spritebankType != SPRITEBANK_CHARSET && spritebankType != SPRITEBANK_FACESET))
		ss << m_paths[spritebankType] << FILE_SEPARATOR << abs(id) << ".png";
	else
		ss << m_paths[spritebankType] << FILE_SEPARATOR << "pnj" << abs(id) << ".png";


	

	if(colorKey)
		m_sprites[spritebankType][id] = unique_ptr<Texture>(new Texture(ss.str(), T_RED, T_GREEN, T_BLUE, alpha));
	else
		m_sprites[spritebankType][id] = unique_ptr<Texture>(new Texture(ss.str()));
}

Texture* SpriteBank::get(unsigned int spritebankType, int id, int alpha)
{
	map<int, unique_ptr<Texture>>::iterator it;
	it = m_sprites[spritebankType].find(id);
	if(it == m_sprites[spritebankType].end())
		add(spritebankType, id, true, alpha);

	return &(*m_sprites[spritebankType][id]);
}


void SpriteBank::free(unsigned int spritebankType)
{
	for (auto &it : m_sprites[spritebankType])
		it.second = NULL;
	
    m_sprites[spritebankType].clear();
}


void SpriteBank::freeAll()
{
	for(unsigned int i = 0; i < SPRITEBANK_NUMBER; i++)
		if(i != SPRITEBANK_SKILL && i != SPRITEBANK_ANIMATION)
			this->free(i);
	
}

SpriteBank::~SpriteBank()
{
    this->freeAll();
}
*/