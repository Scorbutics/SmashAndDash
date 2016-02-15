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

using namespace std;

bool LireFichierMonde(string *name, int *m_nbrBlocX, int *m_nbrBlockY, vector< vector<SDL_Rect> > &m_block, std::vector<std::vector<Uint32> > &m_propblock, int windowWidth, int windowHeight, string chipsetName);
Uint32 GetPixel32(SDL_Surface* image, int x, int y);
SDL_Surface* LoadImage32(const char *fichier_image, int vram);
SDL_Rect LocateColorInCorr(SDL_Surface* corr, SDL_Color c);
SDL_Color translate_color(Uint32 int_color);

//Constructeur ouvrant un monde déjà créé
Layer::Layer(string pathFile, string chipsetName, int windowWidth, int windowHeight)
{

    m_windowHeight = windowHeight;
    m_windowWidth = windowWidth;
    m_windDirection = WIND_STOP;
	m_block.reserve(20);
    this->reset(pathFile, chipsetName);
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

void Layer::display(SDL_Rect rectAnim)
{
	WGameCore& wScreen = WGameCore::getInstance();
    SDL_Rect positionFond, origineRelative, ofChip;
    origineRelative = wScreen.getORel();
    ofChip = wScreen.getOffsetChipset();

    for(int i = abs(origineRelative.x)/TAILLEBLOC; i <= (abs(origineRelative.x)+ (int)wScreen.getWidth())/TAILLEBLOC; i++)
    {
        for(int j = abs(origineRelative.y)/TAILLEBLOC; j <= (abs(origineRelative.y)+(int)wScreen.getHeight())/TAILLEBLOC; j++)
        {

            positionFond.x = i*TAILLEBLOC - abs(origineRelative.x);
            positionFond.y = j*TAILLEBLOC - abs(origineRelative.y);

            if(i*TAILLEBLOC < m_nbrBlocX*TAILLEBLOC && j*TAILLEBLOC < m_nbrBlocY*TAILLEBLOC)
            {
                if(m_block[i][j]->getID() != BLOCK_ID_AIR)
                {
                    if(m_block[i][j]->getProperties() == BLOCK_PROP_WIND_SENSITIVITY)
					{
                        m_block[i][j]->setSpriteFrame(m_windDirection);
						m_block[i][j]->refresh(positionFond);
					}
					else
						m_block[i][j]->refresh(positionFond, &rectAnim);
                    
                }

            }
            /*else
                SDL_FillRect(screen, &positionFond, 0);*/

        }

    }
}


int Layer::getNbrBlocX()
{
    return m_nbrBlocX;
}

int Layer::getNbrBlocY()
{
    return m_nbrBlocY;
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

    m_nbrBlocX = 0;
    m_nbrBlocY = 0;

    fichierMCorr = LoadImage32("."FILE_SEPARATOR"Chipsets"FILE_SEPARATOR"corr.png", 0);
    fichierMPng = LoadImage32((pathFile).c_str(), 0);
    fichierMCol = LoadImage32((m_chipsetname +".col").c_str(), 0);
    fichierMChipset = LoadImage32((m_chipsetname).c_str(), 0);
    fichierMProp = LoadImage32((m_chipsetname + ".prop").c_str(), 0);

    Uint16 darkcolor = SDL_MapRGB(fichierMChipset->format, 70, 70, 70);
    //Uint16 blackColor =  SDL_MapRGB(fichierMChipset->format, 0, 0, 0);
    Uint16 lightColor = SDL_MapRGB(fichierMChipset->format, 170, 170, 170);
    Uint16 whiteColor =  SDL_MapRGB(fichierMChipset->format, 255, 255, 255);

    m_nbrBlocX = fichierMPng->w;
    m_nbrBlocY = fichierMPng->h;


    for(int i = 0; i < m_nbrBlocX; i++)
    {
        m_block.push_back(vector<Block_ptr>());
		m_block[i].reserve(15);

        for(int j = 0; j < m_nbrBlocY; j++)
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

void Layer::printCollisionProfile()
{
    clog << m_name << endl;
    for(int y = 0; y < m_nbrBlocY; y++)
    {
        for(int x = 0; x < m_nbrBlocX; x++)
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

void Layer::setWind(int windDirection)
{
    m_windDirection = windDirection;
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

/*
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
		image_result=SDL_CreateRGBSurface(SDL_HWSURFACE, image_ram->w, image_ram->h, 32, 0, 0, 0, 0);  // cree une imageen VRAM
	if (image_result==NULL)
		vram = 0;
	if (!vram)
		image_result=SDL_CreateRGBSurface(SDL_SWSURFACE, image_ram->w, image_ram->h, 32, 0, 0, 0, 0);  // cree une image en RAM
	BlitSurface(image_ram,NULL,image_result,NULL);	// copie l'image image_ram de moins de 32 bits vers image_result qui fait 32 bits
	SDL_FreeSurface(image_ram);      // supprime la surface image_ram : inutile maintenant --> libere la mémoire
	return image_result;
}
*/

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
