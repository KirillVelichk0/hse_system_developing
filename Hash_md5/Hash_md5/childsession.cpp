#include "childsession.h"
#include "ipcexceptions.h"
#include <array>
#include <algorithm>
#include <iostream>

void ChildSession::CheckParentInput()
{
    auto input = this->ReadData();
    // здесь может быть более сложная логика
    if(input){
        this->InterruptAllWorkers();
    }

}

void ChildSession::InterruptAllWorkers()
{
    for(auto status: m_threads.m_statuses){
        status->TrySetStatus(status::Interrupted);
    }
}

void ChildSession::CheckWorkersStatuses()
{
    for(auto status: m_threads.m_statuses){
        auto code = status->GetCurrentStatus();
        child::TryThrowFromStatusCode(code);
    }
}

void ChildSession::InitWorkers()
{
    auto createTask = [this](std::shared_ptr<status::ThreadStatus> status){
        return [weakSelf = this->weak_from_this(), status](){
            try{
                auto self = weakSelf.lock();
                if(self == nullptr){
                    status->TrySetStatus(status::Error);
                    return;
                }
                self->StartWorkerTask(status);
            } catch(...){
                status->TrySetStatus(status::Error);
            }
        };
    };
    auto poolSize = std::thread::hardware_concurrency() - 1;
    for(int i = 0; i < poolSize; i++){
        auto& status = m_threads.m_statuses.emplace_back();
        m_threads.m_threads.emplace_back(createTask(status));
    }
}

void ChildSession::StartWorkerTask(std::shared_ptr<status::ThreadStatus> status)
{
    if(!status->TrySetStatus(status::Active)){
        status->TrySetStatus(status::Error);
        return;
    }
    for(auto statusCode = status->GetCurrentStatus(); statusCode < status::Ready; statusCode = status->GetCurrentStatus()){
        auto calcResult = m_calcer->GoNext();
        if(calcResult.m_hash_16 == m_expected){
            status->TrySetStatus(status::Ready);
            this->SendData(calcResult.m_word);
        }
    }
}

void ChildSession::SendData(const std::string &data)
{
    std::lock_guard lock(m_writeMutex);
    m_pipe.WriteFromBuffer(data, data.size());
}

static bool ProcessPostfix(std::string& postFix, std::string& result){
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

std::optional<std::string> ChildSession::ReadData()
{
    static std::array<char, 256> dataBuffer;
    static std::string postFix;
    std::string result;
    std::lock_guard lock(m_readMutex);
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

ChildSession::ChildSession(const std::initializer_list<std::string> &initWords, const std::string& expected, BidirectionalPipe &&pipe) : m_pipe(std::move(pipe)), m_expected(expected)
{
    m_calcer = Md5Calcer::Create(initWords);
}

std::shared_ptr<ChildSession> ChildSession::Create(const std::initializer_list<std::string> &initWords, const std::string& expected, BidirectionalPipe &&pipe)
{
    return std::shared_ptr<ChildSession>(new ChildSession(initWords, expected, std::move(pipe)));
}

void ChildSession::StartSession()
{
    InitWorkers();
    while(true){
        try{
            this->CheckParentInput();
            this->CheckWorkersStatuses();
            std::this_thread::yield();
        } catch(std::exception& ex){
            this->InterruptAllWorkers();
            std::cout << "Child session done with id " <<getpid() << " and message: " << ex.what() << std::endl;
            throw ChildSessionExit();
        }
        catch(...){
            this->InterruptAllWorkers();
            std::cout << "Child session done with id " <<getpid() << std::endl;
            throw ChildSessionExit();
        }
    }
}
