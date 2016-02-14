#include "TextureData.h"
#include "Utils\StringUtils.h"

TextureData::TextureData(std::string texturePath, SDL_Color col)
{
	m_data = std::make_pair(texturePath, col);
}

TextureData::TextureData()
{
	m_data = std::make_pair("-1", SDL_Color());
}

std::string TextureData::toString() const
{
	return m_data.first + ";" + StringUtils::intToStr(m_data.second.a) + ";" + StringUtils::intToStr(m_data.second.b) + ";" + StringUtils::intToStr(m_data.second.g) + ";" + StringUtils::intToStr(m_data.second.r);
}

std::pair<std::string, SDL_Color>& TextureData::getData()
{
	return m_data;
}

TextureData::~TextureData()
{
}
