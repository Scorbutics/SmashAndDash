#include <iostream>
#include <fstream>
#include <string>

#include "../../Utils\IDs.h"
#include "Image_Area.h"
#include "DialogMenu.h"

Image_Area::Image_Area(DialogMenu* parent, ska::Rectangle relativePos, ska::Rectangle* rectSrcBuf, const std::string& name, bool alpha) : 
Window_Area(parent),
m_image(name, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, alpha ? 128 : -1)
{
    m_type = BUTTON_IMAGE_AREA;
    m_active = parent->isVisible();
    m_relativePos.x = relativePos.x;
    m_relativePos.y = relativePos.y;
    
	if (rectSrcBuf != NULL)
		m_rectSrc = *rectSrcBuf;
	else
	{
		m_rectSrc.x = 0;
		m_rectSrc.y = 0;
	}

	m_alpha = alpha;
	m_name = name;
	
}

Image_Area::Image_Area(DialogMenu* parent, ska::Rectangle relativePos, ska::Rectangle *rectSrcBuf, ska::Texture* tex, bool alpha) : Window_Area(parent), m_image(*tex)
{
	m_type = BUTTON_IMAGE_AREA;
	m_active = parent->isVisible();
	m_relativePos.x = relativePos.x;
	m_relativePos.y = relativePos.y;
	if (rectSrcBuf != NULL)
		m_rectSrc = *rectSrcBuf;
	else
	{
		m_rectSrc.x = 0;
		m_rectSrc.y = 0;
	}
	
	m_alpha = alpha;
	m_name = "";
}

void Image_Area::setImage(const std::string& name, bool alpha)
{
    m_name = name;
	m_alpha = alpha;
	m_image.load(m_name, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, alpha ? 128 : -1);
}

std::string Image_Area::getImageName() const
{
    return m_name;
}

void Image_Area::display()
{
    if(!m_parent->isVisible())
        return;
    else
        m_active = true;

	ska::Rectangle buf = m_relativePos;
	buf.x += (m_parent->getRect()).x;
	buf.y += (m_parent->getRect()).y;

	m_image.render(buf.x, buf.y, &m_rectSrc);
}

Image_Area::~Image_Area()
{
}
