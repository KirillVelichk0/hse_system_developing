#include "childsession.h"
#include <array>
#include <algorithm>
void ChildSession::InitWorkers()
{
    auto createTask = [this](status::ThreadStatus& status){
        return [weakSelf = this->weak_from_this(), &status](){
            auto self = weakSelf.lock();
            if(self == nullptr){
                status.TrySetStatus(status::Error);
                return;
            }
            self->StartWorkerTask(status);
        };
    };
    auto poolSize = std::thread::hardware_concurrency() - 1;
    for(int i = 0; i < poolSize; i++){
        auto& status = m_threads.m_statuses.emplace_back();
        m_threads.m_threads.emplace_back(createTask(status));
    }
}

void ChildSession::StartWorkerTask(status::ThreadStatus& status)
{
    try{
        if(!status.TrySetStatus(status::Active)){
            status.TrySetStatus(status::Error);
            return;
        }
        while(status.GetCurrentStatus() < status::Ready){
            auto calcResult = m_calcer->GoNext();
            if(calcResult.m_hash_16 == m_expected){
                status.TrySetStatus(status::Ready);
                this->SendData(calcResult.m_word);
            }
        }
    } catch(...){
        status.TrySetStatus(status::Error);
    }
}

void ChildSession::SendData(const std::string &data)
{
    std::lock_guard lock(m_writeMutex);
    m_pipe.WriteFromBuffer(data, data.size());
}

std::optional<std::string> ChildSession::ReadData()
{
    static std::array<char, 256> dataBuffer;
    std::string result;
    std::lock_guard lock(m_readMutex);
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
    try{
        InitWorkers();

    } catch(...){

    }
}
