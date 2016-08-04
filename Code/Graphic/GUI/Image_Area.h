#pragma once

#include <iostream>
#include <string>
#include "Window_Area.h"
#include "../../ska/Graphic/Texture.h"


class Image_Area : public Window_Area
{
public:
	Image_Area(DialogMenu* parent, ska::Point<int> relativePos, ska::Rectangle* rectSrcBuf, const std::string& name, bool alpha);
	Image_Area(DialogMenu* parent, ska::Point<int> relativePos, ska::Rectangle* rectSrcBuf, ska::Texture* tex, bool alpha);
	virtual void display();
	std::string getImageName() const;
    virtual std::string getKey(){return "";};
    virtual void forceValue(unsigned int index){index = index;};
    virtual ~Image_Area();
	void setImage(const std::string& name, bool alpha);

    private:
	ska::Texture m_image;
    std::string m_name;
	bool m_alpha;
	ska::Rectangle m_rectSrc;

};

