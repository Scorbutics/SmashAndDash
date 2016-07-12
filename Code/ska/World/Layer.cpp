#include <iostream>
#include <string>
#include <fstream>

#include "Layer.h"
#include "World.h"
#include "../Exceptions/CorruptedFileException.h"
#include "../Exceptions/IndexOutOfBoundsException.h"
#include "../Utils/StringUtils.h"
#include "../Exceptions/FileException.h"

//Constructeur ouvrant un monde déjà créé
ska::Layer::Layer(ska::World& w, std::string pathFile, std::string chipsetName, Layer* parent) : m_world(w), m_renderable(w) {
	m_block.reserve(20);
	m_parent = parent;
	m_fileWidth = 0; 
	m_fileHeight = 0;
    reset(pathFile, chipsetName);
}

ska::Layer::Layer(ska::World& w, Layer* parent) : m_world(w), m_renderable(w) {
}

ska::Layer* ska::Layer::getParent() const {
	return m_parent;
}

ska::LayerRenderable& ska::Layer::getRenderable() {
	return m_renderable;
}

void ska::Layer::clear() {
	m_block.clear();
}

ska::Block* ska::Layer::getBlock(const unsigned int i, const unsigned int j) const
{
	if (i < m_block.size() && j < m_block[i].size()) {
		return m_block[i][j];
	} else {
		throw ska::IndexOutOfBoundsException("block at coordinates (" + ska::StringUtils::intToStr(i) + "; " + ska::StringUtils::intToStr(j) + ") cannot be accessed");
    }

}



int ska::Layer::getBlockCollision(const unsigned int i, const unsigned int j) const {
	if (i < m_block.size() && j < m_block[i].size()) {
		Block* b = m_block[i][j];
		if (b == nullptr) {
			return BLOCK_COL_VOID;
		}
		return b->getCollision(); 
	} 
	return BLOCK_COL_YES;
}


void ska::Layer::reset(std::string pathFile, std::string chipsetName) {

    m_nomFichier = pathFile.substr(pathFile.find_last_of('/')+1, pathFile.size());
    m_name = m_nomFichier.substr(0, m_nomFichier.find_last_of('.'));

    ska::SDLSurface fichierMPng;
    
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

	auto& chipset = m_world.getChipset();
	const unsigned int blockSize = m_world.getBlockSize();

	std::vector<std::vector<BlockRenderable*>> renderableBlocks;
	m_block.resize(m_fileWidth);
	renderableBlocks.resize(m_fileWidth);
	for (int i = 0; i < m_fileWidth; i++) {
		m_block.reserve(m_fileHeight);
		renderableBlocks.reserve(m_fileHeight);
		for (int j = 0; j < m_fileHeight; j++) {
			ska::Color c = fichierMPng.getPixel32Color(i, j);
			BlockRenderable* brp = nullptr;
			Block* bp = nullptr;
			chipset.generateBlock(c, &bp, &brp);
			m_block[i].push_back(bp);
			renderableBlocks[i].push_back(brp);
        }
    }

	m_renderable.reset(renderableBlocks);

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


