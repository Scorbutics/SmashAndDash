#ifndef DEF_INVENTORY
#define DEF_INVENTORY

#include <list>
#include <memory>
#include <SDL2/SDL_ttf.h>
#include "Object.h"
#include "Texture.h"

//Inventaire contenu dans la fenetre "pokebag"
class Inventory
{
    public:
    Inventory();


    Texture* getSquareSprite();
    unsigned int getSize();

    Object* getObjectFromIndex(int index);
    unsigned int getAmountFromIndex(int index);
    Texture* getAmountSurfaceFromIndex(int index);

	void add(int id, unsigned int amount);
	void remove(int id, unsigned int amount);
    void remove(std::string name, unsigned int amount);
    void swap(unsigned int index1, unsigned int index2);
    void use(int id, unsigned int amount);
    void use(int id, unsigned int amount, int indexPkmn);
    int search(int id);
    int search(std::string name);
    void clear();
    void display(SDL_Rect relativePos);

	void load(std::string squareSpriteName, std::string squareSpriteNameHighlight);

    ~Inventory();

    private:
    int getIndexFromObject(std::list<std::unique_ptr<Object>>::iterator);
    std::list<unsigned int>::iterator getIteratorAmountFromIndex(int index);
    std::list<Texture>::iterator getIteratorAmountSurfaceFromIndex(int index);
    std::list<std::unique_ptr<Object>>::iterator getIteratorFromIndex(int index);
    std::list<std::unique_ptr<Object>> m_objects;
    std::list<unsigned int> m_amount;
    std::list<Texture> m_amountSurface;
    TTF_Font* m_font;
    SDL_Color m_color;
    SDL_Rect m_rectPos; //Position et dimensions de l'inventaire
    Texture m_squareSprite, m_squareSpriteHighlight; //apparence des cases où seront affichés les objets
};

#endif
