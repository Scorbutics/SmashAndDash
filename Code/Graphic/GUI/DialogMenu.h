#ifndef DEF_MWINDOW
#define DEF_MWINDOW

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../../ska/Graphic/Texture.h"
#include "../../ska/Graphic/Point.h"
#include "IDialogMenu.h"

class World;
class Character;
class Inventory;
class Button;
class Window_Area;
class Inventory_Area;

class DialogMenu : public IDialogMenu
{
public:
	DialogMenu(const std::string& text, const std::string& imageResource, const std::string& menuResource, const ska::Rectangle rect, const unsigned int fontSize, const bool scroll = true, const int timeout = -1);
	DialogMenu(const std::string& text, const std::string& imageResource, const ska::Rectangle rect, const int timeout = -1, const bool scroll = true);
	DialogMenu(const std::string& text, const ska::Rectangle rect, const int timeout = -1, const bool scroll = true);
	DialogMenu(const DialogMenu& dm);
	DialogMenu();
	~DialogMenu();
    void resize(int w, int h);
    void modifyText(std::string text);
    void modifyColor(SDL_Color col);
    
    void hide(bool x);
	void move(ska::Point<int> pos);
    bool isMoving();

    void setActionClic(std::string action);
	void setPos(ska::Point<int> pos);
    void setPosImg(int x, int y);

    void setAlpha(bool x);
    const std::string getActionClic() const;
    Window_Area* getCloseButton();
    Window_Area* getButton(std::string key);
    Inventory_Area* getInventoryArea(unsigned int index);

	void addScrollText(std::string buttonAspect, int height, int width, std::vector<std::string> text, int fontSize, ska::Rectangle relativePos);
	void addButtonClose(std::string imgName, std::string secondImgName, ska::Rectangle pos);
	void addTextArea(std::string text, int fontSize, ska::Rectangle relativePos);
	void addImageArea(std::string name, bool alpha, ska::Rectangle relativePos, ska::Rectangle* rectSrc);
	void addImageArea(ska::Texture* tex, bool alpha, ska::Rectangle relativePos, ska::Rectangle* rectSrc);
	void addInventory(Inventory& inv, ska::Rectangle relativePos);
	void addButton(ska::Rectangle relativePos, std::string styleName, std::string styleNamePressed, int* variable, std::vector<int> value, std::vector<std::string> displayedText, int fontSize, std::string key);
	void addButtonBar(ska::Rectangle relativePos, std::string styleName, int* variable, std::vector<int> values, std::vector<std::string> displayedText, int fontSize, std::string key);

    void deleteAll();
	bool isVisible(bool noScrolling) const;

	const ska::Rectangle getRect() const override;
	void refresh() override;
	void display() const override;
	bool isVisible() const override;
	int getPriority() const override;

	void operator=(const DialogMenu& dm);

protected:
    ska::Texture m_menuTiles;
	ska::Texture m_image;
	std::vector<ska::Texture> m_textImage;

	ska::Rectangle m_rect;
	ska::Rectangle m_rectImage;
	ska::Rectangle m_scrollingRect;
    std::vector<std::string> m_text;
	std::vector<float> m_scrollTextLengthPerLine;

	std::string m_actionClic;
    SDL_Color m_color;
	unsigned int m_fontSize, m_size, m_ligne, m_sensScroll;
    
	/* Define a timeout to the displayed window (after that, it auto hides) */
	int m_timeout;
	unsigned int m_t0;


	/* m_scroll : true if the dialog CAN scroll (!= m_isScrolling) */
	bool m_show, m_scroll, m_alpha, m_moving, m_isScrolling;
    std::vector<std::unique_ptr<Window_Area>> m_areaList;


};


#endif
