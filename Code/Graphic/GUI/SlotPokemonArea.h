#include "DynamicWindowArea.h"
#include "../../Gameplay\Character.h"
#include "..\Texture.h"
#include "Image_Area.h"
#include "Text_Area.h"
#ifndef DEF_SLOT_POKEMON_AREA
#define DEF_SLOT_POKEMON_AREA

class WGameCore;

class SlotPokemon_Area : DynamicWindowArea
{
    public:
    SlotPokemon_Area(DialogMenu* parent, SDL_Rect relativePos, std::string styleName, std::string styleNamePressed, std::string action, int* boolUseObjectSelectPkmn, int key);
    void setPokemon(unsigned int index);
    virtual void display();
    SDL_Rect getRectSize();
    Character* getPokemon();
    virtual std::string getKey(){return "";};
    virtual void forceValue(unsigned int){};
    virtual ~SlotPokemon_Area();
	void refresh();

    private:
    int m_key, m_index;
    int* m_boolUseObjectSelectPkmn;
    Character* m_pkmn;
	Texture m_image, m_imagePressed;
	std::string m_action, m_style, m_stylePressed;;
    std::unique_ptr<Image_Area> m_spriteArea;
    std::unique_ptr<Text_Area> m_pvArea, m_type1Area, m_type2Area, m_nameArea, m_levelArea;
    std::unique_ptr<Button> m_buttonArea;

};
#endif
