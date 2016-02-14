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
		//(On a acc�s � l'instance uniquement avec cette m�thode, 
		//cela �vite l'appel au constructeur possible plusieurs fois).
};

#endif