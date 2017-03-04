#pragma once

namespace ska {
	template<typename T>
	class Singleton {
	protected:
		Singleton() = default;
		~Singleton() = default;

	public:
		static T& getInstance() { static T w; return w; }
		//(On a accès à l'instance uniquement avec cette méthode, 
		//cela évite l'appel au constructeur possible plusieurs fois).
	};
}
