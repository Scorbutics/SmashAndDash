#pragma once

#include <string>
#include <memory>
#include "../../ska/Graphic/Draw/DrawableFixedPriority.h"
#include "../../ska/Graphic/Animation.h"
#include "../../ska/Graphic/Texture.h"

class GUI;
class DialogMenu;

class Object;
using ObjectPtr = std::unique_ptr<Object>;

class DialogMenu;
using DialogMenuPtr = std::unique_ptr<DialogMenu>;

namespace ska {
	class InputContextManager;
}

class MouseCursor : public ska::DrawableFixedPriority {
public:
	MouseCursor(const ska::InputContextManager& playerICM);
    ~MouseCursor();

    ska::Animation& getAnimation();
    DialogMenuPtr& getHintBox();
	ObjectPtr& getObject();
    unsigned int getObjectAmount();
    //Character* getPokemon();

    void setObjectAmount(unsigned int x);
	void setObject(Object* object, unsigned int amount);
	//void setPokemon(Character* pkmn);
	void setCursorPos(ska::Rectangle pos);

	void display() const override;
	bool isVisible() const override;

	void update();

	void removePokemon();
	void removeObject();

	void modifyHint(const std::string& hint);
	void displaySelectedPokemon() const;
	void displaySelectedObject() const;
    void displayHint();
    void hideCursor(bool x);
    void hideHint(bool x);
    bool isActiveCursor();
    bool isActiveHint(const GUI& g);
    void showCursorTime(unsigned int delay);

private:
	const ska::InputContextManager& m_playerICM;
    ska::Animation m_aniCursor;
    ska::Texture m_sprite;
	ska::Rectangle m_cursorPos;
    std::unique_ptr<DialogMenu> m_hintBox;
    bool m_hideC, m_hideH;
	//CharacterPtr m_stockPkmn;
    ObjectPtr m_stockObject;
    unsigned int m_objectAmount, m_time, m_delay;
};
