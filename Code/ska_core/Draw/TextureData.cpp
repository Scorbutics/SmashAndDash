#include "TextureData.h"
#include "../Utils/StringUtils.h"

ska::TextureData::TextureData(SDLRenderer& r, const std::string& texturePath, Color col) :
    m_renderer(&r){
	m_data = std::make_pair(texturePath, col);
}

ska::TextureData::TextureData() :
    m_renderer(nullptr) {
}

const ska::SDLRenderer& ska::TextureData::getRenderer() const {
	return *m_renderer;
}

std::pair<std::string, ska::Color> ska::TextureData::getData() const {
	return m_data;
}

ska::TextureData::~TextureData() {
}
