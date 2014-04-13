#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "IniReader.h"
#include "WGameCore.h"
#include "ChargementImages.h"

using namespace std;

/*
//Fonction de blit améliorée (ne blit pas tout ce qui est en dehors de l'écran / dépasse de l'écran [clipping])
int BlitSurface(Texture* src, SDL_Rect *rect_src, SDL_Rect *rect_dest)
{

    if(src == NULL)
        return -2;

	unsigned int wScreenWidth, wScreenHeight;
	wScreenWidth = wScreen.getWidth();
	wScreenHeight = wScreen.getHeight();

    SDL_Rect newSrc, newDest;

    if(rect_src != NULL)
         newSrc = *rect_src;
    else
    {
         newSrc.x = 0;
         newSrc.y = 0;
         newSrc.w = src->getWidth();
         newSrc.h = src->getHeight();
    }

    if(rect_dest != NULL)
         newDest = *rect_dest;
    else
    {
         newDest.x = 0;
         newDest.y = 0;
    }



    if(newDest.x > wScreenWidth) //Si on dépasse l'écran de destination à droite, on ne blit rien
        return -1;
    else if(newDest.x + newSrc.w < 0)//Idem à gauche
        return -1;
    else if(newDest.y > wScreenHeight) //Idem en bas
        return -1;
    else if(newDest.y + newSrc.h < 0)//Idem en haut
        return -1;
    else //Sinon (si on est au moins en partie dans l'écran)
    {

        if(newDest.x + newSrc.w > wScreenWidth) //Si on déborde un peu à droite
            newSrc.w = wScreenWidth - newDest.x;
        else if(newDest.x < 0) //Ou un peu à gauche
        {
            newSrc.x += (-newDest.x); //On décale l'origine en x dans la prise de l'image source
            newSrc.w -= (-newDest.x); //On modifie la taille en conséquence
            newDest.x = 0; //On blit le tout à une position nulle
        }

        if(newDest.y + newSrc.h > wScreenHeight) //Si on déborde un peu en bas
            newSrc.h = wScreenHeight - newDest.y;
        else if(newDest.y < 0) //Ou un peu en haut
        {
            newSrc.y += (-newDest.y);
            newSrc.h -= (-newDest.y);
            newDest.y = 0;
        }

    }
	
	//return src->render(newDest.x, newDest.y, ) SDL_RenderCopy(wScreen.getRenderer(), src, &newSrc, &newDest);
    //return SDL_BlitSurface(src, &newSrc, dest, &newDest);

}
*/
/*
SDL_Texture *SurfaceCopy(SDL_Texture *s)
{
    return SDL_DisplayFormatAlpha(s);
}
*/
string intToStr(int x)
{
    // créer un flux de sortie
    ostringstream oss;
    string buf;

    oss << x; //on ecrit la valeur de l'entier x dedans
    buf = oss.str(); //on le transforme en string

    oss.clear();
    return buf;
}

string uintToStr(unsigned int x)
{
	ostringstream oss;
	string buf;

	oss << x; //on ecrit la valeur de l'entier x dedans
	buf = oss.str(); //on le transforme en string

	oss.clear();
	return buf;
}

unsigned int DistanceSquared(const SDL_Rect* pos1, const SDL_Rect* pos2)
{
	return (((pos1->x - pos2->x) * (pos1->x - pos2->x)) + ((pos1->y - pos2->y) * (pos1->y - pos2->y)));
}

bool IsPositionInBox(const SDL_Rect *pos, const SDL_Rect *box)
{
    return((pos->x >= box->x) && (pos->x <= box->x + box->w) && (pos->y >= box->y) && (pos->y <= box->y + box->h));
}

bool CollisionBoxABoxB(SDL_Rect rectA, SDL_Rect rectB)
{

    if(rectA.x > rectB.x + rectB.w || rectA.x + rectA.w < rectB.x)
        return false;
    if(rectA.y > rectB.y + rectB.h || rectA.y + rectA.h < rectB.y)
        return false;

    return true;

}


int GetDirectionFromPos(SDL_Rect *posHero, SDL_Rect *mousePos) //divise l'écran en 8 directions possibles en fonction de la position de la souris
{
    double pente;
    //SDL_Rect posH = *posHero, posM = *mousePos;

    if(mousePos->x != posHero->x)
        pente = (double)abs((mousePos->y - posHero->y)/((double)mousePos->x - posHero->x));
    else
    {
        if(mousePos->y > posHero->y)
            return 0;
        else if(mousePos->y < posHero->y)
            return 2;
		else
			return -1;
    }

    if(pente < 0.0)
        pente *= -1;

    int direction;
    double penteDiago1 = 1./3; //taux d'accroissement entre (TAILLEECRAN;TAILLEECRAN/3) et (0; TAILLEECRAN*2/3)
    double penteDiago2 = 3.; //taux d'accroissement entre (TAILLEECRAN*2/3; 0) et (TAILLEECRAN/3;TAILLEECRAN)

    if(pente > penteDiago2)
    {
        if(mousePos->y > posHero->y)
            direction = 0;
        else
            direction = 2;
    }
    else if(pente > penteDiago1)
    {
        if(mousePos->x > posHero->x && mousePos->y > posHero->y)
            direction = 4;
        else if(mousePos->x > posHero->x && mousePos->y < posHero->y)
            direction = 5;
        else if(mousePos->x < posHero->x && mousePos->y < posHero->y)
            direction = 6;
        else
            direction = 7;

    }
    else
    {
        if(mousePos->x > posHero->x )
            direction = 1;
        else
            direction = 3;
    }

    return direction;
}

int GetRandom(vector<int>& probs)
{
	int r = rand()%(100);
	size_t newSize;
	vector<int> newProbs;

	const size_t size = probs.size();
	newSize = size;
	for(unsigned int i = 0; i < size && 1 < newSize; i++)
		if(r > probs[i])
		{
			probs[i] = -1;
			newSize--;
		}

	newProbs.reserve(newSize);
	for(unsigned int i = 0; i < size; i++)
		if(probs[i] != -1)
			newProbs.push_back(i);

	
	return newProbs[rand()%newSize];
}

IniReader* GetRandomMobSettings(World* w)
{
	IniReader* reader = NULL;
	vector<int> probs;
	const size_t totMobs = w->getMobSettings().size();

	probs.resize(totMobs);
	for(size_t i = 0; i < totMobs; i++)
		probs[i] = w->getMobSettings()[i].getInt("Spawn probability_percent");


	if(totMobs != 0)
		reader = &w->getMobSettings()[GetRandom(probs)];

	return reader;
}

SDL_Rect GetCurrentSpritePosOfHero(Character* hero, bool animation)
{
    int direction = hero->getDirection();
	SDL_Rect spritePos = hero->getAnimation()->getOffsetBase();


    switch(direction)
    {
        case 0:
        spritePos.y = hero->getAnimation()->getOffsetAndFrameSize().h*2;
        break;
        case 1:
        spritePos.y = hero->getAnimation()->getOffsetAndFrameSize().h;
        break;
        case 2:
        spritePos.y = 0;
        break;
        case 3:
        spritePos.y = hero->getAnimation()->getOffsetAndFrameSize().h*3;
        break;
        case 4:
        spritePos.y = hero->getAnimation()->getOffsetAndFrameSize().h*7;
        break;
        case 5:
        spritePos.y = hero->getAnimation()->getOffsetAndFrameSize().h*5;
        break;
        case 6:
        spritePos.y = hero->getAnimation()->getOffsetAndFrameSize().h*4;
        break;
        case 7:
        spritePos.y = hero->getAnimation()->getOffsetAndFrameSize().h*6;
        break;
        default:
        break;
    }

	hero->getAnimation()->setOffsetAndFrameSize(spritePos);

	if(animation)
		spritePos = hero->getAnimation()->getRectOfCurrentFrame();
	else
		spritePos.x += spritePos.w;

    return spritePos;
}

SDL_Rect PosToCenterPicture(const SDL_Rect *imageToCenter, const SDL_Rect *imageBackground)
{
    SDL_Rect posCenter;
	if (imageToCenter == NULL || imageBackground == NULL)
	{
		posCenter.x = 0;
		posCenter.y = 0;
		posCenter.w = 1;
		posCenter.h = 1;
        cerr << "Erreur (Fonction PosToCenterPicture) : une des deux dimension d'image est nulle" << endl;
	}
	else
    {
        posCenter.x = imageBackground->x + imageBackground->w/2 - imageToCenter->w/2;
        posCenter.y = imageBackground->y + imageBackground->h/2 - imageToCenter->h/2;
        posCenter.h = imageToCenter->h;
        posCenter.w = imageToCenter->w;
    }

    return posCenter;
}

unsigned int GetMax10Pow(int num)
{
	unsigned int absNum = abs(num), i = 0;
	for(i = 0; absNum >= 1; i++, absNum /= 10);

	return i;
}


void VariablesAcquisition(vector<int> &vect, std::string filename)
{
	IniReader reader(filename);
	int var_number = reader.getInt("Game var_number");
	stringstream ss;

	if(var_number > 0)
		for(int i = 1; i <= var_number; i++)
		{
			ss << "%";

			for(int j = 4-GetMax10Pow(i); j >= 0; j--)
				ss << "0";

			ss << i;

			vect.push_back(reader.getInt("Variable " + ss.str()));
			ss.str("");
		}

}

void SwitchesAcquisition(vector<bool> &vect, std::string filename)
{
	IniReader reader(filename);
	int switch_number = reader.getInt("Game switch_number");
	stringstream ss;

	if(switch_number > 0)
		for(int i = 1; i <= switch_number; i++)
		{
			ss << "%";

			for(int j = 4-GetMax10Pow(i); j >= 0; j--)
				ss << "0";

			ss << i;

			vect.push_back(reader.getInt("Switch " + ss.str()) != 0);
			ss.str("");
		}

}

void VariablesWriting(vector<int> &vect, std::string filename)
{
	IniReader reader(filename);
	reader.set("Game var_number", vect.size());
	stringstream ss;

	for(int i = 1; i <= vect.size(); i++)
	{
		ss << "%";

		for(int j = 4-GetMax10Pow(i); j >= 0; j--)
			ss << "0";

		ss << i;

		reader.set("Variable " + ss.str(), vect[i-1]);
		ss.str("");
	}
	reader.save(filename);
}

void SwitchesWriting(vector<bool> &vect, std::string filename)
{
	IniReader reader(filename);
	reader.set("Game switch_number", vect.size());
	stringstream ss;

	for(int i = 1; i <= vect.size(); i++)
	{
		ss << "%";

		for(int j = 4-GetMax10Pow(i); j >= 0; j--)
			ss << "0";

		ss << i;

		reader.set("Switch " + ss.str(), vect[i-1]);
		ss.str("");
	}
	reader.save(filename);
}
