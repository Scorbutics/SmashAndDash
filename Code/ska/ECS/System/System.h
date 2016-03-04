#pragma once
#include <iostream>
#include "../ECSDefines.h"
#include "../Component/Component.h"

namespace ska {
	template<typename T, typename = typename std::enable_if<std::is_base_of<Component, T>::value>::type>
	class System;

	template<class T>
	class System<T> {
	public :

		System(){ }
		
		virtual void refresh(EntityContainer& entities) = 0;
		~System(){}
	protected:
		ComponentContainer<std::unique_ptr<T>> m_components;
	};
}