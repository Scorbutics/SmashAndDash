#ifndef DEF_MWINDOW
#define DEF_MWINDOW

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../../ska/Graphic/Texture.h"
#include "../../ska/Graphic/Point.h"

class World;
class Character;
class Inventory;
class Button;
class Window_Area;
class Inventory_Area;

class DialogMenu
{
    public:
		DialogMenu(std::string texte, std::string messImg, std::string fichierMenu, ska::Rectangle posFond, int taillePolice, bool scroll = true);
    ~DialogMenu();

	void display();
    void resize(int w, int h);
    void modifyText(std::string text);
    void modifyColor(SDL_Color col);
    bool isVisible();
    void hide(bool x);
	void move(ska::Point<int> pos);
    bool isMoving();
    void pause();

    void setActionClic(std::string action);
	void setPos(ska::Point<int> pos);
    void setPos(int x, int y);
    void setPosImg(int x, int y);
    void setMoving(bool x);
    void setMessImg(std::string img);

    int getLines();
    int getWLine(unsigned int ligne);
    int getX(unsigned int ligne);
	const ska::Point<int>& getPos();
	const ska::Rectangle& getRect();
	const unsigned int getWidth();
	const unsigned int getHeight();
    bool getAlpha();
    void setAlpha(bool x);
    bool getAlphaImg();
    void setAlphaImg(bool x);
	const ska::Rectangle& getPosImg();
    std::string getActionClic();
    Window_Area* getCloseButton();
    Window_Area* getButton(std::string key);
    Inventory_Area* getInventoryArea(unsigned int index);
    std::string getText(unsigned int line);

	void addScrollText(std::string buttonAspect, int height, int width, std::vector<std::string> text, int fontSize, ska::Rectangle relativePos);
	void addButtonClose(std::string imgName, std::string secondImgName, ska::Rectangle pos);
	void addTextArea(std::string text, int fontSize, ska::Rectangle relativePos);
	void addImageArea(std::string name, bool alpha, ska::Rectangle relativePos, ska::Rectangle* rectSrc);
	void addImageArea(ska::Texture* tex, bool alpha, ska::Rectangle relativePos, ska::Rectangle* rectSrc);
	void addInventory(Inventory& inv, ska::Rectangle relativePos);
	void addButton(ska::Rectangle relativePos, std::string styleName, std::string styleNamePressed, int* variable, std::vector<int> value, std::vector<std::string> displayedText, int fontSize, std::string key);
	void addButtonBar(ska::Rectangle relativePos, std::string styleName, int* variable, std::vector<int> values, std::vector<std::string> displayedText, int fontSize, std::string key);

    void deleteAll();
	void refresh();

    protected:
    ska::Texture m_fond, m_messImage;
    std::vector< ska::Texture > m_stexte;
	ska::Rectangle m_rect, m_rectMessImage;
	ska::Rectangle m_posTexte, m_posLFond, m_posScrollFond, m_positionFond; //m_posScrollFond sert à faire scroll (défiler) la fenêtre lors de l'affichage d'un message
    std::vector<std::string> m_texte;
    std::string m_actionClic;                                       //m_posLFond, quant à lui, est utilisé pour parcourir le "menu.png" (l'image constructive du menu). c'est en quelque sorte un curseur.
    SDL_Color m_couleur;
	unsigned int m_fontSize, m_size, m_ligne, m_sensScroll;
    std::vector<float> m_x;
    bool m_show, m_scroll, m_alpha, m_alphaImg, m_moving;
    std::vector<std::unique_ptr<Window_Area>> m_areaList;


};

//typedef std::unique_ptr<DialogMenu> DialogMenuPtr;

#endif
