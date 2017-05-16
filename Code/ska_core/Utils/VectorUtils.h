#pragma once
#include <algorithm>

namespace ska {

    class VectorUtils {
    public:
        ~VectorUtils() = default;

        template<class V, class T>
        static bool removeValue(V& v, T val) {
            auto it = std::find(v.begin(), v.end(), val);
            if(it != v.end()) {
                std::swap(*it, v.back());
                v.pop_back();
				return true;
            }
			return false;
        }

    private:
        VectorUtils() = default;
    };

}
