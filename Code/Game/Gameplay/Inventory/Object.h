#pragma once

#include <string>
#include <vector>
#include "Point.h"
#include "Graphic/Texture.h"
#include "Graphic/Animation.h"

class Object {
    public:
    Object(int id);

    bool use();
    bool use(int i);
    void display();

    void setPos(int x, int y);

	template <class T>
	void setPos(ska::Point<T> pos) {
		m_pos.x = (int) pos.x;
		m_pos.y = (int) pos.y;
	}

	const ska::Point<int>& getPos() const;
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
	ska::Point<int> m_pos;
	unsigned int m_width, m_height;
    std::string m_description, m_name, m_effect, m_action, m_spriteName;
    int m_id;

};

using ObjectPtr = std::unique_ptr<Object>;

