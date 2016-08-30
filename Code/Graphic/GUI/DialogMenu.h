#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../../ska/Graphic/Color.h"
#include "../../ska/Graphic/Texture.h"
#include "../../ska/Graphic/Point.h"
#include "../../ska/Graphic/GUI/WindowIG.h"
#include "IDialogMenu.h"


class World;
class Inventory;
class Button;

namespace ska {
	class Widget;
}

class DialogMenu : public IDialogMenu, public ska::WindowIG
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
/*	void move(ska::Point<int> pos);*/
    bool isMoving();

	/*void setClickHandler(std::function<void(void)> const& action) override;
	void click(const ska::Point<int>& clickPos) override;*/
	void move(const ska::Point<int>& pos) override;
    //void setPosImg(int x, int y);

    void setAlpha(bool x);
	ska::Widget* getButton(const std::string& key);
    //Inventory_Area* getInventoryArea(unsigned int index);

	//void name(const std::string& name);
	//const std::string& getName() const;

// 	void addDynamicArea(std::unique_ptr<ska::Widget>&& area);
// 	void addScrollText(const std::string& buttonAspect, int height, int width, const std::vector<std::string>& text, int fontSize, ska::Rectangle relativePos);
// 	void addTextArea(const std::string&text, int fontSize, ska::Point<int> relativePos);
// 	void addImageArea(const std::string& name, bool alpha, ska::Point<int> relativePos, ska::Rectangle* rectSrc);
// 	void addInventory(Inventory& inv, ska::Rectangle relativePos);
// 	void addButton(ska::Rectangle relativePos, const std::string& styleName, const std::string& styleNamePressed, int* variable, const std::vector<int>& value, const std::vector<std::string>& displayedText, int fontSize, const std::string& key);
// 	void addButtonBar(ska::Rectangle relativePos, const std::string& styleName, int* variable, const std::vector<int>& values, const std::vector<std::string>& displayedText, int fontSize, const std::string& key);

    //void clear();
	bool isVisible(bool noScrolling) const;

	//const ska::Rectangle getRect() const override;
	void refresh() override;
	void display() const override;
	bool isVisible() const override;
	int getPriority() const override;

	void operator=(const DialogMenu& dm);

protected:
	//ska::Texture m_image;
	std::vector<ska::Texture> m_stext;

	//ska::Rectangle m_rectImage;
	ska::Rectangle m_scrollingRect;
    std::vector<std::string> m_text;
	//std::string m_name;
	std::vector<float> m_scrollTextLengthPerLine;

	//std::function<void(void)> m_clickHandler;
    ska::Color m_color;
	unsigned int m_fontSize, m_size, m_ligne, m_sensScroll;
    
	/* Define a timeout to the displayed window (after that, it auto hides) */
	int m_timeout;
	unsigned int m_t0;

	/* m_scroll : true if the dialog CAN scroll (!= m_isScrolling) */
	bool m_show, m_scroll, m_alpha, m_moving, m_isScrolling;


};

