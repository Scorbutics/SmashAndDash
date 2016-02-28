#include "TextureData.h"
#include "../Utils\StringUtils.h"

ska::TextureData::TextureData(std::string texturePath, SDL_Color col) {
	m_data = std::make_pair(texturePath, col);
}

ska::TextureData::TextureData() {
	m_data = std::make_pair("-1", SDL_Color());
}

std::string ska::TextureData::toString() const {
	return m_data.first + ";" + ska::StringUtils::intToStr(m_data.second.a) + ";" + ska::StringUtils::intToStr(m_data.second.b) + ";" + ska::StringUtils::intToStr(m_data.second.g) + ";" + ska::StringUtils::intToStr(m_data.second.r);
}

std::pair<std::string, SDL_Color>& ska::TextureData::getData() {
	return m_data;
}

ska::TextureData::~TextureData() {
}
