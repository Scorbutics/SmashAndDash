#ifndef DEF_OBJECT
#define DEF_OBJECT

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include "../../ska/Graphic/Texture.h"
#include "../../ska/Graphic/Animation.h"

class Object
{
    public:
    Object(int id);

    bool use();
    bool use(int i);
    void display();

    void setPos(int x, int y);
	void setPos(ska::Rectangle pos);

	const ska::Rectangle* getPos();
	ska::Texture* getSprite();
	ska::Animation* getAnimation();
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
		ska::Texture m_sprite;
		ska::Animation m_animation;
	ska::Rectangle m_pos;
    std::string m_description, m_name, m_effect, m_action, m_spriteName;
    int m_id;

};

#endif
