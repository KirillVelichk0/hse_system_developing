#ifndef CHILDSESSION_H
#define CHILDSESSION_H
#include <initializer_list>
#include <vector>
#include <string>
#include <memory>
#include "threadguard.h"
#include "bidirectionalpipe.h"
#include "md5calcer.h"

class ChildSession final : std::enable_shared_from_this<ChildSession>
{
private:
    std::vector<ThreadGuard> m_threads;
    ChildEndPipe m_pipe;
    std::shared_ptr<Md5Calcer> m_calcer;
private:
    void InitWorkers();
    void StartWorkerTask();
    ChildSession(const std::initializer_list<std::string>& initWords, BidirectionalPipe&& pipe);
public:
    static std::shared_ptr<ChildSession> Create(const std::initializer_list<std::string>& initWords, BidirectionalPipe&& pipe);
    ~ChildSession() = default;
    [[noreturn]] void StartSession();
public:
    ChildSession(const ChildSession&)=delete;
    ChildSession(ChildSession&&)=delete;
    ChildSession& operator=(const ChildSession&)=delete;
    ChildSession& operator=(ChildSession&&)=delete;
};

#endif // CHILDSESSION_H
