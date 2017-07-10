#include <iostream>
#include "RectangleUtils.h"
#include "NumberUtils.h"

ska::RectangleUtils::RectangleUtils()
{
}

bool ska::RectangleUtils::collisionBoxABoxB(const Rectangle& rectA, const Rectangle& rectB) {

	if (rectA.x > rectB.x + rectB.w || rectA.x + rectA.w < rectB.x) {
		return false;
	}

	if (rectA.y > rectB.y + rectB.h || rectA.y + rectA.h < rectB.y) {
		return false;
	}

	return true;

}

ska::Rectangle ska::RectangleUtils::intersect(const Rectangle& r1, const Rectangle& r2) {
	Rectangle output;
	output.x = NumberUtils::maximum(r1.x, r2.x);
	output.y = NumberUtils::maximum(r1.y, r2.y);
	int xmax = NumberUtils::minimum(r1.x + r1.w, r2.x + r2.w);
	int ymax = NumberUtils::minimum(r1.y + r1.h, r2.y + r2.h);
	if (xmax >= output.x && ymax >= output.y) {
		output.w = xmax - output.x;
		output.h = ymax - output.y;
	} else {
		output.x = 0;
		output.y = 0;
		output.w = 0;
		output.h = 0;
	}
	return output;

}

//divise l'écran en 8 directions possibles en fonction de la position de la souris
int ska::RectangleUtils::getDirectionFromPos(const Point<int>& posHero, const Point<int>& mousePos) {
	double pente;

	if (mousePos.x != posHero.x) {
		pente = NumberUtils::absolute((mousePos.y - posHero.y) / ((double)mousePos.x - posHero.x));
	} else {
		if (mousePos.y > posHero.y){
			return 0;
		} else if (mousePos.y < posHero.y) {
			return 2;
		} else {
			return -1;
		}
	}

	if (pente < 0.0) {
		pente *= -1;
	}

	int direction;
	//taux d'accroissement entre (TAILLEECRAN;TAILLEECRAN/3) et (0; TAILLEECRAN*2/3)
	const double penteDiago1 = 1. / 3;
	//taux d'accroissement entre (TAILLEECRAN*2/3; 0) et (TAILLEECRAN/3;TAILLEECRAN)
	const double penteDiago2 = 3.;

	if (pente > penteDiago2) {
		if (mousePos.y > posHero.y) {
			direction = 0;
		} else {
			direction = 2;
		}
	} else if (pente > penteDiago1) {
		if (mousePos.x > posHero.x && mousePos.y > posHero.y) {
			direction = 4;
		} else if (mousePos.x > posHero.x && mousePos.y < posHero.y) {
			direction = 5;
		} else if (mousePos.x < posHero.x && mousePos.y < posHero.y) {
			direction = 6;
		} else {
			direction = 7;
		}

	} else {
		if (mousePos.x > posHero.x) {
			direction = 1;
		} else {
			direction = 3;
		}
	}

	return direction;
}


ska::Rectangle ska::RectangleUtils::posToCenterPicture(const Rectangle& imageToCenter, const Rectangle& imageBackground)
{
	Rectangle posCenter;
	posCenter.x = imageBackground.x + (imageBackground.w + 1) / 2 - (imageToCenter.w + 1) / 2;
	posCenter.y = imageBackground.y + (imageBackground.h + 1) / 2 - (imageToCenter.h + 1) / 2;
	posCenter.h = imageToCenter.h;
	posCenter.w = imageToCenter.w;

	return posCenter;
}

ska::RectangleUtils::~RectangleUtils()
{
}
