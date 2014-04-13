#ifndef DEF_OBJECT
#define DEF_OBJECT

#include <string>
#include <vector>
#include <SDL2/SDL.h>

#include "Animation.h"
#include "Texture.h"

class Object
{
    public:
    Object(int id);

    bool use();
    bool use(int i);
    void display();

    void setPos(int x, int y);
    void setPos(SDL_Rect pos);

    const SDL_Rect* getPos();
    Texture* getSprite();
    Animation* getAnimation();
    std::string getDescription();
    std::string getName();
    std::string getEffect();
    std::string getAction();
    unsigned int getHeight();
    unsigned int getWidth();
    int getID();
    std::string getSpriteName();

    ~Object();

    private:
    Texture m_sprite;
    Animation m_animation;
    SDL_Rect m_pos;
    std::string m_description, m_name, m_effect, m_action, m_spriteName;
    int m_id;

};

#endif
