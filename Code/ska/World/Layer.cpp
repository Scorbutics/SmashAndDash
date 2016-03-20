#include <stdint.h>
#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <sstream>

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
SDL_Surface* LoadImage32(const char *fichier_image, int vram);
ska::Rectangle LocateColorInCorr(SDL_Surface* corr, SDL_Color c);
SDL_Color translate_color(Uint32 int_color);

//Constructeur ouvrant un monde déjà créé
ska::Layer::Layer(ska::EntityManager& entityManager, ska::World& w, string pathFile, string chipsetName, Layer* parent) : m_world(w), m_entityManager(entityManager) {
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

SDL_Color translate_color(Uint32 int_color)     //Change from an "int color" to an SDL_Color
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
                if(b->getID() != BLOCK_ID_AIR) {
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
		 m_block[i][j]->getCollision(); //m_block : proprietes des blocs (passable fixe, non passable fixe, passable anime, non passable anime)
	} else {
		return BLOCK_COL_NO;
	}
}

ska::Layer::~Layer()
{
}

void ska::Layer::reset(string pathFile, string chipsetName)
{

	/* TODO réécrire... */

    m_chipsetname = chipsetName;
    m_nomFichier = pathFile.substr(pathFile.find_last_of('/')+1, pathFile.size());
    m_name = m_nomFichier.substr(0, m_nomFichier.find_last_of('.'));

    SDL_Surface *fichierMPng, *fichierMCol, *fichierMCorr, *fichierMChipset, *fichierMProp;
    Uint32 pix;

    bool auto_anim;
    int collision;

    SDL_Color c;
    string buf;
    ostringstream oss;

    fichierMCorr = LoadImage32("."FILE_SEPARATOR"Chipsets"FILE_SEPARATOR"corr.png", 0);
    fichierMPng = LoadImage32((pathFile).c_str(), 0);
    fichierMCol = LoadImage32((m_chipsetname +".col").c_str(), 0);
    fichierMChipset = LoadImage32((m_chipsetname).c_str(), 0);
    fichierMProp = LoadImage32((m_chipsetname + ".prop").c_str(), 0);

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


	m_block.resize(m_fileWidth);
	for (int i = 0; i < m_fileWidth; i++) {
		m_block.reserve(m_fileHeight);
		for (int j = 0; j < m_fileHeight; j++) {

            pix = GetPixel32(fichierMPng,i,j);
            SDL_GetRGB(pix, fichierMPng->format, &c.r,&c.g,&c.b);

            if(c.r != 255 || c.g != 255 || c.b != 255)
            {
				ska::Rectangle buf_lcorr = LocateColorInCorr(fichierMCorr, c);
                if(buf_lcorr.x != -1 && buf_lcorr.y != -1)
                {
                    Uint16 prop = GetPixel32(fichierMProp, buf_lcorr.x, buf_lcorr.y);
                    Uint16 col = GetPixel32(fichierMCol, buf_lcorr.x, buf_lcorr.y);
                    if(col == whiteColor || col == lightColor)
                        collision = BLOCK_COL_NO;
                    else
                        collision = BLOCK_COL_YES;

                    auto_anim = (col == darkcolor || col == lightColor);
					
					/*EntityId entity = m_entityManager.createEntity();
					PositionComponent pc;
					pc.x = i*blockSize;
					pc.y = j*blockSize;
					pc.z = 0;
					m_entityManager.addComponent<ska::PositionComponent>(entity, pc);
					ska::GraphicComponent gc;
					gc.sprite.load(m_world.getChipsetName(), fichierMCorr->w, fichierMCorr->h, auto_anim ? 3 : 1);
					gc.sprite.setDelay(500);
					gc.priority = 0;

					if (collision) {
						ska::HitboxComponent hc;
						hc.xOffset = 0;
						hc.yOffset = 0;
						hc.width = blockSize;
						hc.height = blockSize;
						m_entityManager.addComponent<ska::HitboxComponent>(entity, hc);
					}*/

					Rectangle rectCorr = { 0, 0, blockSize, blockSize };
					
					m_block[i].push_back(BlockPtr(new Block(blockSize, rectCorr, buf_lcorr, translate_color(prop).r, auto_anim, collision)));

                } else {
					throw CorruptedFileException("Impossible de trouver la correspondance en pixel (fichier niveau corrompu)");
                }
            }
            else
            {
				ska::Rectangle rectCorr;
				rectCorr.x = 0;
				rectCorr.y = 0;
				rectCorr.w = fichierMCorr->w;
				rectCorr.h = fichierMCorr->h;
                m_block[i].push_back(BlockPtr(new Block(blockSize, rectCorr, BLOCK_ID_AIR, BLOCK_PROP_NONE, false, BLOCK_COL_VOID)));
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

void ska::Layer::printCollisionProfile()
{
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

ska::Rectangle LocateColorInCorr(SDL_Surface* corr, SDL_Color c)
{
	ska::Rectangle buf;
	SDL_Color cCmp;

	for(int x = 0; x < corr->w; x++)
        for(int y = 0; y < corr->h; y++)
            {
				SDL_GetRGB(GetPixel32(corr, x, y), corr->format,&cCmp.r, &cCmp.g, &cCmp.b);
				if(c.r == cCmp.r && c.g == cCmp.g && c.b == cCmp.b)
				{

					buf.x = x;
					buf.y = y;
					return buf;
				}
			}

    buf.x = -1;
    buf.y = -1;
    return buf;
}



Uint32 GetPixel32(SDL_Surface* image, int x, int y)
{
	if (x<0 || x>image->w-1 || y<0 || y>image->h-1)
		return 0;
	return ((Uint32*)(image->pixels))[y*(image->pitch/4)+x];   // lecture directe des pixels
}

SDL_Surface* LoadImage32(const char *fichier_image, int vram)
{
	SDL_Surface *image_result, *image_ram;
	image_ram = IMG_Load(fichier_image);

	if (image_ram == NULL)
	{
		stringstream ss;
		ss << "Erreur (fonction LoadImage32) : Impossible de charger le monde " << *fichier_image << endl;
		ss << "IMG_Load : " << IMG_GetError() << endl;
		throw ska::FileException(ss.str());
	}

	image_result = NULL;
	if (vram)
		image_result=SDL_CreateRGBSurface(0, image_ram->w, image_ram->h, 32, 0, 0, 0, 0);  // cree une imageen VRAM
	if (image_result==NULL)
		vram = 0;
	if (!vram)
		image_result=SDL_CreateRGBSurface(0, image_ram->w, image_ram->h, 32, 0, 0, 0, 0);  // cree une image en RAM

	
	SDL_UpperBlit(image_ram,NULL,image_result,NULL);	// copie l'image image_ram de moins de 32 bits vers image_result qui fait 32 bits
	SDL_FreeSurface(image_ram);      // supprime la surface image_ram : inutile maintenant --> libere la mémoire
	return image_result;
}
