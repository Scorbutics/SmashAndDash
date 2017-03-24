#pragma once
#include <queue>
#include "DrawableContainer.h"

namespace ska {
	class PriorityQueueDrawableContainer :
		public DrawableContainer
	{
	public:
		PriorityQueueDrawableContainer();
	
		virtual void clear() override;
		virtual void draw() override;
		virtual ~PriorityQueueDrawableContainer();
	
	protected:
		virtual void push(const Drawable& d) override;
	private:
		/* FIXME : operator< sur Drawable ne marchera pas ici : On utilise des pointeurs.
			Il faut par exemple utiliser unique_ptr et surcharger l'operator< de comparaison entre
			unique_ptr */
		std::priority_queue<const Drawable*> m_data;
	};
}
