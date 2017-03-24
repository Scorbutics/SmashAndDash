#include "TextureData.h"
#include "../Utils/StringUtils.h"

ska::TextureData::TextureData(Window& window, std::string texturePath, Color col) : m_window(&window){
	m_data = make_pair(texturePath, col);
}

ska::TextureData::TextureData() {
	m_data = std::make_pair("-1", Color());
}

void ska::TextureData::operator=(const TextureData& t2) {
	m_data = t2.m_data;
	m_window = t2.m_window;
}

const ska::Window& ska::TextureData::getWindow() const {
	return *m_window;
}

std::pair<std::string, ska::Color> ska::TextureData::getData() const {
	return m_data;
}

ska::TextureData::~TextureData() {
}
