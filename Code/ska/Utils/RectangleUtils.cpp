#include <iostream>
#include "RectangleUtils.h"
#include "NumberUtils.h"

ska::RectangleUtils::RectangleUtils()
{
}


unsigned int ska::RectangleUtils::distanceSquared(const ska::Point<int>& pos1, const ska::Point<int>& pos2)
{
	return (((pos1.x - pos2.x) * (pos1.x - pos2.x)) + ((pos1.y - pos2.y) * (pos1.y - pos2.y)));
}



bool ska::RectangleUtils::collisionBoxABoxB(const ska::Rectangle& rectA, const ska::Rectangle& rectB)
{

	if (rectA.x > rectB.x + rectB.w || rectA.x + rectA.w < rectB.x)
		return false;
	if (rectA.y > rectB.y + rectB.h || rectA.y + rectA.h < rectB.y)
		return false;

	return true;

}


int ska::RectangleUtils::getDirectionFromPos(const ska::Point<int>& posHero, const ska::Point<int>& mousePos) //divise l'écran en 8 directions possibles en fonction de la position de la souris
{
	double pente;

	if (mousePos.x != posHero.x)
		pente = ska::NumberUtils::absolute((mousePos.y - posHero.y) / ((double)mousePos.x - posHero.x));
	else
	{
		if (mousePos.y > posHero.y)
			return 0;
		else if (mousePos.y < posHero.y)
			return 2;
		else
			return -1;
	}

	if (pente < 0.0)
		pente *= -1;

	int direction;
	double penteDiago1 = 1. / 3; //taux d'accroissement entre (TAILLEECRAN;TAILLEECRAN/3) et (0; TAILLEECRAN*2/3)
	double penteDiago2 = 3.; //taux d'accroissement entre (TAILLEECRAN*2/3; 0) et (TAILLEECRAN/3;TAILLEECRAN)

	if (pente > penteDiago2)
	{
		if (mousePos.y > posHero.y)
			direction = 0;
		else
			direction = 2;
	}
	else if (pente > penteDiago1)
	{
		if (mousePos.x > posHero.x && mousePos.y > posHero.y)
			direction = 4;
		else if (mousePos.x > posHero.x && mousePos.y < posHero.y)
			direction = 5;
		else if (mousePos.x < posHero.x && mousePos.y < posHero.y)
			direction = 6;
		else
			direction = 7;

	}
	else
	{
		if (mousePos.x > posHero.x)
			direction = 1;
		else
			direction = 3;
	}

	return direction;
}


ska::Rectangle ska::RectangleUtils::posToCenterPicture(const ska::Rectangle& imageToCenter, const ska::Rectangle& imageBackground)
{
	ska::Rectangle posCenter;
	posCenter.x = imageBackground.x + imageBackground.w / 2 - imageToCenter.w / 2;
	posCenter.y = imageBackground.y + imageBackground.h / 2 - imageToCenter.h / 2;
	posCenter.h = imageToCenter.h;
	posCenter.w = imageToCenter.w;

	return posCenter;
}

ska::RectangleUtils::~RectangleUtils()
{
}
