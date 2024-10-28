#include "parentsession.h"
#include <algorithm>
#include <array>
#include <numeric>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void ParentSession::ForkSession(const std::initializer_list<std::string>& initWords)
{
    BidirectionalPipe pipe;
    pid_t pid = fork();
    if( pid < 0){
        throw std::runtime_error("Cant fork process");
    } else if(pid == 0){
        //дочерний процесс
        auto childSession = ChildSession::Create(initWords, m_expectedWord, std::move(pipe));
        childSession->StartSession(); //выход только через исключение ChildSessionExit
    } else{
        m_controller.AddSession(pid, std::move(pipe));
    }
}

void ParentSession::InitChilds()
{
    std::initializer_list<std::string> uppercaseLetters = {
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
        "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
        "U", "V", "W", "X", "Y", "Z"
    };

    std::initializer_list<std::string> lowercaseLetters = {
        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
        "k", "l", "m", "n", "o", "p", "q", "r", "s", "t",
        "u", "v", "w", "x", "y", "z"
    };

    std::initializer_list<std::string> digits = {
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
    };
    this->ForkSession(uppercaseLetters);
    this->ForkSession(lowercaseLetters);
    this->ForkSession(digits);
}

std::string ParentSession::Dispatch()
{
    try{
    while(true){
        auto answer = m_controller.TryGetAnswer();
        if(answer){
            return answer.value();
        }
        m_controller.UpdateStatuses();
    }
    } catch (ChildSessionExit& exit){
        throw;
    } catch(...){
        this->m_controller.InterruptAll();
        throw;
    }
}

ParentSession::ParentSession(const std::string &expected) : m_expectedWord(expected)
{

}

std::string ParentSession::FindAnswer()
{
    this->InitChilds();
    return this->Dispatch();
}

void ChildSessionsController::AddSession(int32_t sessionId, ParentEndPipe &&pipe)
{
    auto sessionData = std::make_shared<ChildSessionInfo>(sessionId, std::move(pipe));
    m_sessions.insert({sessionId, sessionData});
}

void ChildSessionsController::UpdateStatuses()
{
    int status;
    for(auto session: m_sessions){
        if(session.second->GetCurrentStatus() != ChildSessionStatus::Active){
            continue;
        }
        pid_t result = waitpid(session.first, &status, WNOHANG);
        if(result == session.first){
            m_exitedCounter++;
            if (WIFEXITED(status)) {
                if(WEXITSTATUS(status) == 0){
                    session.second->UpdateStatus(ChildSessionStatus::Done);
                } else{
                    session.second->UpdateStatus(ChildSessionStatus::Error);
                    throw ChildSessionError(session.first);
                }
            } else if (WIFSIGNALED(status)) {
                session.second->UpdateStatus(ChildSessionStatus::Interrupted);
            }
        }
    }
    if(m_exitedCounter == m_sessions.size()){
        throw AllSessionsDone();
    }
}

std::optional<std::string> ChildSessionsController::TryGetAnswer()
{
    std::optional<std::string> result;
    for(auto session: m_sessions){
        auto sessionInput = session.second->TryReadData();
        if(sessionInput){
            result = sessionInput;
            session.second->UpdateStatus(ChildSessionStatus::Done);
            break;
        }
    }
    return result;
}

void ChildSessionsController::InterruptAll()
{
    for(auto session: m_sessions){
        if(session.second->GetCurrentStatus() == ChildSessionStatus::Active){
            session.second->SendData("q");
            session.second->UpdateStatus(ChildSessionStatus::Interrupted);
        }
    }
}


bool ProcessPostfix(std::string& postFix, std::string& result){
    auto it = std::find(postFix.begin(), postFix.end(), '\0');
    result = std::string(postFix.begin(), it);
    if(it != postFix.end() && *it == '\0'){
        postFix = std::string(std::next(it, 1), postFix.end());
        return true;
    } else{
        postFix = std::string();
        return false;
    }
}

std::optional<std::string> ChildSessionInfo::TryReadData()
{
    static std::array<char, 256> dataBuffer;
    static std::string postFix;
    std::string result;
    if(ProcessPostfix(postFix, result)){
        return result;
    }
    auto bytesReaded = m_pipe.ReadToBuffer(dataBuffer, dataBuffer.size());
    if(bytesReaded == 0){
        return {};
    }
    bool goNext = true;
    auto begin = dataBuffer.begin();
    auto end = std::next(begin, bytesReaded);
    while(goNext){
        auto endIt = std::find(begin, end, '\0');
        goNext = (endIt == end);
        std::copy(dataBuffer.begin(), endIt, std::back_inserter(result));
        if(!goNext){
            postFix = std::string(std::next(endIt, 1), end);
        }
    }
    return result;
}

void ChildSessionInfo::SendData(const std::string &data)
{
    m_pipe.WriteFromBuffer(data, data.size());
}
