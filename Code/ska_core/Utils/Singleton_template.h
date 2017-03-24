#pragma once

namespace ska {
	template<typename T>
	class Singleton {
	protected:
		Singleton() = default;
		~Singleton() = default;

	public:
		static T& getInstance() { static T w; return w; }
		//(On a acc�s � l'instance uniquement avec cette m�thode, 
		//cela �vite l'appel au constructeur possible plusieurs fois).
	};
}
