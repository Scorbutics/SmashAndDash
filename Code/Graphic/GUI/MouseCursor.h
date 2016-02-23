#ifndef DEF_MOUSECURSOR
#define DEF_MOUSECURSOR

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <memory>
#include "../../Gameplay/Inventory/Object.h"
#include "GUI.h"

class MouseInput;
class MouseCursor : public Drawable
{
    public:
    MouseCursor();
    ~MouseCursor();

    Animation* getAnimation();
    DialogMenu* getHintBox();
    Object* getObject();
    unsigned int getObjectAmount();
    Character* getPokemon();

    void setObjectAmount(unsigned int x);
	void setObject(Object* object, unsigned int amount);
	void setPokemon(Character* pkmn);
    void setCursorPos(SDL_Rect pos);

	void display() override;

	void removePokemon();
	void removeObject();

    void modifyHint(string hint);
	void displaySelectedPokemon();
    void displaySelectedObject();
    void displayHint();
    void hideCursor(bool x);
    void hideHint(bool x);
    bool isActiveCursor();
    bool isActiveHint(GUI *g);
    void showCursorTime(unsigned int delay);

    private:
    MouseInput *m_in;
    Animation m_aniCursor;
    Texture m_sprite;
    SDL_Rect m_cursorPos;
    std::unique_ptr<DialogMenu> m_hintBox;
    bool m_hideC, m_hideH;
    Character_ptr m_stockPkmn;
    unique_ptr<Object> m_stockObject;
    unsigned int m_objectAmount, m_time, m_delay;
};

#endif
