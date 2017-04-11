#include <functional>
#include <memory>

namespace ska {
    enum WorkNodeState {
        WORK_NONE,
        WORK_ACTIVE
    };

    class WorkNode {
    public:
        WorkNode();
        ~WorkNode() = default;

        WorkNode& then(std::function<void(void)> callback);
        void run();

    private:
        std::function<void(void)> m_callback;
        WorkNodeState m_state;
        std::unique_ptr<WorkNode> m_next;
    };
}
