#ifndef DEF_MWINDOW
#define DEF_MWINDOW

#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <memory>
#include "../Texture.h"


class World;
class Character;
class Inventory;
class Button;
class Window_Area;
class Inventory_Area;

class DialogMenu
{
    public:
    DialogMenu(std::string texte, std::string messImg, std::string fichierMenu, SDL_Rect posFond, int taillePolice, bool scroll = true);
    ~DialogMenu();

	void display();
    void resize(int w, int h);
    void modifyText(std::string text);
    void modifyColor(SDL_Color col);
    bool isVisible();
    void hide(bool x);
    void move(SDL_Rect pos);
    bool isMoving();
    void pause();

    void setActionClic(std::string action);
    void setPos(SDL_Rect pos);
    void setPos(int x, int y);
    void setPosImg(int x, int y);
    void setMoving(bool x);
    void setMessImg(std::string img);

    int getLines();
    int getWLine(unsigned int ligne);
    int getX(unsigned int ligne);
    const SDL_Rect* getPos();
    bool getAlpha();
    void setAlpha(bool x);
    bool getAlphaImg();
    void setAlphaImg(bool x);
    const SDL_Rect* getPosImg();
    std::string getActionClic();
    Window_Area* getCloseButton();
    Window_Area* getButton(std::string key);
    Inventory_Area* getInventoryArea(unsigned int index);
    std::string getText(unsigned int line);

    void addScrollText(std::string buttonAspect, int height, int width, std::vector<std::string> text, int fontSize, SDL_Rect relativePos);
    void addButtonClose(std::string imgName, std::string secondImgName, SDL_Rect pos);
    void addTextArea(std::string text, int fontSize, SDL_Rect relativePos);
	void addImageArea(std::string name, bool alpha, SDL_Rect relativePos, SDL_Rect* rectSrc);
	void addImageArea(Texture* tex, bool alpha, SDL_Rect relativePos, SDL_Rect* rectSrc);
    void addInventory(Inventory& inv, SDL_Rect relativePos);
    void addButton(SDL_Rect relativePos, std::string styleName, std::string styleNamePressed, int* variable, std::vector<int> value, std::vector<std::string> displayedText, int fontSize, std::string key);
    void addButtonBar(SDL_Rect relativePos, std::string styleName, int* variable, std::vector<int> values, std::vector<std::string> displayedText, int fontSize, std::string key);

    void deleteAll();
	void refresh();

    protected:
    Texture m_fond, m_messImage;
    std::vector< Texture > m_stexte;
    SDL_Rect m_posFond, m_posTexte, m_posMessImage, m_posLFond, m_posScrollFond, m_positionFond; //m_posScrollFond sert à faire scroll (défiler) la fenêtre lors de l'affichage d'un message
    std::vector<std::string> m_texte;
    std::string m_actionClic;                                       //m_posLFond, quant à lui, est utilisé pour parcourir le "menu.png" (l'image constructive du menu). c'est en quelque sorte un curseur.
    SDL_Color m_couleur;
	unsigned int m_fontSize, m_size, m_ligne, m_sensScroll;
    std::vector<float> m_x;
    bool m_show, m_scroll, m_alpha, m_alphaImg, m_moving;
    std::vector<std::unique_ptr<Window_Area>> m_areaList;


};


#endif
