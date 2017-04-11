#pragma once

namespace ska {

    class VectorUtils {
    public:
        ~VectorUtils() = default;

        template<class T>
        static void removeValue(std::vector<T>& v, T val) {
            auto it = std::find(v.begin(), v.end(), val);
            if(it != v.end()) {
                swap(*it, v.back());
                v.pop_back();
            }
        }

    private:
        VectorUtils() = default;
    };

}
