#ifndef DEF_INVENTORY
#define DEF_INVENTORY

#include <list>
#include <memory>
#include "Object.h"
#include "Draw/Color.h"


//Inventaire contenu dans la fenetre "pokebag"
class Inventory
{
    public:
		Inventory();

		ska::Texture* getSquareSprite();
		unsigned int getSize();

		Object* getObjectFromIndex(int index);
		unsigned int getAmountFromIndex(int index);
		ska::Texture* getAmountSurfaceFromIndex(int index);

		void add(int id, unsigned int amount);
		void remove(int id, unsigned int amount);
		void remove(std::string name, unsigned int amount);
		void swap(unsigned int index1, unsigned int index2);
		void use(int id, unsigned int amount);
		void use(int id, unsigned int amount, int indexPkmn);
		int search(int id);
		int search(std::string name);
		void clear();
		void display(ska::Rectangle relativePos);

		void load(std::string squareSpriteName, std::string squareSpriteNameHighlight);

		~Inventory();

    private:
		int getIndexFromObject(std::list<std::unique_ptr<Object>>::iterator);
		std::list<unsigned int>::iterator getIteratorAmountFromIndex(int index);
		std::list<ska::Texture>::iterator getIteratorAmountSurfaceFromIndex(int index);
		std::list<std::unique_ptr<Object>>::iterator getIteratorFromIndex(int index);
		std::list<std::unique_ptr<Object>> m_objects;
		std::list<unsigned int> m_amount;
		std::list<ska::Texture> m_amountSurface;
		static unsigned int m_fontSize;
		ska::Color m_color;
		ska::Rectangle m_rectPos; //Position et dimensions de l'inventaire
		ska::Texture m_squareSprite, m_squareSpriteHighlight; //apparence des cases où seront affichés les objets
};

#endif
