#pragma once
#include <vector>
#include <functional>

#include "DrawableContainer.h"

namespace ska {
	class VectorDrawableContainer :
		public DrawableContainer
	{
	public:
		VectorDrawableContainer();
		virtual void reserve(const unsigned int size);
		std::vector<ska::Drawable*>::iterator begin();
		std::vector<ska::Drawable*>::iterator end();
		std::vector<ska::Drawable*>::const_iterator cbegin() const;
		std::vector<ska::Drawable*>::const_iterator cend() const;


		virtual void clear() override;
		virtual void draw() override;
		virtual ~VectorDrawableContainer();
	
	protected:
		virtual void push(Drawable& d) override;

	private:
		/* TODO : smart pointers ? */
		std::vector<Drawable*> m_data;
	};
}
