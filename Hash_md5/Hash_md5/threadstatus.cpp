#include "threadstatus.h"

ThreadStatus::ThreadStatus() : m_status(0)
{

}

bool ThreadStatus::TrySetStatus(ThreadStatusCode code)
{
    bool result = false;
    std::uint16_t currentStatus = this->GetCurrentStatus();
    while(code > currentStatus){
        if(m_status.compare_exchange_weak(currentStatus, (std::uint16_t)code, std::memory_order_acq_rel, std::memory_order_relaxed)){
            result = true;
            break;
        }
    }
    return result;
}

ThreadStatusCode ThreadStatus::GetCurrentStatus() const
{
    std::uint16_t status = m_status.load(std::memory_order_acquire);
    return static_cast<ThreadStatusCode>(status);
}
