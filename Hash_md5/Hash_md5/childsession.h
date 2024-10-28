#ifndef CHILDSESSION_H
#define CHILDSESSION_H
#include <initializer_list>
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include "threadguard.h"
#include "bidirectionalpipe.h"
#include "md5calcer.h"
#include "threadstatus.h"

class ChildSessionExit : public std::runtime_error{
public:
    ChildSessionExit() : std::runtime_error("Child session exit"){}
};


class ChildSession final : public std::enable_shared_from_this<ChildSession>
{
private:
    struct ThreadPool{
        std::vector<ThreadGuard> m_threads;
        std::vector<std::shared_ptr<status::ThreadStatus>> m_statuses;
    };
    ThreadPool m_threads;
    ChildEndPipe m_pipe;
    std::shared_ptr<Md5Calcer> m_calcer;
    std::string m_expected;
    std::mutex m_readMutex;
    std::mutex m_writeMutex;
private:
    void CheckParentInput();
    void InterruptAllWorkers();
    void CheckWorkersStatuses();
    void InitWorkers();
    void StartWorkerTask(std::shared_ptr<status::ThreadStatus> status);
    void SendData(const std::string& data);
    std::optional<std::string> ReadData();
    ChildSession(const std::initializer_list<std::string>& initWords, const std::string& expected, BidirectionalPipe&& pipe);
public:
    static std::shared_ptr<ChildSession> Create(const std::initializer_list<std::string>& initWords, const std::string& expected, BidirectionalPipe&& pipe);
    ~ChildSession() = default;
    [[noreturn]] void StartSession();
public:
    ChildSession(const ChildSession&)=delete;
    ChildSession(ChildSession&&)=delete;
    ChildSession& operator=(const ChildSession&)=delete;
    ChildSession& operator=(ChildSession&&)=delete;
};

#endif // CHILDSESSION_H
