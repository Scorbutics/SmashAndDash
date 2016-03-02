#include "TextureData.h"
#include "../Utils\StringUtils.h"

ska::TextureData::TextureData(Window& window, std::string texturePath, SDL_Color col) : m_window(&window){
	m_data = std::make_pair(texturePath, col);
}

ska::TextureData::TextureData() {
	m_data = std::make_pair("-1", SDL_Color());
}

void ska::TextureData::operator=(ska::TextureData& t2) {
	m_data = t2.m_data;
	m_window = t2.m_window;
}

ska::Window& ska::TextureData::getWindow() {
	return *m_window;
}

std::string ska::TextureData::toString() const {
	return m_data.first + ";" + ska::StringUtils::intToStr(m_data.second.a) + ";" + ska::StringUtils::intToStr(m_data.second.b) + ";" + ska::StringUtils::intToStr(m_data.second.g) + ";" + ska::StringUtils::intToStr(m_data.second.r);
}

std::pair<std::string, SDL_Color>& ska::TextureData::getData() {
	return m_data;
}

ska::TextureData::~TextureData() {
}
