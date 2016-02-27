#include <stdint.h>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>
#include "Character.h"
#include "../Inputs/Readers/IniReader.h"
#include "WGameCore.h"
#include "Data/Statistics.h"
#include "Fight/Skill.h"
#include "../Graphic/SpritePath.h"
#include "../Utils/ChargementImages.h"
#include "../Utils/StringUtils.h"
#include "World/Layer.h"

using namespace std;

Character::Character(int id):
    PhysicSprite(0, 0, 75., 6.5, 6, 6*TAILLEBLOC, 10*TAILLEBLOC, 0, 0, 3, 3, 3, 3),
    m_grassSprite(SPRITEBANK_ANIMATION, 1)
{
	IniReader readerAutoAttack("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Skills"FILE_SEPARATOR"autoattack.ini");
	
	spritesReload();

	m_entityFollow = NULL;
	m_autoAttack = true;
	m_alive = true;
	m_stats = NULL;
    m_id = id;
	m_hp = 150;
	m_debuffSpeed = 0;
    m_rect.x = 0;
    m_rect.y = 0;
    m_rect.w = 72;
    m_rect.h = 72;
    m_compteurDeplacement = 0;
    m_direction = 0;

	m_evolutionNextLevel = 0;
	m_evolutionId = 0;

    m_offset[0] = 5;
    m_offset[1] = 15;
    m_offset[2] = 27;
    m_offset[3] = 15;

	if(m_id > 0)
		loadData();
	else
		m_stats = unique_ptr<Statistics>(new Statistics(10,0,0,0,0,84));
 
	m_skillAutoAttack = Skill_ptr(new Projectile(&readerAutoAttack, this));

	setHPBar("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"hpbar.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"hpbarcontent.png", m_rect);
	m_hp = m_stats->getHpMax();
	m_hpbar.setMaxValue(m_hp);
	m_hpbar.setCurrentValue(m_hp);

	m_characterParts.push_back(unique_ptr<CharacterDrawable>(new CharacterDrawable(this, 1)));
	m_characterParts.push_back(unique_ptr<CharacterDrawable>(new CharacterDrawable(this, 2)));
}

vector<unique_ptr<CharacterDrawable>>& Character::getCharacterParts() {
	return m_characterParts;
}

void Character::spritesReload()
{
	SDL_Rect ofHeroA;
	m_sprite.load(SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, m_id), T_RED, T_GREEN, T_BLUE);
	m_faceset.load(SpritePath::getInstance().getPath(SPRITEBANK_FACESET, m_id), T_RED, T_GREEN, T_BLUE);
	ofHeroA.x = 0;
	ofHeroA.y = 0;
	ofHeroA.w = static_cast<Uint16> (m_sprite.getWidth() / HORIZONTAL_SPRITE_NUMBER_PER_CHAR_SHEET);
	ofHeroA.h = static_cast<Uint16>(m_sprite.getHeight() / VERTICAL_SPRITE_NUMBER_PER_CHAR_SHEET);

	m_rect.w = ofHeroA.w;
	m_rect.h = ofHeroA.h;

	m_anim.setOffsetAndFrameSize(ofHeroA);
}


vector<SDL_Rect> Character::move()
{
    vector<SDL_Rect> ids = PhysicObject::move();
	WGameCore& wScreen = WGameCore::getInstance();

    //Si on est en combat et qu'on va assez vite en percutant l'ennemi, on lui inflige des dégâts !
    if(wScreen.getFight().isFighting())
    {
        size_t idSize = ids.size();
        for(unsigned int i = 0; i < idSize; i++)
        {
			
            if((ids[i].y == ID_CURRENT_OPPONENT && m_entityNumber == ID_CURRENT_POKEMON) || (ids[i].y == ID_CURRENT_POKEMON && m_entityNumber == ID_CURRENT_OPPONENT))
            {
				Character* mob = wScreen.getEntityFactory().getNPC(ids[i].x, ids[i].y);
                bool thisGetDamages = mob->getSpeed() > this->getSpeed();
                float speedDiff = sqrt((m_speedx - mob->m_speedx)*(m_speedx - mob->m_speedx) + (m_speedy - mob->m_speedy)*(m_speedy - mob->m_speedy));
                float coeff = (float)0.20;
                unsigned int totdamage = (unsigned int)pow((float)coeff*speedDiff, 3);

                if(thisGetDamages)
                    this->damage(mob, (unsigned int)log((float)(totdamage + 1)));
                else
					mob->damage(this, (unsigned int)log((float)(totdamage + 1)));

				if (!mob->isAlive() || !m_alive)
					wScreen.getFight().end(EndFightReason::Win);
				
                if(totdamage > 1)
                    wScreen.getShakerManager().shake(10, 1000);
            }
        }
    }

    return ids;


}

void Character::refresh()
{
	WGameCore& wScreen = WGameCore::getInstance();
	
	//Suivre une entité
	if (m_entityFollow != NULL)
	{
		SDL_Rect entityPos = m_entityFollow->getPos();
		this->setSpeedLimit(m_entityFollow->getSpeedLimit());
 		if (DistanceSquared(&entityPos, &m_rect) > 2*2*TAILLEBLOC*TAILLEBLOC)
		{
			//entityPos.x += wScreen.getORel().x;
			//entityPos.y += wScreen.getORel().y;
			applyForce(entityPos, (float)getSpeedLimit()*5);
		}
	
		
	}
	

	//Mouvements programmés
	if (!m_path.isMotionless())
	{
		m_compteurDeplacement--;

		int direction = m_direction;
		if (m_compteurDeplacement <= 0)
		{

			if (m_path.isRandom())
			{
				m_direction = rand() % (20 - 0) + 0; //chiffre aléatoire entre 0 et 19
				m_compteurDeplacement = 30;
			}
			else if (m_path.isPredefined())
			{
				m_direction = m_path.getNextPathDirection();
				m_compteurDeplacement = 15;
			}


			direction = m_direction;
			

			
			applyForce(direction, (float)(m_speedLimit * 5 * 8));
		}
			

		

	}


	PhysicObject::refresh();
}

bool Character::addExperience(int exp)
{
	bool levelUp = this->getStatistics()->addExperience(exp);
	
	if (levelUp)
	{
		if (m_stats->getLevel() == m_evolutionNextLevel)
		{
			unsigned int level = m_evolutionNextLevel;
			m_id = m_evolutionId;
			loadData();
			m_stats->setLevel(level);
			spritesReload();
			refreshStats();
		}
	}

	return levelUp;
}

void Character::playAnimation(unsigned int characterAnim, bool waitFinish)
{
	SDL_Rect 
			ofHeroA;			//Offset Héros actuel

	if (!m_visible)
		return;
	
	//Si l'animation a été jouée une fois (allez + retour donc >= 2)
	if (m_anim.getCycles() >= 2 || !waitFinish)
	{
		//On rejoue l'animation, mais de façon différente selon characterAnim
		m_anim.resetCycles();
		
		//Rectangle base de l'animation
		ofHeroA = m_anim.getOffsetBase();
	
		switch (characterAnim)
		{
			//Positionnement sur la partie "autoattack" de l'image
			//(Décalage de la base de l'animation à droite)
			case CHARACTER_ANIMATION_ATTACK:
				ofHeroA.x = 3 * m_anim.getOffsetAndFrameSize().h;
				break;
		
			default:
			case CHARACTER_ANIMATION_MOVE:
				ofHeroA.x = 0;
				break; 
		}
		
		
		m_anim.setOffsetAndFrameSize(ofHeroA);
	}
}

void Character::display(int part, bool shadow)
{
	WGameCore& wScreen = WGameCore::getInstance();
	SDL_Rect origineRelative = wScreen.getORel(), ofHeroA, buf = m_rect;
		
	if (!m_visible) {
		return;
	}	
	
	//par défaut, dès qu'une animation est finie on retourne à celle de base (celle qui permet de bouger)
	playAnimation(CHARACTER_ANIMATION_MOVE, true);
	
	ofHeroA = GetCurrentSpritePosOfHero(this, (m_speedx*m_speedx + m_speedy*m_speedy + m_speedz*m_speedz) >= 1);

	if (part == 1)
	{
		buf.y += ofHeroA.h / 2;
		ofHeroA.y += ofHeroA.h / 2;
		ofHeroA.h = ofHeroA.h / 2;
		if (shadow)
			displayShadow();
	}
	else if (part == 2)
	{
		ofHeroA.h -= ofHeroA.h / 2;
		displaySpeed();
		if (shadow)
			displayGrassWalk();
	}
    
	buf.x += origineRelative.x;
	buf.y += origineRelative.y;
	m_sprite.render(buf.x, buf.y - m_jumpHeight, &ofHeroA);

}

void Character::loadData()
{
	IniReader data("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Monsters"FILE_SEPARATOR + StringUtils::intToStr(m_id) + ".ini");

	m_descriptor.load(&data, "Description");
	m_stats = unique_ptr<Statistics>(new Statistics(&data, "BaseStats"));
	m_stats->nextLevel();
	m_stats->nextLevel();

	refreshStats(true);

	for (int i = 0; data.get("Skills " + StringUtils::intToStr(i)); i++)
	{
		if (data.getInt("Skills " + StringUtils::intToStr(i) + "_level") <= m_stats->getLevel())
			addSkill(data.getString("Skills " + StringUtils::intToStr(i)));
	}

	m_evolutionNextLevel = data.getInt("Evolution level");
	m_evolutionId = data.getInt("Evolution next_pokemon_id");
	
}

void Character::nextLevel()
{
	m_stats->nextLevel();
	refreshStats();
}

void Character::refreshStats(bool resetHp)
{
	m_speedLimit = m_stats->getSpeed()/7;
	m_attack = m_stats->getAttack();
	m_defense = m_stats->getDefense();
	m_spe_defense = m_stats->getSpeDefense();
	m_spe_attack = m_stats->getSpeAttack();

	if(resetHp)
		m_hp = m_stats->getHpMax();
	
	m_hpbar.setMaxValue(m_hp);
	m_hpbar.setCurrentValue(m_hp);
	
}

void Character::decrease(int stat, int value)
{
	switch(stat)
	{
		case STATS_HP:
			damage(NULL, value);
			break;

		case STATS_ATTACK:
			if( ((int)m_attack) - value >= 0)
				m_attack -= value;
			else
				m_attack = 0;
			break;

		case STATS_DEFENSE:
			if (((int)m_defense) - value >= 0)
				m_defense -= value;
			else
				m_defense = 0;
			break;

		case STATS_SPE_ATTACK:
			if (((int)m_spe_attack) - value >= 0)
				m_spe_attack -= value;
			else
				m_spe_attack = 0;
			break;

		case STATS_SPE_DEFENSE:
			if (((int)m_spe_defense) - value >= 0)
				m_spe_defense -= value;
			else
				m_spe_defense = 0;
			break;

		case STATS_SPEED:
			if(m_speedLimit - value >= 1)
			{
				m_debuffSpeed += value;
				m_speedLimit -= value;
			}
			else
				m_speedLimit = 1;
			break;
	}
}

void Character::displaySpeed()
{
	WGameCore& wScreen = WGameCore::getInstance();

    //5 est une valeur arbitraire
    if(this->getSpeed() >= 5)
    {
        SDL_Rect rect = getHitbox();
        rect.y += rect.h/2;
        rect.h /= 4;
		wScreen.getParticleManager().playEffect(2, 0.5, (float)0.35, 0, rect);
    }

}

void Character::displayGrassWalk()
{
	WGameCore& wScreen = WGameCore::getInstance();
	SDL_Rect hitCenterPos = getHitboxCenterPos();
	if(hitCenterPos.x/TAILLEBLOC >= wScreen.getWorld().getNbrBlocX() || hitCenterPos.y/TAILLEBLOC >= wScreen.getWorld().getNbrBlocY())
		return;

	Block* bBot = wScreen.getWorld().getLayerBot()->getBlock(hitCenterPos.x / TAILLEBLOC, hitCenterPos.y / TAILLEBLOC);
	Block* bMid = wScreen.getWorld().getLayerMid()->getBlock(hitCenterPos.x / TAILLEBLOC, hitCenterPos.y / TAILLEBLOC);

	if(bBot != NULL && bMid != NULL && (bBot->getID() == BLOCK_ID_GRASS || bMid->getID() == BLOCK_ID_GRASS) && this->getSpeed() >= 1)
    {
        SDL_Rect rect = this->getHitbox();
        rect.y += rect.h/4;
        rect.h /= 4;
        wScreen.getParticleManager().playEffect(3, (float)EARTH_GRAVITY, (float)1., 0, rect);

        SDL_Rect pos = m_rect;
        pos.y += m_rect.h - m_grassSprite.getHeight();
        pos.x += m_rect.w/2 - m_grassSprite.getWidth()/2;
        m_grassSprite.setPos(pos);
        m_grassSprite.refresh();
    }
}

vector<Skill_ptr> *Character::getSkills()
{
    return &m_skill;
}


void Character::addSkill(const string& pathAttackName)
{
	IniReader skillData(pathAttackName);
	string type1, type2;

	type1 = skillData.getString("Description style_1");
	type2 = skillData.getString("Description style_2");

    if(type1 == "Projectile" || type2 == "Projectile")
        m_skill.push_back(Skill_ptr(new Projectile(&skillData, this)));
	else if(type1 == "AOE" || type2 == "AOE")
		m_skill.push_back(Skill_ptr(new AOE(&skillData, this)));
}

void Character::setCountD(float x)
{
    m_compteurDeplacement = x;
}

void Character::setHP(int hp)
{
    m_hp = hp;
    if(m_hp > (unsigned int) m_stats->getHpMax())
        m_hp = m_stats->getHpMax();
	else if (m_hp <= 0)
	{
		m_hp = 0;
		m_alive = false;
	}
    m_hpbar.setCurrentValue(m_hp);
}

bool Character::launchSkill(unsigned int skillNumber, SDL_Rect dest)
{
	WGameCore& wScreen = WGameCore::getInstance();
    //Au cas où on essaie d'accéder à un skill inexistant
    //ou si le cooldown n'est pas encore terminé
    if(skillNumber >= m_skill.size() || m_skill[skillNumber]->cooldownOK() == false)
        return false;

    m_skill[skillNumber]->launch(dest);

    if(m_entityNumber == ID_CURRENT_POKEMON)
    {
        wScreen.getFight().getDialog()->modifyText(m_descriptor.getName() + " lance " + m_skill[skillNumber]->getName() + " !");
        wScreen.getFight().showDialog(2000);
    }
    return true;
}

void Character::displaySkills()
{
    for(unsigned int j = 0; j < m_skill.size(); j++)
        m_skill[j]->display();
    
	m_skillAutoAttack->display();
}

void Character::refreshSkills()
{
	WGameCore& wScreen = WGameCore::getInstance();

	if (!isOnGround())
		return;

	
	for(unsigned int j = 0; j < m_skill.size(); j++)
		m_skill[j]->refresh();
	
	if (wScreen.getFight().isFighting() && m_autoAttack )
	{
		Character* pkmn = wScreen.getFight().getPokemon(), *opponent = wScreen.getFight().getOpponent();
		SDL_Rect pkmnPos = pkmn->getHitboxCenterPos(), opponentPos = opponent->getHitboxCenterPos();

		if (m_skillAutoAttack->cooldownOK() && m_entityNumber == ID_CURRENT_POKEMON  && DistanceSquared(&pkmnPos, &opponentPos) <= m_skillAutoAttack->getRange()*m_skillAutoAttack->getRange()*TAILLEBLOC*TAILLEBLOC)
		{
			//pkmn->playAnimation(CHARACTER_ANIMATION_ATTACK, false);
			pkmn->setDirection(GetDirectionFromPos(&pkmnPos, &opponentPos));
			m_skillAutoAttack->launch(opponentPos);
		}
		else if (m_skillAutoAttack->cooldownOK() && m_entityNumber == ID_CURRENT_OPPONENT && DistanceSquared(&pkmnPos, &opponentPos) <= m_skillAutoAttack->getRange()*m_skillAutoAttack->getRange()*TAILLEBLOC*TAILLEBLOC)
		{
			//opponent->playAnimation(CHARACTER_ANIMATION_ATTACK, false);
			opponent->setDirection(GetDirectionFromPos(&opponentPos, &pkmnPos));
			m_skillAutoAttack->launch(pkmnPos);
		}
		
		m_skillAutoAttack->refresh();
	}
}

Texture* Character::getFaceset()
{
    return &m_faceset;
}

Bar* Character::getHPBar()
{
    return &m_hpbar;
}

void Character::setVisible(bool v)
{
    m_visible = v;
    m_hpbar.setVisible(v);
}

float Character::getCountD()
{
    return m_compteurDeplacement;
}



bool Character::isAlive()
{
    if(m_hp > 0)
    {
        m_alive = true;
        m_hpbar.setVisible(true);
        return true;
    }
    else
    {
        m_alive = false;
        m_hpbar.setVisible(false);
        return false;
    }

}



Path* Character::getPath()
{
    return &m_path;
}

void Character::findPath(SDL_Rect dest)
{
    SDL_Rect buf;

    buf.x = m_rect.x + m_offset[3] + m_anim.getRectOfCurrentFrame().w/2 - m_offset[1];
	buf.y = m_rect.y + m_anim.getRectOfCurrentFrame().h / 2 - m_offset[0] + m_offset[2];

    buf.x = (buf.x/TAILLEBLOC) * TAILLEBLOC;
    buf.y = (buf.y/TAILLEBLOC) * TAILLEBLOC;

    m_path.findPath(buf, dest);
}


void Character::setHPBar(string styleName, string contentName, SDL_Rect pos)
{
    m_hpbar.setBarContent(contentName);
    m_hpbar.setBarStyle(styleName);
    m_hpbar.setPos(pos);
}

void Character::setID(int id)
{
	SDL_Rect actualOffsetAnim;

    PhysicSprite::setID(id);
    
	m_sprite.load(SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, id), T_RED, T_GREEN, T_BLUE);
	m_faceset.load(SpritePath::getInstance().getPath(SPRITEBANK_FACESET, id), T_RED, T_GREEN, T_BLUE);

	actualOffsetAnim.x = 0;
	actualOffsetAnim.y = 0;
	actualOffsetAnim.w = static_cast<Uint16> (m_sprite.getWidth()/HORIZONTAL_SPRITE_NUMBER_PER_CHAR_SHEET);
	actualOffsetAnim.h = static_cast<Uint16>(m_sprite.getHeight()/VERTICAL_SPRITE_NUMBER_PER_CHAR_SHEET);

	m_rect.w = actualOffsetAnim.w;
	m_rect.h = actualOffsetAnim.h;

	m_anim.setOffsetAndFrameSize(actualOffsetAnim);
}

void Character::run(bool b)
{
	if(b)
		setSpeedLimit(-m_debuffSpeed/7 + m_stats->getMaxSpeed()/7);
	else
		setSpeedLimit(-m_debuffSpeed/7 + m_stats->getSpeed()/7);
}

bool Character::damage(Character* src, unsigned int damages)
{
	SDL_Rect animPos;
	WGameCore& wScreen = WGameCore::getInstance();

	if (wScreen.getPokemonManager().getFirstUsablePokemon() == NULL)
		wScreen.getFight().end(EndFightReason::Lose);

	if (damages == 0)
		return m_alive;

    if(((int)(m_hp - damages)) <= 0)
    {
        m_hp = 0;
        m_alive = false;

		//personnage source des dommages (NULL si aucun)
		if (src != NULL)
		{
			if (!m_alive)
			{
				if (src->addExperience(this->getStatistics()->getDroppedExperience()))
				{
					wScreen.getFight().getDialog()->modifyText(src->getDescriptor()->getName() + " monte au niveau " + StringUtils::intToStr(src->getStatistics()->getLevel()) + " !");
					wScreen.getFight().showDialog(2000);
				}
				
			}
			else if (src->isAlive())
			{
				if (this->addExperience(src->getStatistics()->getDroppedExperience()))
				{
					wScreen.getFight().getDialog()->modifyText(getDescriptor()->getName() + " monte au niveau " + StringUtils::intToStr(src->getStatistics()->getLevel()) + " !");
					wScreen.getFight().showDialog(2000);
				}
			}
			
		}
		
    }
	else
		m_hp -= damages;
	
	animPos.w = 48;
	animPos.h = 48;
	wScreen.getSpriteAnimationManager().play(SPRITEBANK_ANIMATION, 14, PosToCenterPicture(&animPos, &m_rect), 1, -1, 3, 150);
    m_hpbar.setCurrentValue(m_hp);
	return m_alive;
}


Character::~Character()
{
}

PokemonDescriptor* Character::getDescriptor()
{
	return &m_descriptor;
}

Statistics* Character::getStatistics()
{
	if(m_stats != NULL)
		return &(*m_stats);
	else
		return NULL;
}

unsigned int Character::getHp()
{
	return m_hp;
}

bool Character::autoattack()
{
	return m_autoAttack;
}

void Character::setAutoattack(bool b)
{
	m_autoAttack = b;
}

void Character::followEntity(Character* entity)
{
	m_entityFollow = entity;
}
