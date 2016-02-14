#include <math.h>
#include <algorithm>    // std::find
#include "PhysicObject.h"
#include "../Gameplay\WGameCore.h"
#include "../Utils\ChargementImages.h"

using namespace std;

//Constructeur classique
PhysicObject::PhysicObject(int id, unsigned int entityNumber)
{
    m_id = id;
	m_weight = 1.;
	m_rect.x = 0;
	m_rect.y = 0;
	m_rect.w = 0;
	m_rect.h = 0;
	m_jumpPower = 0;
	m_jumpStarted = false;

	m_ax = m_ay = m_az = 0.;
	m_speedx = m_speedy = m_speedz = 0;
    m_speedLimit = 6;
    m_frictionCoeff = 0.25;
    m_canFly = m_ghost = false;
	m_jumpHeight = 0;
    m_movementsEnabled = true;
    m_entityNumber = entityNumber;

	for(int i = 0; i < 4; i++)
		m_offset[i] = 0;
}

//Constructeur détaillé
PhysicObject::PhysicObject(int id, unsigned int entityNumber, float weight, float frictionCoeff, unsigned int speedLimit, SDL_Rect r, int offset[4])
{
    m_id = id;
    m_weight = weight;
    if(m_weight == 0.)
		m_weight = 1.;

    if(speedLimit != 0)
        m_speedLimit = speedLimit;
    else
        m_speedLimit = 1;

    m_frictionCoeff = frictionCoeff;
	m_rect = r;
	m_ax = m_ay = m_az = 0.;
	m_speedx = m_speedy = m_speedz = 0;
	m_canFly = m_ghost = false;
	m_jumpHeight = 0;
    m_movementsEnabled = true;
    m_entityNumber = entityNumber;
	m_jumpPower = 0;
	m_jumpStarted = false;

	for(int i = 0; i < 4; i++)
		m_offset[i] = offset[i];
}

//Variante constructeur détaillé
PhysicObject::PhysicObject(int id, unsigned int entityNumber, float weight, float frictionCoeff, unsigned int speedLimit, SDL_Rect r)
{
    m_id = id;
    m_weight = weight;
	if(m_weight == 0.)
		m_weight = 1.;

    if(speedLimit != 0)
        m_speedLimit = speedLimit;
    else
        m_speedLimit = 1;

    m_frictionCoeff = frictionCoeff;

	m_rect = r;
	m_ax = m_ay = m_az = 0.;
	m_speedx = m_speedy = m_speedz = 0;
	m_canFly = m_ghost = false;
	m_jumpHeight = 0;
    m_movementsEnabled = true;
    m_entityNumber = entityNumber;
	m_jumpPower = 0;
	m_jumpStarted = false;

	for(int i = 0; i < 4; i++)
		m_offset[i] = 0;
}

PhysicObject::PhysicObject(int id, unsigned int entityNumber, float weight, float frictionCoeff, unsigned int speedLimit, SDL_Rect r, int offset0, int offset1, int offset2, int offset3)
{
    m_id = id;

	m_weight = weight;
	if(m_weight == 0.)
		m_weight = 1.;
	m_rect = r;
	m_ax = m_ay = m_az = 0.;
	m_speedx = m_speedy = m_speedz = 0;

    if(speedLimit != 0)
        m_speedLimit = speedLimit;
    else
        m_speedLimit = 1;

    m_frictionCoeff = frictionCoeff;
	m_canFly = m_ghost = false;
	m_jumpHeight = 0;
    m_movementsEnabled = true;
    m_entityNumber = entityNumber;
	m_jumpPower = 0;
	m_jumpStarted = false;

    m_offset[0] = offset0;
    m_offset[1] = offset1;
    m_offset[2] = offset2;
    m_offset[3] = offset3;
}

PhysicObject::PhysicObject(int id, unsigned int entityNumber, float weight, float frictionCoeff, unsigned int speedLimit, int x, int y, unsigned int w, unsigned int h, int offset0, int offset1, int offset2, int offset3)
{
	m_id = id;

	m_weight = weight;
	if(m_weight == 0.)
		m_weight = 1.;
	
	m_rect.x = x;
	m_rect.y = y;
	m_rect.w = w;
	m_rect.h = h;

	m_ax = m_ay = m_az = 0.;
	m_speedx = m_speedy = m_speedz = 0;

	if(speedLimit != 0)
		m_speedLimit = speedLimit;
	else
		m_speedLimit = 1;

	m_frictionCoeff = frictionCoeff;
	m_canFly = m_ghost = false;
	m_jumpHeight = 0;
	m_movementsEnabled = true;
	m_entityNumber = entityNumber;
	m_jumpPower = 0;
	m_jumpStarted = false;

	m_offset[0] = offset0;
	m_offset[1] = offset1;
	m_offset[2] = offset2;
	m_offset[3] = offset3;
}

int PhysicObject::getJumpHeight()
{
	return m_jumpHeight;
}

void PhysicObject::setJumpHeight(int jumpHeight)
{
	m_jumpHeight = jumpHeight;
}

bool PhysicObject::isDodging()
{
	return (m_jumpHeight >= TAILLEBLOC*2/3);
}

void PhysicObject::setDirection(int x)
{
	if(x >= 0 && x < 8)
		m_direction = x;
	else
		m_direction = -1;
}

int PhysicObject::getDirection() const
{
    return m_direction;
}

void PhysicObject::applyForce(Force f)
{
	int power = (int)f.getPower();
	float angle = f.getAngle();

	//application de la force sur x et y:
	m_ax += power*cos(angle)/m_weight;
	m_ay += power*sin(angle)/m_weight;
}

int PhysicObject::getID()
{
    return m_id;
}

SDL_Rect PhysicObject::getHitboxCenterPos() const
{
    SDL_Rect buf;

    buf.x = m_rect.x + (m_rect.w - m_offset[1] + m_offset[3])/2;
    buf.y = m_rect.y + (m_rect.h - m_offset[0] + m_offset[2])/2;

    return buf;
}

bool PhysicObject::isOnGround()
{
	return (m_jumpHeight == 0);
}

void PhysicObject::applyForce(int direction, float power)
{
	float angle;
	Force f;

	switch(direction)
	{
		case 0:
		angle = (float)M_PI/2;
		break;

		case 1:
			angle = (float)0.;
		break;

		case 2:
			angle = -(float)M_PI / 2;
		break;

		case 3:
			angle = -(float)M_PI;
		break;

		case 4:
			angle = (float)M_PI / 4;
		break;

		case 5:
			angle = -(float)M_PI / 4;
		break;

		case 6:
			angle = (float)(-3 * M_PI / 4);
		break;

		case 7:
			angle = (float)(3 * M_PI / 4);
		break;

		default:
		return;
		break;
	}

	f.setAngle(angle);
	f.setPower(power);
	this->applyForce(f);
}

void PhysicObject::teleport(int x, int y)
{
    m_rect.x = x;
    m_rect.y = y;
}

void PhysicObject::applyForce(SDL_Rect dest, float power)
{
	Force f;
	float slope, angle;
    SDL_Rect hitboxPos = this->getHitboxCenterPos();

	if(hitboxPos.x == dest.x)
		angle = (float)M_PI / 2;
	else
	{
		slope = ((float)(dest.y - hitboxPos.y)) / (dest.x - hitboxPos.x);
		angle = atan(slope);
        if((dest.x - hitboxPos.x) < 0)
			angle += (float)M_PI;
	}

	f.setAngle(angle);
	f.setPower(power);
	this->applyForce(f);

}

//bouge dans la direction et avec la vitesse déterminées par l'application des forces
void PhysicObject::refresh()
{
	//on termine le PFD selon x et y avec la force de frottements fluides
	//( en comptant qu'au préalable on a peut-être déjà appliqué une ou plusieurs force(s) avec "applyForce" )
	m_ax += - m_frictionCoeff * m_speedx / m_weight;
	m_ay += - m_frictionCoeff * m_speedy / m_weight;
	
	//EARTH_GRAVITY était trop élevée alors j'ai préféré la diviser par 5
	m_az -= (float)EARTH_GRAVITY / 5;
	
	//(v(t) - v(t-1))/(t - (t-1)) = dv/dt (t) = a(t)
	m_speedx += m_ax;
	m_speedy += m_ay;
	m_speedz += m_az;

    m_ax = 0;
    m_ay = 0;
	m_az = 0;

	if(m_speedx*m_speedx + m_speedy*m_speedy + m_speedz*m_speedz != 0)
		this->move();
}

float PhysicObject::getSpeed()
{
    return (sqrtf(m_speedx*m_speedx + m_speedy*m_speedy));
}

SDL_Rect PhysicObject::getPos()
{
    return m_rect;
}

SDL_Rect PhysicObject::getCenterPos()
{
    SDL_Rect r = m_rect;
    r.x += r.w/2;
    r.y += r.h/2;

    return r;
}

void PhysicObject::setGhost(bool x)
{
    m_ghost = x;
}

void PhysicObject::move(int direction, float speed)
{
	float angle;

    switch(direction)
	{
		case 0:
			angle = (float)M_PI / 2;
		break;

		case 1:
			angle = (float)0.;
		break;

		case 2:
			angle = -(float)M_PI / 2;
		break;

		case 3:
			angle = -(float)M_PI;
		break;

		case 4:
			angle = (float)M_PI / 4;
		break;

		case 5:
			angle = -(float)M_PI / 4;
		break;

		case 6:
			angle = (float)(-3 * M_PI / 4);
		break;

		case 7:
			angle = (float)(3 * M_PI / 4);
		break;

		default:
		return;
		break;
	}

    m_speedx = speed*cos(angle);
    m_speedy = speed*sin(angle);

	if(m_speedx*m_speedx + m_speedy*m_speedy + m_speedz*m_speedz != 0)
		this->move();

}

void PhysicObject::reset()
{
    m_ay = m_ax = m_az = 0.;
    m_speedx = m_speedy = m_speedz = 0.;
}

//bouge dans la direction déterminée par "dest" et à la vitesse "speed"
void PhysicObject::move(SDL_Rect dest, float speed)
{
	float slope, angle;
    SDL_Rect hitboxPos = this->getHitboxCenterPos();

	if(hitboxPos.x == dest.x)
	{

        if(dest.y - hitboxPos.y > 0)
            m_speedy = +speed;
        else if (dest.y - hitboxPos.y < 0)
            m_speedy = -speed;
        else
        {
            m_speedy = 0;
        }

		m_speedx = 0;
	}
	else
	{
		slope = ((float)(dest.y - hitboxPos.y)) / (dest.x - hitboxPos.x);
		angle = atan(slope);
		if((dest.x - hitboxPos.x) < 0)
            angle += (float)M_PI;

		m_speedx = speed*cos(angle);
		m_speedy = speed*sin(angle);
	}

	if(m_speedx*m_speedx + m_speedy*m_speedy + m_speedz*m_speedz != 0)
		this->move();

}

//bouge dans une direction vers "dest", à la vitesse courante de l'objet
void PhysicObject::move(SDL_Rect dest)
{
	if(m_speedx*m_speedx + m_speedy*m_speedy + m_speedz*m_speedz != 0)
		this->move(dest, sqrt(m_speedx*m_speedx + m_speedy*m_speedy));
}

void PhysicObject::jump(unsigned int power)
{
	m_az += power;
	m_lastGroundPos = getHitboxCenterPos();
	m_jumpPower = power;
	m_jumpStarted = true;
}

//bouge dans la direction courante à la vitesse courante
vector<SDL_Rect> PhysicObject::move()
{
	WGameCore& wScreen = WGameCore::getInstance();
    vector<SDL_Rect> ids, ids2, idsEnd;
    SDL_Rect lastTheoricPos = m_rect;
    SDL_Rect rectOrigin = m_rect;
	float currentSpeed = getSpeed();



    if(!m_movementsEnabled)
        return ids;

    //si on dépasse la vitesse limite fixée du personnage
    if(currentSpeed > m_speedLimit)
    {
        //on réajuste les vitesses
        m_speedx *= ((float)m_speedLimit)/currentSpeed;
        m_speedy *= ((float)m_speedLimit)/currentSpeed;
    }


    //obtention de la direction du personnage à l'aide de m_speedx et m_speedy
    lastTheoricPos.x += (int)(m_speedx + 0.5);
	lastTheoricPos.y += (int)(m_speedy + 0.5);
    if(lastTheoricPos.x != rectOrigin.x || lastTheoricPos.y != rectOrigin.y)
        m_direction = GetDirectionFromPos(&rectOrigin, &lastTheoricPos);
	



	


	//(x(t) - x(t-1))/(t - (t-1)) = dx/dt (t) = vx(t)
	//(y(t) - y(t-1))/(t - (t-1)) = dy/dt (t) = vy(t)
	m_rect.x += (int)(m_speedx + 0.5);
    ids = this->collisionNPC();

	//si jamais on entre en contact avec un objet non traversable du monde...
	bool colWorld = this->collisionWorld();
	if(!m_ghost && (colWorld || (!ids.empty() && !isDodging()) ))
        m_rect = rectOrigin;       //on annule le déplacement
    if(colWorld)
        m_speedx = 0;


	//puis on recommence selon les y maintenant
	SDL_Rect buf = m_rect;
	m_rect.y += (int)(m_speedy + 0.5);
    ids2 = this->collisionNPC();

    colWorld = this->collisionWorld();
	if (!m_ghost && (colWorld || (!ids2.empty() && !isDodging())))
        m_rect = buf;
    if(colWorld)
        m_speedy = 0;


	//gestion du saut
	//(z(t) - z(t-1))/(t - (t-1)) = dz/dt (t) = vz(t)
	m_jumpHeight += (int)(m_speedz + 0.5);
	//Au moment où l'on touche le sol
	if (m_jumpHeight < 0)
	{

		m_jumpHeight = 0;
		m_speedz = 0.;
		m_az = 0.;

		//Si après le saut on tombe sur un endroit non praticable
		if (m_jumpStarted && this->collision())
		{
			SDL_Rect hitboxCenterPos, targetPos;					//position théorique d'atterissage
			hitboxCenterPos = getHitboxCenterPos();
			targetPos = getHitboxCenterPos();
			targetPos.x += (hitboxCenterPos.x - m_lastGroundPos.x);
			targetPos.y += (hitboxCenterPos.y - m_lastGroundPos.y);

			m_az = 0;
			m_speedz = 0;
			m_jumpHeight = 3 * TAILLEBLOC / 4;

			//Si la case "cible" est praticable
			if (!wScreen.getWorld().getCollision(targetPos.x/TAILLEBLOC, targetPos.y/TAILLEBLOC))
			{
				//On saute par là-bas
				move(targetPos, (float)getSpeedLimit() * 100);
			}
			else
			{
				//sinon on fait une annulation du saut
				move(m_lastGroundPos, (float)getSpeedLimit() * 100);
			}

		}
		else
			m_jumpStarted = false;
	}


    //Copie la concaténation des deux dans un seul vecteur en supprimant les doublons possibles
    const size_t size1 = ids.size();
    idsEnd.resize(size1);
    for(size_t i = 0 ; i < size1; i++)
    {
        idsEnd[i].x = ids[i].x;
        idsEnd[i].y = ids[i].y;
    }
    for(size_t i = 0 ; i < ids2.size(); i++)
    {
        if(!FindID(idsEnd, ids2[i]))
            idsEnd.push_back(ids2[i]);
    }

	//Possibilité de "pousser" les autres objets
	if (!isDodging())
	{
		const size_t idsEndSize = idsEnd.size();
		for(size_t i = 0; i < idsEndSize; i++)
		{
			if(wScreen.getEntityFactory().getNPC(idsEnd[i].x, idsEnd[i].y) != NULL)
			{
				Character* mob = &(*wScreen.getEntityFactory().getNPC(idsEnd[i].x, idsEnd[i].y));
				int speedDiffx = (int)(m_speedx - mob->m_speedx);
				int speedDiffy = (int)(m_speedy - mob->m_speedy);
				float coeff = 0.5*30;
				SDL_Rect nullPos;
				nullPos.x = 0;
				nullPos.y = 0;
				nullPos.w = 0;
				nullPos.h = 0;
				Force fx(nullPos, coeff*speedDiffx, 0.);
				Force fy(nullPos, coeff*speedDiffy, (float)M_PI/2);
				Force f2x(nullPos, coeff*speedDiffx, -(float)M_PI);
				Force f2y(nullPos, coeff*speedDiffy, -(float)M_PI / 2);
				mob->applyForce(fx);
				mob->applyForce(fy);
				this->applyForce(f2x);
				this->applyForce(f2y);
			}
		}
	}
    return idsEnd;
}

bool PhysicObject::collision(SDL_Rect targetPos)
{
	SDL_Rect lastPos = m_rect;
	bool result;

	m_rect = targetPos;
	m_rect.x -= (m_rect.w - m_offset[1] + m_offset[3]) / 2;
	m_rect.y -= (m_rect.h - m_offset[0] + m_offset[2]) / 2;
	
	result = collision();

	m_rect = lastPos;

	return result;
}

bool PhysicObject::collision()
{
	vector<SDL_Rect> ids, ids2;
	SDL_Rect lastTheoricPos = m_rect;
	SDL_Rect rectOrigin = m_rect;
	float currentSpeed = getSpeed();

	//(x(t) - x(t-1))/(t - (t-1)) = dx/dt (t) = vx(t)
	//(y(t) - y(t-1))/(t - (t-1)) = dy/dt (t) = vy(t)
	m_rect.x += (int)(m_speedx + 0.5);
	ids = this->collisionNPC();

	//si jamais on entre en contact avec un objet non traversable du monde...
	bool colWorld = this->collisionWorld();
	if (!m_ghost && (colWorld || (!ids.empty() && !isDodging())))
		return true;       

	//puis on recommence selon les y maintenant
	SDL_Rect buf = m_rect;
	m_rect.y += (int)(m_speedy + 0.5);
	ids2 = this->collisionNPC();

	colWorld = this->collisionWorld();
	if (!m_ghost && (colWorld || (!ids2.empty() && !isDodging())))
		return true;

	return false;
}

//teste les collisions avec les NPC uniquement
vector<SDL_Rect> PhysicObject::collisionNPC()
{
    vector<SDL_Rect> ids, buf;
	WGameCore& wScreen = WGameCore::getInstance();

    buf = wScreen.detectEntity(this->getHitbox());
    const size_t numberEntities = buf.size();
    for(size_t i = 0; i < numberEntities; i++)
        if((buf[i].x != m_id || (unsigned int)buf[i].y != m_entityNumber) && !FindID(ids, buf[i]))     //si buf[i] n'est pas déjà dans ids
            ids.push_back(buf[i]);                                                      //alors on rajoute l'élément à ids

    return ids;
}

void PhysicObject::setEntityNumber(unsigned int n)
{
    m_entityNumber = n;
}

//pour un pnj, renvoie l'index dans le tableau des entités
unsigned int PhysicObject::getEntityNumber()
{
    return m_entityNumber;
}

SDL_Rect PhysicObject::getHitbox()
{
    SDL_Rect hitbox;
    hitbox.x = m_rect.x + m_offset[3];
    hitbox.y = m_rect.y + m_offset[2];
    hitbox.w = m_rect.w - m_offset[3] - m_offset[1] -1;
    hitbox.h = m_rect.h - m_offset[2] - m_offset[0] -1;
    return hitbox;
}


//teste les collisions avec les blocs uniquement
bool PhysicObject::collisionWorld()
{
	WGameCore& wScreen = WGameCore::getInstance();
	World& w = wScreen.getWorld();
	SDL_Rect chd, chg, cbg;


	//position coin haut droit hitbox
	chd.x = m_rect.x + m_rect.w-1 - m_offset[1];
	chd.y = m_rect.y + m_offset[2];

	//position coin haut gauche hitbox
	chg.x = m_rect.x + m_offset[3];
	chg.y = m_rect.y + m_offset[2];

	//position coin bas gauche hitbox
	cbg.x = m_rect.x + m_offset[3];
	cbg.y = m_rect.y + m_rect.h-1 - m_offset[0];

    for(int j = chg.y/TAILLEBLOC; j <= cbg.y/TAILLEBLOC; j++)
        for(int i = chg.x/TAILLEBLOC; i <= chd.x/TAILLEBLOC; i++)
            if(w.getCollision(i, j) && !(isDodging() && w.isBlockDodgeable(i, j)))
                return true;
    return false;
}

void PhysicObject::setMoving(bool b)
{
    m_movementsEnabled = b;
}

bool PhysicObject::movable()
{
    return m_movementsEnabled;
}

void PhysicObject::setOffset(int direction, int nbr)
{
    if(direction >= 0 && direction < 4)
        m_offset[direction] = nbr;
}

void PhysicObject::setSpeedLimit(int speedLimit)
{
	if(speedLimit > 0)
		m_speedLimit = speedLimit;
}

unsigned int PhysicObject::getSpeedLimit()
{
    return m_speedLimit;
}

int PhysicObject::getOffset(int direction)
{
    if(direction >= 0 && direction < 4)
        return m_offset[direction];
    else
        return 0;
}

float PhysicObject::getAngle()
{
    if(m_speedx != 0)
    {
        if(m_speedx > 0)
            return atan(m_speedy/m_speedx);
        else
			return (float)M_PI + atan(m_speedy / m_speedx);
    }
    else if(m_speedy >= 0)
		return (float)M_PI / 2;
    else
		return -(float)M_PI / 2;
}

bool PhysicObject::getGhost()
{
	return m_ghost;
}

PhysicObject::~PhysicObject(){}




bool FindID(vector<SDL_Rect> &ids, SDL_Rect id)
{
    const size_t idSize = ids.size();
    for(size_t i = 0; i < idSize; i++)
        if(id.x == ids[i].x && id.y == ids[i].y)
            return true;
    return false;
}


