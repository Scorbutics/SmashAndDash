#include <stdint.h>
#include <iostream>
#include <string>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <windows.h>
#include <sstream>

#include "Layer.h"
#include "../WGameCore.h"
#include "../../Exceptions/CorruptedFileException.h"

using namespace std;

bool LireFichierMonde(string *name, int *m_nbrBlocX, int *m_nbrBlockY, vector< vector<SDL_Rect> > &m_block, std::vector<std::vector<Uint32> > &m_propblock, int windowWidth, int windowHeight, string chipsetName);
Uint32 GetPixel32(SDL_Surface* image, int x, int y);
SDL_Surface* LoadImage32(const char *fichier_image, int vram);
SDL_Rect LocateColorInCorr(SDL_Surface* corr, SDL_Color c);
SDL_Color translate_color(Uint32 int_color);

//Constructeur ouvrant un monde déjà créé
Layer::Layer(World& w, string pathFile, string chipsetName, Layer* parent) : m_world(w) {
	m_block.reserve(20);
	m_rectAnim = { 0 };
	m_parent = parent;
	m_fileWidth = 0; 
	m_fileHeight = 0;
    this->reset(pathFile, chipsetName);
}

Layer* Layer::getParent() const {
	return m_parent;
}

bool Layer::isVisible() const {
	return !m_block.empty();
}

void Layer::clear()
{
	const size_t size = m_block.size();
	for(size_t i = 0; i < size; i++)
		m_block[i].pop_back();

	m_block.pop_back();
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

Block* Layer::getBlock(const unsigned int i, const unsigned int j)
{
    if( i < m_block.size() && j < m_block[i].size())
        return &(*m_block[i][j]);
    else
    {
        cerr << "Erreur (class Layer) : Dépassement mémoire lors de l'accès au block ( " << i << ";" << j << ")"  << endl;
        return NULL;
    }

}

void Layer::display()
{
	WGameCore& wScreen = WGameCore::getInstance();
	SDL_Rect absoluteCurrentPos, origineRelative, ofChip;
    origineRelative = wScreen.getORel();
    ofChip = wScreen.getOffsetChipset();

	const unsigned int layerPixelsX = m_world.getNbrBlocX()*TAILLEBLOC;
	const unsigned int layerPixelsY = m_world.getNbrBlocY()*TAILLEBLOC;
	const unsigned int absORelX = abs(origineRelative.x);
	const unsigned int absORelY = abs(origineRelative.y);
	const unsigned int cameraPositionStartBlockX = absORelX / TAILLEBLOC;
	const unsigned int cameraPositionStartBlockY = absORelY / TAILLEBLOC;
	const unsigned int cameraPositionEndBlockX = (absORelX + wScreen.getWidth()) / TAILLEBLOC;
	const unsigned int cameraPositionEndBlockY = (absORelY + wScreen.getHeight()) / TAILLEBLOC;
	
	for (unsigned int i = cameraPositionStartBlockX; i <= cameraPositionEndBlockX; i++)
    {
		for (unsigned int j = cameraPositionStartBlockY; j <= cameraPositionEndBlockY; j++)
        {
			unsigned int currentXBlock = i*TAILLEBLOC;
			unsigned int currentYBlock = j*TAILLEBLOC;
			absoluteCurrentPos.x = currentXBlock - absORelX;
			absoluteCurrentPos.y = currentYBlock - absORelY;

			if (currentXBlock < layerPixelsX && currentYBlock < layerPixelsY)
            {
				Block_ptr& b = m_block[i][j];
                if(b->getID() != BLOCK_ID_AIR)
                {
                    if(b->getProperties() == BLOCK_PROP_WIND_SENSITIVITY)
					{
						b->setSpriteFrame(m_world.getWind());
						b->refresh(absoluteCurrentPos);
					}
					else
						b->refresh(absoluteCurrentPos, &m_rectAnim);
                    
                }

            }

        }

    }
}

void Layer::setRectAnim(SDL_Rect rectAnim) {
	m_rectAnim = rectAnim;
}

int Layer::getBlockCollision(const unsigned int i, const unsigned int j)
{
    if(i < m_block.size() && j < m_block[i].size())
        return m_block[i][j]->getCollision(); //m_block : proprietes des blocs (passable fixe, non passable fixe, passable anime, non passable anime)
    else
        return BLOCK_COL_NO;
}

Layer::~Layer()
{
}

void Layer::reset(string pathFile, string chipsetName)
{

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
    //Uint16 blackColor =  SDL_MapRGB(fichierMChipset->format, 0, 0, 0);
    Uint16 lightColor = SDL_MapRGB(fichierMChipset->format, 170, 170, 170);
    Uint16 whiteColor =  SDL_MapRGB(fichierMChipset->format, 255, 255, 255);

	m_fileHeight = fichierMPng->h;
	m_fileWidth = fichierMPng->w;
	/* Layer coherence check */
	checkSize(m_fileWidth, m_fileHeight);

	m_world.setNbrBlocX(m_fileWidth);
	m_world.setNbrBlocY(m_fileHeight);

	for (int i = 0; i < m_fileWidth; i++)
    {
        m_block.push_back(vector<Block_ptr>());
		m_block[i].reserve(15);

		for (int j = 0; j < m_fileHeight; j++)
        {

            pix = GetPixel32(fichierMPng,i,j);
            SDL_GetRGB(pix, fichierMPng->format, &c.r,&c.g,&c.b);

            if(c.r != 255 || c.g != 255 || c.b != 255)
            {
                SDL_Rect buf_lcorr = LocateColorInCorr(fichierMCorr, c);
                if(buf_lcorr.x != -1 && buf_lcorr.y != -1)
                {
                    Uint16 prop = GetPixel32(fichierMProp, buf_lcorr.x, buf_lcorr.y);
                    Uint16 col = GetPixel32(fichierMCol, buf_lcorr.x, buf_lcorr.y);
                    if(col == whiteColor || col == lightColor)
                        collision = BLOCK_COL_NO;
                    else
                        collision = BLOCK_COL_YES;

                    auto_anim = (col == darkcolor || col == lightColor);
					
					SDL_Rect rectCorr;
					rectCorr.x = 0;
					rectCorr.y = 0;
					rectCorr.w = fichierMCorr->w;
					rectCorr.h = fichierMCorr->h;
                    m_block[i].push_back(Block_ptr(new Block(rectCorr, buf_lcorr, translate_color(prop).r, auto_anim, collision)));

                }
                else
                {
                    SDL_Rect buf_rect;
                    buf_rect.x = fichierMCorr->w + 1;
                    buf_rect.y = fichierMCorr->h + 1;

					SDL_Rect rectCorr;
					rectCorr.x = 0;
					rectCorr.y = 0;
					rectCorr.w = fichierMCorr->w;
					rectCorr.h = fichierMCorr->h;

                    m_block[i].push_back(Block_ptr(new Block(rectCorr, buf_rect, BLOCK_PROP_NONE, false, BLOCK_COL_YES)));

                    cerr << "Impossible de trouver la correspondance en pixel (fichier niveau corrompu)" << endl;
                }
            }
            else
            {
				SDL_Rect rectCorr;
				rectCorr.x = 0;
				rectCorr.y = 0;
				rectCorr.w = fichierMCorr->w;
				rectCorr.h = fichierMCorr->h;
                m_block[i].push_back(Block_ptr(new Block(rectCorr, BLOCK_ID_AIR, BLOCK_PROP_NONE, false, BLOCK_COL_VOID)));
            }
        }
    }


    SDL_FreeSurface(fichierMCol);
    SDL_FreeSurface(fichierMPng);
    SDL_FreeSurface(fichierMCorr);
    SDL_FreeSurface(fichierMChipset);
    SDL_FreeSurface(fichierMProp);

}

void Layer::checkSize(int nbrBlocX, int nbrBlocY) {
	if (m_fileWidth != nbrBlocX || m_fileHeight != nbrBlocY) {
		throw CorruptedFileException("Layer " + m_name + " has a wrong size dimension in his file " + m_nomFichier);
	}

	if (m_parent != NULL) {
		m_parent->checkSize(nbrBlocX, nbrBlocY);
	}
}

void Layer::printCollisionProfile()
{
    clog << m_name << endl;
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

    clog << endl << endl;
}

SDL_Rect LocateColorInCorr(SDL_Surface* corr, SDL_Color c)
{
	SDL_Rect buf;
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
		cerr << "Erreur (fonction LoadImage32) : Impossible de charger le monde " << *fichier_image << endl;
		cerr << "IMG_Load : " << IMG_GetError() << endl;
		SDL_Quit();
		exit(-1);
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
