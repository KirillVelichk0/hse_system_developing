#ifndef PARENTSESSION_H
#define PARENTSESSION_H
#include "childsession.h"
#include "bidirectionalpipe.h"
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <optional>
#include <string>
#include <stdexcept>

class ChildSessionError : std::runtime_error{
private:
    std::int32_t m_sessionId;
public:
    ChildSessionError(std::int32_t sessionId) : std::runtime_error("Child sesssion was crashed"), m_sessionId(sessionId){}
};

class AllSessionsDone : std::runtime_error{
public:
    AllSessionsDone() : std::runtime_error("All sessions Done"){}
};

enum class ChildSessionStatus{Active, Interrupted, Done, Error};

class ChildSessionInfo{
private:
    std::int32_t m_id;
    ChildSessionStatus m_lastKnownCode;
    ParentEndPipe m_pipe;
public:
    ChildSessionInfo(std::int32_t id, ParentEndPipe&& pipe) : m_id(id), m_lastKnownCode(ChildSessionStatus::Active), m_pipe(std::move(pipe)){}

    ChildSessionStatus GetCurrentStatus() const{
        return m_lastKnownCode;
    }
    void UpdateStatus(ChildSessionStatus newStatus){
        m_lastKnownCode = newStatus;
    }
    std::optional<std::string> TryReadData();
    void SendData(const std::string& data);
};

class ChildSessionsController{
private:
    std::unordered_map<std::int32_t, std::shared_ptr<ChildSessionInfo>> m_sessions;
    std::int32_t m_exitedCounter = 0;

public:
    ChildSessionsController()=default;
    ~ChildSessionsController()=default;

    void AddSession(std::int32_t sessionId, ParentEndPipe&& pipe);
    void UpdateStatuses();
    std::optional<std::string> TryGetAnswer();
    void InterruptAll();

public:
    ChildSessionsController(ChildSessionsController&&)=default;
    ChildSessionsController& operator=(ChildSessionsController&&)=default;

    ChildSessionsController(const ChildSessionsController&)=delete;
    ChildSessionsController& operator=(const ChildSessionsController&)=delete;

};


class ParentSession
{
private:
    ChildSessionsController m_controller;
    std::string m_expectedWord;
private:
    void ForkSession(const std::initializer_list<std::string>& initWords);
    void InitChilds();
    std::string Dispatch();
public:
    ParentSession(const std::string& expected);
    std::string FindAnswer();
};

#endif // PARENTSESSION_H
