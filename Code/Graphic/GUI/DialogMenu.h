#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../../ska/Graphic/Color.h"
#include "../../ska/Graphic/Texture.h"
#include "../../ska/Graphic/Point.h"
#include "IDialogMenu.h"
#include "HasClickHandler.h"

class World;
class Character;
class Inventory;
class Button;
class Window_Area;
class Inventory_Area;
class Button_Quit;

class DynamicWindowArea;
using DynamicWindowAreaPtr = std::unique_ptr<DynamicWindowArea>;

class DialogMenu : public IDialogMenu, public HasClickHandler
{
public:
	DialogMenu(const std::string& text, const std::string& imageResource, const std::string& menuResource, const ska::Rectangle rect, const unsigned int fontSize, const bool scroll = true, const int timeout = -1);
	DialogMenu(const std::string& text, const std::string& imageResource, const ska::Rectangle rect, const int timeout = -1, const bool scroll = true);
	DialogMenu(const std::string& text, const ska::Rectangle rect, const int timeout = -1, const bool scroll = true);
	DialogMenu(const DialogMenu& dm);
	DialogMenu();
	~DialogMenu();
    void resize(int w, int h);
	void modifyText(const std::string& text);
    void modifyColor(ska::Color col);
    
    void hide(bool x);
	void move(ska::Point<int> pos);
    bool isMoving();

	void setClickHandler(std::function<void(void)> const& action) override;
	void click(const ska::Point<int>& clickPos) override;
	void setPos(ska::Point<int> pos);
    void setPosImg(int x, int y);

    void setAlpha(bool x);
    Window_Area* getCloseButton();
	Window_Area* getButton(const std::string& key);
    Inventory_Area* getInventoryArea(unsigned int index);

	void name(const std::string& name);
	const std::string& getName() const;

	void addDynamicArea(DynamicWindowAreaPtr&& area);
	void addScrollText(const std::string& buttonAspect, int height, int width, const std::vector<std::string>& text, int fontSize, ska::Rectangle relativePos);
	void setButtonClose(std::unique_ptr<Button_Quit>& button);
	void addTextArea(const std::string&text, int fontSize, ska::Point<int> relativePos);
	void addImageArea(const std::string& name, bool alpha, ska::Point<int> relativePos, ska::Rectangle* rectSrc);
	void addImageArea(ska::Texture* tex, bool alpha, ska::Point<int> relativePos, ska::Rectangle* rectSrc);
	void addInventory(Inventory& inv, ska::Rectangle relativePos);
	void addButton(ska::Rectangle relativePos, const std::string& styleName, const std::string& styleNamePressed, int* variable, const std::vector<int>& value, const std::vector<std::string>& displayedText, int fontSize, const std::string& key);
	void addButtonBar(ska::Rectangle relativePos, const std::string& styleName, int* variable, const std::vector<int>& values, const std::vector<std::string>& displayedText, int fontSize, const std::string& key);

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
	std::string m_name;
	std::vector<float> m_scrollTextLengthPerLine;

	std::unique_ptr<Button_Quit> m_closeButton;

	std::function<void(void)> m_clickHandler;
    ska::Color m_color;
	unsigned int m_fontSize, m_size, m_ligne, m_sensScroll;
    
	/* Define a timeout to the displayed window (after that, it auto hides) */
	int m_timeout;
	unsigned int m_t0;

	/* m_scroll : true if the dialog CAN scroll (!= m_isScrolling) */
	bool m_show, m_scroll, m_alpha, m_moving, m_isScrolling;
    std::vector<std::unique_ptr<Window_Area>> m_areaList;


};

