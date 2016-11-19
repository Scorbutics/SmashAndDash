#include <fstream>
#include "Inventory.h"
#include "../../ska/Utils/StringUtils.h"
#include "../../Utils/ChargementImages.h"
#include "../../ska/Utils/RectangleUtils.h"

unsigned int Inventory::m_fontSize = 12;

Inventory::Inventory()
{
    m_color.r = 128;
    m_color.g = 0;
    m_color.b = 0;
}

void Inventory::load(std::string squareSpriteName, std::string squareSpriteNameHighlight)
{
	m_squareSprite.load(squareSpriteName, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);
	m_squareSpriteHighlight.load(squareSpriteNameHighlight, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);
}

int Inventory::search(std::string name)
{
	std::list<std::unique_ptr<Object>>::iterator iter;
    int i = 0;
    for(iter = m_objects.begin(); iter != m_objects.end(); ++iter, i++)
	if(name == (*iter)->getName())
		return i;
    return -1;
}


void Inventory::swap(unsigned int index1, unsigned int index2)
{
	std::list<std::unique_ptr<Object>>::iterator iter1 = m_objects.begin(), iter2 = m_objects.begin();
	std::list<unsigned int>::iterator iterAmount1 = m_amount.begin(), iterAmount2 = m_amount.begin();
	std::list<ska::Texture>::iterator iterAmountS1 = m_amountSurface.begin(), iterAmountS2 = m_amountSurface.begin();
    for(unsigned int i = 0; i < index1 && iter1 != m_objects.end(); iter1++, i++);
    for(unsigned int i = 0; i < index2 && iter2 != m_objects.end(); iter2++, i++);
    for(unsigned int i = 0; i < index1 && iterAmount1 != m_amount.end(); iterAmount1++, i++);
    for(unsigned int i = 0; i < index2 && iterAmount2 != m_amount.end(); iterAmount2++, i++);
    for(unsigned int i = 0; i < index1 && iterAmountS1 != m_amountSurface.end(); iterAmountS1++, i++);
    for(unsigned int i = 0; i < index2 && iterAmountS2 != m_amountSurface.end(); iterAmountS2++, i++);
    if(iter1 != iter2)
    {
        iter_swap(iter1, iter2);
        iter_swap(iterAmount1, iterAmount2);
        iter_swap(iterAmountS1, iterAmountS2);

    }

}

int Inventory::search(int id)
{
	std::list<std::unique_ptr<Object>>::iterator iter;
    int i = 0;
	for (auto& iter : m_objects) {
		if (iter != NULL && id == iter->getID()) {
			return i;
		}
		i++;
	}
    return -1;
}

unsigned int Inventory::getSize()
{
	std::list<std::unique_ptr<Object>>::iterator iter;
    unsigned int i = 0;
    for(iter = m_objects.begin(); iter != m_objects.end(); ++iter, i++);
    return i;
}

void Inventory::clear()
{
    m_objects.clear();
    m_amount.clear();
	std::list<ska::Texture>::iterator iterAmountS;
    for(iterAmountS = m_amountSurface.begin(); iterAmountS != m_amountSurface.end(); ++iterAmountS)
        (*iterAmountS).free();
    m_amountSurface.clear();
}

//Obtenir l'objet numéro "index" dans l'inventaire
Object *Inventory::getObjectFromIndex(int index)
{
	std::list<std::unique_ptr<Object>>::iterator iter;
    int i = 0;
    for(iter = m_objects.begin(); iter != m_objects.end() && i != index; i++ ,++iter);

	if(i == index && !m_objects.empty())
		return (Object*) &(**iter);
	return NULL;
}


std::list<std::unique_ptr<Object>>::iterator Inventory::getIteratorFromIndex(int index)
{
	std::list<std::unique_ptr<Object>>::iterator iter;
    int i = 0;
    for(iter = m_objects.begin(); iter != m_objects.end() && i != index; i++ ,++iter);
    return iter;
}

int Inventory::getIndexFromObject(std::list<std::unique_ptr<Object>>::iterator iterIndex)
{
	std::list<std::unique_ptr<Object>>::iterator iter;
    unsigned int i = 0;
	for (iter = m_objects.begin(); iter != m_objects.end() && iter != iterIndex; i++, ++iter);
    if(i != m_objects.size())
        return i;
    else
        return -1;
}

//Obtenir la quantité numéro "index" dans l'inventaire
std::list<unsigned int>::iterator Inventory::getIteratorAmountFromIndex(int index)
{
	std::list<unsigned int>::iterator iter;
    int i = 0;
    for(iter = m_amount.begin(); iter != m_amount.end() && i != index; i++ ,++iter);
    return iter;
}

unsigned int Inventory::getAmountFromIndex(int index)
{
	std::list<unsigned int>::iterator iter;
    int i = 0;
    for(iter = m_amount.begin(); iter != m_amount.end() && i != index; i++ ,++iter);
    return *iter;
}

ska::Texture* Inventory::getAmountSurfaceFromIndex(int index)
{
	std::list<ska::Texture>::iterator iter;
    int i = 0;
    for(iter = m_amountSurface.begin(); iter != m_amountSurface.end() && i != index; i++ ,++iter);
    return (ska::Texture*) &(*iter);
}

std::list<ska::Texture>::iterator Inventory::getIteratorAmountSurfaceFromIndex(int index)
{
	std::list<ska::Texture>::iterator iter;
    int i = 0;
    for(iter = m_amountSurface.begin(); iter != m_amountSurface.end() && i != index; i++ ,++iter);
    return iter;
}

void Inventory::add(int id, unsigned int amount)
{
    int index = this->search(id);

    if(index == -1)
    {
        int invSize = this->getSize();
		std::list<std::unique_ptr<Object>>::iterator iter;
        for(iter = m_objects.begin(), index = 0; index < invSize && *iter != NULL; ++iter, index++);
        if(index < invSize)
			*iter = std::unique_ptr<Object>(new Object(id));
        else
        {
			m_objects.push_back(std::unique_ptr<Object>(new Object(id)));
            m_amount.push_back(amount);
			ska::Texture buf;
			buf.loadFromText(m_fontSize, ska::StringUtils::intToStr(amount), m_color);
            m_amountSurface.push_back(buf);
        }


    }
    else
    {
        *(this->getIteratorAmountFromIndex(index)) += amount;
		(*(this->getIteratorAmountSurfaceFromIndex(index))).loadFromText(m_fontSize, ska::StringUtils::intToStr(*(this->getIteratorAmountFromIndex(index))), m_color);
    }

}


void Inventory::remove(int id, unsigned int amount)
{
    int index = this->search(id);
    if(index == -1)
        return;

    //Si on retire une quantité d'objet plus élevée que celle que l'on possède
    if((*(this->getIteratorAmountFromIndex(index))) - amount <= 0)
    {
        //On efface l'objet
        m_objects.erase(this->getIteratorFromIndex(index));
        m_amount.erase(this->getIteratorAmountFromIndex(index));
        m_amountSurface.erase(this->getIteratorAmountSurfaceFromIndex(index));
    }
    else
    {
        *(this->getIteratorAmountFromIndex(index)) -= amount;
		(*(this->getIteratorAmountSurfaceFromIndex(index))).loadFromText(m_fontSize, ska::StringUtils::intToStr(*(this->getIteratorAmountFromIndex(index))), m_color);
    }

}

void Inventory::remove(std::string name, unsigned int amount)
{
    int index = this->search(name);
    if(index == -1)
        return;

    if((*(this->getIteratorAmountFromIndex(index))) - amount <= 0)
    {
        m_objects.erase(this->getIteratorFromIndex(index));
        m_amount.erase(this->getIteratorAmountFromIndex(index));
        m_amountSurface.erase(this->getIteratorAmountSurfaceFromIndex(index));
    }
    else
    {
        *(this->getIteratorAmountFromIndex(index)) -= amount;
		(*(this->getIteratorAmountSurfaceFromIndex(index))).loadFromText(m_fontSize, ska::StringUtils::intToStr(*(this->getIteratorAmountFromIndex(index))), m_color);
    }

}

void Inventory::display(ska::Rectangle rect)
{
	/*WGameCore& wScreen = WGameCore::getInstance();
	const ska::InputRange& mousePos = wScreen.getRanges()[ska::InputRangeType::MousePos];
	std::list<std::unique_ptr<Object>>::iterator iter;
	ska::Rectangle buf, bufcenter;
    int i;

    for(iter = m_objects.begin(), i = 0; iter != m_objects.end(); ++iter, i++)
    {
        if(*iter != NULL)
        {

            buf.x = rect.x + (i * m_squareSprite.getWidth())%rect.w;
            buf.y = rect.y + (i * m_squareSprite.getWidth())/rect.w;
            buf.w = m_squareSprite.getWidth();
            buf.h = m_squareSprite.getHeight();
            bufcenter.w = (*iter)->getSprite()->getWidth()/2;
            bufcenter.h = (*iter)->getSprite()->getHeight();
			bufcenter = ska::RectangleUtils::posToCenterPicture(bufcenter, buf);
            bufcenter.x -= bufcenter.w/4;
            bufcenter.y += bufcenter.h/4;
            (*iter)->setPos(bufcenter.x, bufcenter.y);
			if (ska::RectangleUtils::isPositionInBox(mousePos, buf))
                m_squareSpriteHighlight.render(buf.x, buf.y);
            else
                m_squareSprite.render(buf.x, buf.y);

            buf.x += m_squareSprite.getWidth()/2;
            buf.y += m_squareSprite.getHeight()/8;
			(*this->getIteratorAmountSurfaceFromIndex(i)).render(buf.x, buf.y);

            (*iter)->display();
        }
    }
	*/
}

ska::Texture* Inventory::getSquareSprite()
{
    return &m_squareSprite;
}

void Inventory::use(int id, unsigned int amount)
{
    int index = this->search(id);
    if(index == -1)
        return;
    for(unsigned int i = 0; i < amount; i++)
        if(this->getObjectFromIndex(index)->use())
			this->remove(id, 1);
}

void Inventory::use(int id, unsigned int amount, int indexPkmn)
{
    int index = this->search(id);
    if(index == -1)
        return;

    if(this->getObjectFromIndex(index)->getEffect() == "onPkmn")
        for(unsigned int i = 0; i < amount; i++)
			if(this->getObjectFromIndex(index)->use(indexPkmn))
				this->remove(id, 1);
    else
        for(unsigned int i = 0; i < amount; i++)
            if(this->getObjectFromIndex(index)->use())
				this->remove(id, 1);
}

Inventory::~Inventory()
{
    //TTF_CloseFont(m_font);
}
