#ifndef THREADSTATUS_H
#define THREADSTATUS_H
#include <atomic>
#include <cstdint>
namespace status{
enum ThreadStatusCode : std::uint16_t {NotActive, Active, Ready, Interrupted, Error};
class ThreadStatus
{
private:
    std::atomic<std::uint16_t> m_status;
public:
    ThreadStatus();
    ~ThreadStatus() = default;
    bool TrySetStatus(ThreadStatusCode code);
    ThreadStatusCode GetCurrentStatus() const;
};
}

#endif // THREADSTATUS_H
