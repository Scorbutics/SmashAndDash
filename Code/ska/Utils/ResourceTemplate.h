#ifndef DEF_RESOURCETEMPLATE
#define DEF_RESOURCETEMPLATE

#include <memory>
#include <unordered_map>

namespace ska {
	template <class V, class K>
	class ResourceTemplate
	{

	public:

		virtual void load(K key)
		{
			m_key = key;
			m_keyStr = key.toString();
			if (m_container.find(m_keyStr) == m_container.end() || m_container[m_keyStr].lock() == NULL)
			{
				m_value = std::shared_ptr<V>(new V(m_key));
				m_container[m_keyStr] = m_value;
			}
			else
				m_value = m_container[m_keyStr].lock();
		}

		virtual void free() { m_value = NULL; }

		virtual ~ResourceTemplate() { free(); }


	protected:
		ResourceTemplate() { free(); }
		ResourceTemplate(K key) { free();  load(key); }

		K m_key;
		std::string m_keyStr;
		std::shared_ptr<V> m_value;
		static std::unordered_map<std::string, std::weak_ptr<V>> m_container;

	};

	template<class V, class K>
	std::unordered_map<std::string, std::weak_ptr<V>> ResourceTemplate<V, K>::m_container;
}
#endif

