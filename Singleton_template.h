#ifndef DEF_SINGLETON_TEMPLATE
#define DEF_SINGLETON_TEMPLATE

//Pattern Singleton :
template<typename T>
class Singleton
{
	protected:
		Singleton(){}
		~Singleton(){}

	public:
		static T& getInstance() { static T w; return w;}
		//(On a accès à l'instance uniquement avec cette méthode, 
		//cela évite l'appel au constructeur possible plusieurs fois).
};

#endif