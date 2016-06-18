#include <stdint.h>
#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <sstream>

#include "../Graphic/SDLSurface.h"
#include "../Graphic/Color.h"
#include "Layer.h"
#include "World.h"
#include "../Exceptions/CorruptedFileException.h"
#include "../Exceptions/IndexOutOfBoundsException.h"
#include "../Utils/StringUtils.h"
#include "../Utils/NumberUtils.h"
#include "../Exceptions/FileException.h"

#include "../Physic/HitboxComponent.h"
#include "../Graphic/GraphicComponent.h"

using namespace std;

ska::Color translate_color(Uint32 int_color);

//Constructeur ouvrant un monde déjà créé
ska::Layer::Layer(ska::World& w, string pathFile, string chipsetName, Layer* parent) : m_world(w) {
	m_block.reserve(20);
	m_rectAnim = { 0 };
	m_parent = parent;
	m_fileWidth = 0; 
	m_fileHeight = 0;
    reset(pathFile, chipsetName);
}

ska::Layer* ska::Layer::getParent() const {
	return m_parent;
}

bool ska::Layer::isVisible() const {
	return !m_block.empty();
}

void ska::Layer::clear() {
	m_block.clear();
}

ska::Color translate_color(Uint32 int_color)     //Change from an "int color" to an SDL_Color
{
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        SDL_Color color;
        color.r = (int_color & 0x00ff0000)/0x10000;
        color.g = (int_color & 0x0000ff00)/0x100;
        color.b = (int_color & 0x000000ff);
        color.a = 255;
    #else
        SDL_Color color;
        color.r = (int_color & 0x000000ff);
        color.g = (int_color & 0x0000ff00)/0x100;
        color.b = (int_color & 0x00ff0000)/0x10000;
        color.a = 255;
    #endif
    return color;
}

ska::Block* ska::Layer::getBlock(const unsigned int i, const unsigned int j)
{
	if (i < m_block.size() && j < m_block[i].size()) {
		return m_block[i][j].get();
	} else {
		throw ska::IndexOutOfBoundsException("block at coordinates (" + ska::StringUtils::intToStr(i) + "; " + ska::StringUtils::intToStr(j) + ") cannot be accessed");
    }

}

void ska::Layer::display() {
	
	ska::Rectangle absoluteCurrentPos;
	const ska::Rectangle* cameraPos = m_world.getView();

	if (cameraPos == NULL) {
		return;
	}

	const unsigned int layerPixelsX = m_world.getPixelWidth();
	const unsigned int layerPixelsY = m_world.getPixelHeight();
	const unsigned int absORelX = NumberUtils::absolute(cameraPos->x);
	const unsigned int absORelY = NumberUtils::absolute(cameraPos->y);
	const unsigned int cameraPositionStartBlockX = absORelX / m_world.getBlockSize();
	const unsigned int cameraPositionStartBlockY = absORelY / m_world.getBlockSize();
	const unsigned int cameraPositionEndBlockX = (absORelX + cameraPos->w) / m_world.getBlockSize();
	const unsigned int cameraPositionEndBlockY = (absORelY + cameraPos->h) / m_world.getBlockSize();
	
	for (unsigned int i = cameraPositionStartBlockX; i <= cameraPositionEndBlockX; i++) {
		for (unsigned int j = cameraPositionStartBlockY; j <= cameraPositionEndBlockY; j++) {
			unsigned int currentXBlock = i*m_world.getBlockSize();
			unsigned int currentYBlock = j*m_world.getBlockSize();
			absoluteCurrentPos.x = currentXBlock - absORelX;
			absoluteCurrentPos.y = currentYBlock - absORelY;

			if (currentXBlock < layerPixelsX && currentYBlock < layerPixelsY) {
				BlockPtr& b = m_block[i][j];
                if(b != nullptr) {
					ska::Rectangle chipsetPartRender;
                    if(b->getProperties() == BLOCK_PROP_WIND_SENSITIVITY) {
						b->setSpriteFrame(m_world.getWind());
						chipsetPartRender = b->refresh(absoluteCurrentPos);
					} else {
						chipsetPartRender = b->refresh(absoluteCurrentPos, &m_world.getChipsetAnimation().getRectOfCurrentFrame());
					}                    
					m_world.getChipset().getChipset().render(absoluteCurrentPos.x, absoluteCurrentPos.y, &chipsetPartRender);
                }
            }
        }
    }
}

void ska::Layer::setRectAnim(ska::Rectangle rectAnim) {
	m_rectAnim = rectAnim;
}

int ska::Layer::getBlockCollision(const unsigned int i, const unsigned int j) {
	if (i < m_block.size() && j < m_block[i].size()) {
		BlockPtr& b = m_block[i][j];
		if (b == nullptr) {
			return BLOCK_COL_VOID;
		}
		return b->getCollision(); //m_block : proprietes des blocs (passable fixe, non passable fixe, passable anime, non passable anime)
	} 
	return BLOCK_COL_NO;
}

ska::Layer::~Layer() {
}

void ska::Layer::reset(string pathFile, string chipsetName) {

    m_chipsetname = chipsetName;
    m_nomFichier = pathFile.substr(pathFile.find_last_of('/')+1, pathFile.size());
    m_name = m_nomFichier.substr(0, m_nomFichier.find_last_of('.'));

    ska::SDLSurface fichierMPng;
    Uint32 pix;

    fichierMPng.load32(pathFile);
	if (fichierMPng.getInstance() == nullptr) {
		throw ska::FileException("Erreur lors du chargement de la couche " + m_name + " : " + std::string(SDL_GetError()));
	}

	m_fileHeight = fichierMPng.getInstance()->h;
	m_fileWidth = fichierMPng.getInstance()->w;

	/* Layer coherence check */
	checkSize(m_fileWidth, m_fileHeight);

	m_world.setNbrBlocX(m_fileWidth);
	m_world.setNbrBlocY(m_fileHeight);

	auto& map = m_world.getChipset();
	const unsigned int blockSize = m_world.getBlockSize();

	m_block.resize(m_fileWidth);
	for (int i = 0; i < m_fileWidth; i++) {
		m_block.reserve(m_fileHeight);
		for (int j = 0; j < m_fileHeight; j++) {
			ska::Color c = fichierMPng.getPixel32Color(i, j);
			m_block[i].push_back(m_world.getChipset().generateBlock(c));
        }
    }

}

void ska::Layer::checkSize(int nbrBlocX, int nbrBlocY) {
	if (m_fileWidth != nbrBlocX || m_fileHeight != nbrBlocY) {
		throw ska::CorruptedFileException("Layer " + m_name + " has a wrong size dimension in his file " + m_nomFichier);
	}

	if (m_parent != NULL) {
		m_parent->checkSize(nbrBlocX, nbrBlocY);
	}
}

void ska::Layer::printCollisionProfile() {
}


