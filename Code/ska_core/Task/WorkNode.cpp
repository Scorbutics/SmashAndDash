#include "WorkNode.h"

ska::WorkNode::WorkNode() : m_state(WORK_NONE) {
}

ska::WorkNode& ska::WorkNode::then(std::function<void(void)> callback) {
    m_callback = callback;
    m_state = WORK_ACTIVE;
    m_next = std::make_unique<WorkNode>();
    return *m_next.get();
}

void ska::WorkNode::run() {
    if(m_next == nullptr || m_state == WORK_NONE) {
        return;
    }
    m_callback();
    m_next->run();
    m_next = nullptr;
    m_state = WORK_NONE;
}
