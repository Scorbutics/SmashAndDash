#include <iostream>

#include "Skill.h"
#include "../../ska/Physic/LinearParticle.h"
#include "../WGameCore.h"
#include "../../Utils/ChargementImages.h"
#include "../../ska/Graphic/SpritePath.h"
#include "../../Gameplay/Data/Statistics.h"
#include "../../ska/Utils/StringUtils.h"
#include "../../ska/Utils/RectangleUtils.h"
#include "../../Utils/IDs.h"

const unsigned int Skill::m_fontSize = 30;

//Déroulement des combats :
//Chargement d'un monde type "Arène" dépendant du lieu où se trouve le héro (même fonctionnement que l'affichage d'un monde mais
//avec pour héro le pokemon qui combat, et pour seules entités : le héro, fixe, le pokémon adverse et notre pokémon).
bool CollisionBoxABoxB(ska::Rectangle rectA, ska::Rectangle rectB);

Projectile::Projectile(ska::IniReader* data):
	Skill(data)
{
	if(m_style1 == "Projectile" || m_style2 == "Projectile")
	{
		m_vitesse = (float)data->getInt("Particle speed"); 
		m_degats = data->getInt("Particle damage");
		m_knockback = data->getInt("Particle knockback");
		m_nombre = data->getInt("Particle number");
		m_slopeNoise = data->getInt("Particle slope_noise");
		m_noise = data->getInt("Particle noise");
	}
	else
	{
		m_vitesse = 0; 
		m_degats = 0;
		m_knockback = 0;
		m_nombre = 0;
		m_slopeNoise = 0;
		m_noise = 0;
	}

	m_t = 0;

	for(int i = 0; i < m_nombre; i++)
	{
		ska::Rectangle nullPos;
		nullPos.x = 0;
		nullPos.y = 0;
		nullPos.w = 0;
		nullPos.h = 0;

		m_particles.push_back(unique_ptr<ska::LinearParticle>(new ska::LinearParticle(m_id, nullPos, 30, 10, false, false)));
		m_particles[i]->toSkillParticle();
	}
}

void Projectile::refresh()
{
	WGameCore& wScreen = WGameCore::getInstance();

    for(int i = 0; i < m_nombre; i++)
        if(m_particles[i] != NULL && m_particles[i]->isActive())
        {
			ska::Point<int> particlePos, particleOrigin;
			unsigned int distance;
            m_particles[i]->refresh();
            m_particles[i]->addSlopeNoise((float)((rand()%(m_slopeNoise + 1) - m_slopeNoise/2)/10.));
            m_particles[i]->setNoise((int)((m_noise*(cos(m_t+i*M_PI/m_nombre))/10.)));
			
			particlePos = m_particles[i]->getPos();
			particleOrigin = m_particles[i]->getOrigin();
/*
			particleOrigin.x += wScreen.getORel().x;
			particleOrigin.y += wScreen.getORel().y;*/
			distance = ska::RectangleUtils::distanceSquared(particlePos, particleOrigin);
			//Si on dépasse la portée prévue, on détruit la particule
			if (distance > m_range*TAILLEBLOC*m_range*TAILLEBLOC)
				m_particles[i]->destroy();
        }

    m_t++;

	collision();
}

void Projectile::display()
{
	for(int i = 0; i < m_nombre; i++)
		if(m_particles[i] != NULL && m_particles[i]->isActive())
			m_particles[i]->display();
}

Projectile::~Projectile()
{

}




AOE::AOE(ska::IniReader* data, Character* parent):
	Skill(data, parent)
{
	m_active = false;
}

void AOE::launch(ska::Point<int> pos)
{
	ska::Texture text(ska::SpritePath::getInstance().getPath(SPRITEBANK_ANIMATION, m_range-1+3), DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, 128);
	//WGameCore& wScreen = WGameCore::getInstance();
	ska::Rectangle ppos = m_parent->getHitboxCenterPos();
	ppos.x -= text.getWidth()/4;
	ppos.y -= text.getHeight()/2;
	//wScreen.getSpriteAnimationManager().play(SPRITEBANK_ANIMATION, m_range-1+3, ppos, 1, 80, 2);
	m_lastTimeCD = m_timeCD;
	m_active = true;
}

void AOE::refresh()
{
	WGameCore& wScreen = WGameCore::getInstance();

	/*if(!wScreen.getFight().isFighting() || !m_active)
		return;

	Character* pkmn = wScreen.getFight().getPokemon();
	Character* opponent = wScreen.getFight().getOpponent();

	if (pkmn != NULL && opponent != NULL)
	{
		ska::Rectangle pkmnPos = pkmn->getPos(), opponentPos = opponent->getPos();
		if (ska::RectangleUtils::distanceSquared(pkmnPos, opponentPos) <= (m_range*m_range*TAILLEBLOC*TAILLEBLOC))
		{
			m_active = false;
			//opposant qui tire
			if(m_parent->getEntityNumber() == INT16_MAX)
			{
				opponent->decrease(STATS_ATTACK, -m_statsBuffAlly->getAttack());			//augmente les stats
				opponent->decrease(STATS_DEFENSE, -m_statsBuffAlly->getDefense());			//
				opponent->decrease(STATS_SPEED, -m_statsBuffAlly->getSpeed());				//
				opponent->decrease(STATS_SPE_DEFENSE, -m_statsBuffAlly->getSpeDefense());	//
				opponent->decrease(STATS_SPE_ATTACK, -m_statsBuffAlly->getSpeAttack());		//
				opponent->damage(NULL, -m_statsBuffAlly->getHpMax());						//heal

				pkmn->decrease(STATS_ATTACK, -m_statsBuffEnemy->getAttack());
				pkmn->decrease(STATS_DEFENSE, -m_statsBuffEnemy->getDefense());
				pkmn->decrease(STATS_SPEED, -m_statsBuffEnemy->getSpeed());
				pkmn->decrease(STATS_SPE_DEFENSE, -m_statsBuffEnemy->getSpeDefense());
				pkmn->decrease(STATS_SPE_ATTACK, -m_statsBuffEnemy->getSpeAttack());

				int damages = opponent->getStatistics()->getAttack() - pkmn->getStatistics()->getDefense() -m_statsBuffEnemy->getHpMax();
				if (m_statsBuffEnemy->getHpMax() != 0)
					pkmn->damage(opponent, damages);				
			


			
			}
			//pokémon qui tire
			else if(m_parent->getEntityNumber() == INT16_MAX-1)
			{
				pkmn->decrease(STATS_ATTACK, -m_statsBuffAlly->getAttack());
				pkmn->decrease(STATS_DEFENSE, -m_statsBuffAlly->getDefense());
				pkmn->decrease(STATS_SPEED, -m_statsBuffAlly->getSpeed());
				pkmn->decrease(STATS_SPE_DEFENSE, -m_statsBuffAlly->getSpeDefense());
				pkmn->decrease(STATS_SPE_ATTACK, -m_statsBuffAlly->getSpeAttack());
				pkmn->damage(NULL, -m_statsBuffAlly->getHpMax());

				opponent->decrease(STATS_ATTACK, -m_statsBuffEnemy->getAttack());
				opponent->decrease(STATS_DEFENSE, -m_statsBuffEnemy->getDefense());
				opponent->decrease(STATS_SPEED, -m_statsBuffEnemy->getSpeed());
				opponent->decrease(STATS_SPE_DEFENSE, -m_statsBuffEnemy->getSpeDefense());
				opponent->decrease(STATS_SPE_ATTACK, -m_statsBuffEnemy->getSpeAttack());
				int damages = pkmn->getStatistics()->getAttack() - opponent->getStatistics()->getDefense() - m_statsBuffEnemy->getHpMax();
				if (m_statsBuffEnemy->getHpMax() != 0)
					opponent->damage(pkmn, damages);
				
			}
		}
	}*/
}

void AOE::display()
{

}

AOE::~AOE()
{}



/*

Melee::Melee(int id, string skill, string icone, Rectangle *posIcone, int degats, int knockback, string description, string nom, string type, int cooldown, int context, Character* parent): 
	Skill(id, skill, 1, TAILLEBLOC*2, icone, posIcone, degats, knockback, description, nom, type, cooldown, context, parent)
{
    m_vitesse = 512;
	Rectangle nullPos;
	nullPos.x = 0;
	nullPos.y = 0;
	nullPos.w = 0;
	nullPos.h = 0;
    m_particles.push_back(unique_ptr<LinearParticle>(new LinearParticle(id, nullPos, 1, 10, false, false)));
    m_particles[0]->toSkillParticle();
}


void Melee::refresh()
{
    if(m_particles[0]->isActive())
        m_particles[0]->refresh(0);
}

Melee::~Melee()
{}


*/











Skill::Skill(ska::IniReader* data, Character* parent)
{
	m_statsBuffAlly = NULL;
	m_statsBuffEnemy = NULL;
	m_parent = parent;
	m_posIcone.x = m_posIcone.y = 0;
	m_comptAnim = 0;
	m_anim.x = 0;
	m_anim.y = 0;
	m_src.x = 0;
	m_src.y = 0;
	m_dst.x = 0;
	m_dst.y = 0;
	m_lastTimeCD = m_timeCD = 0;

	m_style1 = data->getString("Description style_1");
	m_style2 = data->getString("Description style_2");
	m_description = data->getString("Description description");
	m_name = data->getString("Description name");
	m_type = data->getString("Description type");
	m_context = data->getInt("Description context");
	m_id = data->getInt("Description id");
	m_icone.load("."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR + ska::StringUtils::intToStr(m_id) + ".png", DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);

	m_cooldown = data->getInt("Stats cooldown");
	m_range = data->getInt("Stats blocks_range");

	if(m_style1 == "Buff" || m_style2 == "Buff")
	{
		m_statsBuffAlly = unique_ptr<Statistics>(new Statistics(data, "BuffsAlly"));
		m_statsBuffEnemy = unique_ptr<Statistics>(new Statistics(data, "BuffsEnemy"));
		m_statusAlterAlly = data->getInt("StatusAlter ally");
		m_statusAlterEnemy = data->getInt("StatusAlter enemy");;
	}
	else
	{
		m_statsBuffAlly = unique_ptr<Statistics>(new Statistics(0, 0, 0, 0, 0, 0));
		m_statsBuffEnemy = unique_ptr<Statistics>(new Statistics(0, 0, 0, 0, 0, 0));
		m_statusAlterAlly = 0;
		m_statusAlterEnemy = 0;
	}

	
}

int Skill::getKnockback()
{
	return m_knockback;
}

unsigned int Skill::getRange()
{
	return m_range;
}

void Skill::setKnockback(int knockback)
{
	m_knockback = knockback;
}

void Projectile::setSpeed(float vitesse)
{
	m_vitesse = vitesse;
}

float Projectile::getSpeed()
{
	return m_vitesse;
}

void Projectile::launch(ska::Point<int> dest)
{
	ska::Point<int> buf = m_parent->getCenterPos();
	WGameCore& wScreen = WGameCore::getInstance();

    float angle = atan((dest.y - buf.y)/(float)(dest.x - buf.x));
    if((dest.x - buf.x) < 0)
       angle += (float)M_PI;
	
	m_direction = ska::RectangleUtils::getDirectionFromPos(buf, dest);
/*
    buf.x -= wScreen.getORel().x;
    buf.y -= wScreen.getORel().y;*/
    
    const size_t particleSize = m_particles.size();
    for(size_t i = 0; i < particleSize; i++)
        m_particles[i]->launch(buf, angle, (unsigned int)m_vitesse);

    m_lastTimeCD = m_timeCD;
}

bool Skill::cooldownOK()
{
    m_timeCD = SDL_GetTicks();

    if(m_timeCD - m_lastTimeCD < m_cooldown)
    {
        SDL_Color c;
        c.r = 0;
        c.g = 0;
        c.b = 0;
		c.a = 255;
		string s = ska::StringUtils::intToStr((m_cooldown - (m_timeCD - m_lastTimeCD)) / 1000);
        m_cooldownText.loadFromText(m_fontSize, s, c);
    }
    return (m_timeCD - m_lastTimeCD >= m_cooldown);
}

//Nombre de "sous" skills
int Skill::getNumber()
{
    return (int) m_particles.size();
}

int Skill::getContext()
{
    return m_context;
}

ska::Texture* Skill::getSpriteRemainingCD()
{
    return &m_cooldownText;
}



void Projectile::collision()
{
    //Rectangle posHero = wScreen.getHero()->getPos();
	vector<ska::Rectangle> idChar;
	WGameCore& wScreen = WGameCore::getInstance();
    //Rectangle mousePos = wScreen.getInputListener().getInput()->getMousePos();
    //int speed;

   /* for(int i = 0; i < m_nombre; i++)
    {
			idChar = m_particles[i]->collisionNPC();
			size_t idCharSize = idChar.size();

			if(m_parent != NULL && m_parent->getEntityNumber() != INT16_MAX)
			{
				for(int j = 0; j < idCharSize; j++)
				{
					if (idChar[j].y == ID_CURRENT_OPPONENT && m_particles[i]->isActive() && wScreen.getFight().getOpponent() != NULL && !wScreen.getFight().getOpponent()->isDodging())
					{
						Character* opponent = wScreen.getFight().getOpponent();
						m_particles[i]->destroy();
						m_particles[i]->resetSlopeNoise();


						//speed = wScreen.getFight().getOpponent()->getSpeed();
						opponent->applyForce(m_direction, (float)(m_knockback*10.));
						opponent->damage(wScreen.getFight().getPokemon(), m_degats);
						opponent->decrease(STATS_ATTACK, -m_statsBuffEnemy->getAttack());
						opponent->decrease(STATS_DEFENSE, -m_statsBuffEnemy->getDefense());
						opponent->decrease(STATS_SPEED, -m_statsBuffEnemy->getSpeed());
						opponent->decrease(STATS_SPE_DEFENSE, -m_statsBuffEnemy->getSpeDefense());
						opponent->decrease(STATS_SPE_ATTACK, -m_statsBuffEnemy->getSpeAttack());
						opponent->decrease(STATS_HP, -m_statsBuffEnemy->getHpMax());

						opponent->setCountD(10);
						wScreen.getShakerManager().shake(5, 500);

						//Si le pokémon adverse est K.O., le combat est fini
						if (!opponent->isAlive())
						{
							wScreen.getFight().end(EndFightReason::Win);
							return;
						}
					}
					
				}
			}
			else
			{
				for(int j = 0; j < idCharSize; j++)
				{
					if (idChar[j].y == ID_CURRENT_POKEMON && m_particles[i]->isActive() && wScreen.getFight().getPokemon() != NULL && !wScreen.getFight().getPokemon()->isDodging())
					{
						
						Character* pkmn = wScreen.getFight().getPokemon();
						m_particles[i]->destroy();
						m_particles[i]->resetSlopeNoise();
						
						if (wScreen.getFight().isFighting())
						{
							//speed = wScreen.getFight().getPokemon()->getSpeed();
							pkmn->applyForce(m_direction, (float)(m_knockback*10.));
							wScreen.getShakerManager().shake(5, 500);
							pkmn->damage(wScreen.getFight().getOpponent(), m_degats);


							pkmn->decrease(STATS_ATTACK, -m_statsBuffEnemy->getAttack());
							pkmn->decrease(STATS_DEFENSE, -m_statsBuffEnemy->getDefense());
							pkmn->decrease(STATS_SPEED, -m_statsBuffEnemy->getSpeed());
							pkmn->decrease(STATS_SPE_DEFENSE, -m_statsBuffEnemy->getSpeDefense());
							pkmn->decrease(STATS_SPE_ATTACK, -m_statsBuffEnemy->getSpeAttack());
							pkmn->decrease(STATS_HP, -m_statsBuffEnemy->getHpMax());

							pkmn->setCountD(10);

							if (!pkmn->isAlive())
							{
								
									wScreen.getFight().end(EndFightReason::Win);
									return;
								
							}
						}
							
					}
				}

			}
		
	}*/

}

ska::Particle* Skill::getParticle(unsigned int number)
{
    if(number < m_particles.size())
        return &(*m_particles[number]);
    else
        return NULL;
}


ska::Texture* Skill::getIcon()
{
    return &m_icone;
}

void Skill::setIcon(ska::Texture *icone)
{
    m_icone = *icone;
}

ska::Rectangle Skill::getPosIcon()
{
    return m_posIcone;
}

void Skill::setPosIcon(ska::Rectangle posIcone)
{
    m_posIcone = posIcone;
}

string Skill::getName()
{
    return m_name;
}

void Skill::setName(string name)
{
    m_name = name;
}

string Skill::getDescription()
{
    return m_description;
}

void Skill::setDescription(string description)
{
    m_description = description;
}



string Skill::getType()
{
    return m_type;
}

void Skill::setType(string type)
{
    m_type = type;
}





Skill::~Skill()
{
    //TTF_CloseFont(m_font);
}
