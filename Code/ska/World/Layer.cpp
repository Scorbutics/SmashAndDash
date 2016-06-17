#include <stdint.h>
#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <sstream>

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

bool LireFichierMonde(string *name, int *m_nbrBlocX, int *m_nbrBlockY, vector< vector<ska::Rectangle> > &m_block, std::vector<std::vector<Uint32> > &m_propblock, int windowWidth, int windowHeight, string chipsetName);
Uint32 GetPixel32(SDL_Surface* image, int x, int y);
SDL_Surface* LoadImage32(const std::string& fichier_image);
void BuildCorrMap(SDL_Surface* corr, std::unordered_map<ska::Color, ska::Point<int>>& map);
ska::Color translate_color(Uint32 int_color);
ska::Color GetPixelColor(SDL_Surface* image, int x, int y);

//Constructeur ouvrant un monde déjà créé
ska::Layer::Layer(ska::World& w, string pathFile, string chipsetName, Layer* parent) : m_world(w) {
	m_block.reserve(20);
	m_rectAnim = { 0 };
	m_parent = parent;
	m_fileWidth = 0; 
	m_fileHeight = 0;
    this->reset(pathFile, chipsetName);
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

void ska::Layer::display()
{
	//WGameCore& wScreen = WGameCore::getInstance();
	ska::Rectangle absoluteCurrentPos, ofChip;
	const ska::Rectangle* cameraPos = m_world.getView();
	//origineRelative = wScreen.getORel();
    //ofChip = wScreen.getOffsetChipset();

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
					m_world.getChipset().render(absoluteCurrentPos.x, absoluteCurrentPos.y, &chipsetPartRender);
                }
            }
        }
    }
}

void ska::Layer::setRectAnim(ska::Rectangle rectAnim) {
	m_rectAnim = rectAnim;
}

int ska::Layer::getBlockCollision(const unsigned int i, const unsigned int j)
{
	if (i < m_block.size() && j < m_block[i].size()) {
		//return m_entityManager.hasComponent<ska::HitboxComponent>(m_block[i][j]);
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

	/* TODO réécrire... */

    m_chipsetname = chipsetName;
    m_nomFichier = pathFile.substr(pathFile.find_last_of('/')+1, pathFile.size());
    m_name = m_nomFichier.substr(0, m_nomFichier.find_last_of('.'));

    SDL_Surface *fichierMPng, *fichierMCol, *fichierMCorr, *fichierMChipset, *fichierMProp;
    Uint32 pix;

    fichierMCorr = LoadImage32("."FILE_SEPARATOR"Chipsets"FILE_SEPARATOR"corr.png");
    fichierMPng = LoadImage32(pathFile);
    fichierMCol = LoadImage32(m_chipsetname + ".col");
    fichierMChipset = LoadImage32(m_chipsetname);
    fichierMProp = LoadImage32(m_chipsetname + ".prop");

    Uint16 darkcolor = SDL_MapRGB(fichierMChipset->format, 70, 70, 70);
    Uint16 lightColor = SDL_MapRGB(fichierMChipset->format, 170, 170, 170);
    Uint16 whiteColor =  SDL_MapRGB(fichierMChipset->format, 255, 255, 255);

	m_fileHeight = fichierMPng->h;
	m_fileWidth = fichierMPng->w;

	/* Layer coherence check */
	checkSize(m_fileWidth, m_fileHeight);

	m_world.setNbrBlocX(m_fileWidth);
	m_world.setNbrBlocY(m_fileHeight);

	const unsigned int blockSize = m_world.getBlockSize();

	std::unordered_map<ska::Color, ska::Point<int>> map;
	BuildCorrMap(fichierMCorr, map);

	m_block.resize(m_fileWidth);
	for (int i = 0; i < m_fileWidth; i++) {
		m_block.reserve(m_fileHeight);
		for (int j = 0; j < m_fileHeight; j++) {

			ska::Color c = GetPixelColor(fichierMPng, i, j);
            if(c.r != 255 || c.g != 255 || c.b != 255) {
                if(map.find(c) != map.end()) {
					ska::Point<int> posCorr = map.at(c);
					Uint16 prop = GetPixel32(fichierMProp, posCorr.x, posCorr.y);
					Uint16 col = GetPixel32(fichierMCol, posCorr.x, posCorr.y);
					
					int collision = (col == whiteColor || col == lightColor) ? BLOCK_COL_NO : BLOCK_COL_YES;
                    bool auto_anim = (col == darkcolor || col == lightColor);

					m_block[i].push_back(BlockPtr(new Block(blockSize, fichierMCorr->w, posCorr, translate_color(prop).r, auto_anim, collision)));
                } else {
					throw CorruptedFileException("Impossible de trouver la correspondance en pixel (fichier niveau corrompu)");
                }
            } else {
				m_block[i].push_back(BlockPtr(nullptr));
            }
        }
    }


    SDL_FreeSurface(fichierMCol);
    SDL_FreeSurface(fichierMPng);
    SDL_FreeSurface(fichierMCorr);
    SDL_FreeSurface(fichierMChipset);
    SDL_FreeSurface(fichierMProp);

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
    /*clog << m_name << endl;
    for(int y = 0; y < m_world.getNbrBlocY(); y++)
    {
		for (int x = 0; x < m_world.getNbrBlocX(); x++)
        {
            if(m_block[x][y]->getCollision() != BLOCK_COL_YES)
                clog << " ";
            else
                clog << 0;

            clog << " ";
        }

        clog << endl;
    }

    clog << endl << endl;*/
}

void BuildCorrMap(SDL_Surface* corr, std::unordered_map<ska::Color, ska::Point<int>>& map) {
	for (int x = 0; x < corr->w; x++) {
		for (int y = 0; y < corr->h; y++) {
			ska::Color c = GetPixelColor(corr, x, y);
			if (map.find(c) == map.end()) {
				map.insert(std::make_pair(c, ska::Point<int>(x, y)));
			} else {
				ska::Point<int>& pos2 = map[c];
				throw ska::CorruptedFileException("Chipset correspondance file (corr.png) has several tiles with same color at (" 
					+ ska::StringUtils::intToStr(pos2.x) + "; " + ska::StringUtils::intToStr(pos2.y) + ") and (" + ska::StringUtils::intToStr(x) + "; " + ska::StringUtils::intToStr(y) 
					+ "). Please keep only one color correspondance per tile");
			}
		}
	}

}

ska::Color GetPixelColor(SDL_Surface* image, int x, int y) {
	SDL_Color c;
	c.a = 0;
	Uint32 pix = GetPixel32(image, x, y);
	SDL_GetRGB(pix, image->format, &c.r, &c.g, &c.b);
	return c;
}

Uint32 GetPixel32(SDL_Surface* image, int x, int y) {
	if (x < 0 || x > image->w - 1 || y < 0 || y > image->h - 1) {
		return 0;
	}
	return ((Uint32*)(image->pixels))[y*(image->pitch/4)+x];
}

SDL_Surface* LoadImage32(const std::string& fichier_image) {
	SDL_Surface *result; 
	SDL_Surface *imageRam;
	imageRam = IMG_Load(fichier_image.c_str());

	result = SDL_CreateRGBSurface(0, imageRam->w, imageRam->h, 32, 0, 0, 0, 0);
	if (result == NULL || imageRam == NULL) {
		stringstream ss;
		ss << "Erreur (fonction LoadImage32) : Impossible de charger le monde " << fichier_image << endl;
		ss << "IMG_Load : " << IMG_GetError() << endl;
		throw ska::FileException(ss.str());
	}
		
	/* Copie l'image image_ram de moins de 32 bits vers image_result qui fait 32 bits */
	SDL_UpperBlit(imageRam, NULL, result, NULL);
	SDL_FreeSurface(imageRam);
	imageRam = NULL;
	return result;
}
