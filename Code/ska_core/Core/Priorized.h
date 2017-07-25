#pragma once

namespace ska {
    //TODO refacto avec DrawableFixedPriority
    class Priorized {
    public:
	    virtual ~Priorized() = default;

	    Priorized() :
            m_priority(0) {
        }

        virtual void setPriority(const int p) {
			m_priority = p;
		}

		virtual int getPriority() const {
			return m_priority;
		}

		template<class T>
        static bool comparatorInf(const T& a, const T& b) {
			int p1 = a->getPriority();
			int p2 = b->getPriority();
			return p1 < p2;
		}

        template<class T>
        static bool comparatorSup(const T& a, const T& b) {
			int p1 = a->getPriority();
			int p2 = b->getPriority();
			return p1 > p2;
		}

		bool operator< (const Priorized &id) {
			int p1 = getPriority();
			int p2 = id.getPriority();
			return p1 < p2;
		}

		bool operator> (const Priorized &id) {
			int p1 = getPriority();
			int p2 = id.getPriority();
			return p1 > p2;
		}

	private:
		int m_priority;
    };
}
