#include "childsession.h"

void ChildSession::InitWorkers()
{
    auto createTask = [this](){
        return [weakSelf = this->weak_from_this()](){

        };
    };
    auto poolSize = std::thread::hardware_concurrency() - 1;
    for(int i = 0; i < poolSize; i++){

    }
}

void ChildSession::StartWorkerTask()
{

}

ChildSession::ChildSession(const std::initializer_list<std::string> &initWords, BidirectionalPipe &&pipe) : m_pipe(std::move(pipe))
{
    m_calcer = Md5Calcer::Create(initWords);
}

std::shared_ptr<ChildSession> ChildSession::Create(const std::initializer_list<std::string> &initWords, BidirectionalPipe &&pipe)
{
    return std::shared_ptr<ChildSession>(new ChildSession(initWords, std::move(pipe)));
}

void ChildSession::StartSession()
{

}
