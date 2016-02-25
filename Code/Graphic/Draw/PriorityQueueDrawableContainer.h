#pragma once
#include <queue>
#include "DrawableContainer.h"

class PriorityQueueDrawableContainer :
	public DrawableContainer
{
public:
	PriorityQueueDrawableContainer();
	virtual void push(Drawable& d) override;
	virtual void draw() override;
	virtual ~PriorityQueueDrawableContainer();
private:
	/* FIXME : operator< sur Drawable ne marchera pas ici : On utilise des pointeurs. 
		Il faut par exemple utiliser unique_ptr et surcharger l'operator< de comparaison entre
		unique_ptr */
	std::priority_queue<Drawable*> m_data;
};

