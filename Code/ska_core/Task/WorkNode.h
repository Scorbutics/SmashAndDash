#include <functional>
#include <memory>

namespace ska {
    enum WorkNodeState {
        WORK_NONE,
        WORK_ACTIVE
    };

    template <class C>
    class WorkNode {
    public:
        WorkNode() : m_state(WORK_NONE){
        }
        ~WorkNode() = default;

        WorkNode<C>& then(std::function<void(C&)> callback) {
            m_callback = callback;
            m_state = WORK_ACTIVE;
            m_next = std::make_unique<WorkNode<C>>();
            return *m_next.get();
        }

        void executeAndPop(C& caller) {
            if(m_next == nullptr || m_state == WORK_NONE) {
                return;
            }
            m_callback(caller);
            m_next->executeAndPop(caller);
            m_next = nullptr;
            m_state = WORK_NONE;
        }

    private:
        std::function<void(C&)> m_callback;
        WorkNodeState m_state;
        std::unique_ptr<WorkNode<C>> m_next;
    };
}
