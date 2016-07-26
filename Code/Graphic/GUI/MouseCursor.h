#ifndef DEF_MOUSECURSOR
#define DEF_MOUSECURSOR

#include <string>
#include <memory>
#include "../../Gameplay/Inventory/Object.h"
#include "../../ska/Graphic/Draw/DrawableFixedPriority.h"
#include "../../ska/Inputs/MouseInput.h"

class GUI;
class DialogMenu;

class MouseCursor : public ska::DrawableFixedPriority {
    public:
    MouseCursor();
    ~MouseCursor();

    ska::Animation* getAnimation();
    DialogMenu* getHintBox();
    Object* getObject();
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
	void displaySelectedPokemon();
    void displaySelectedObject();
    void displayHint();
    void hideCursor(bool x);
    void hideHint(bool x);
    bool isActiveCursor();
    bool isActiveHint(GUI *g);
    void showCursorTime(unsigned int delay);

    private:
    ska::Animation m_aniCursor;
    ska::Texture m_sprite;
	ska::Rectangle m_cursorPos;
    std::unique_ptr<DialogMenu> m_hintBox;
    bool m_hideC, m_hideH;
	//CharacterPtr m_stockPkmn;
    std::unique_ptr<Object> m_stockObject;
    unsigned int m_objectAmount, m_time, m_delay;
};

#endif
