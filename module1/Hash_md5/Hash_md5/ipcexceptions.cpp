#include "ipcexceptions.h"


namespace child{
ChildStopSuccess::ChildStopSuccess() : std::runtime_error("Child stopped successfull")
{

}

ChildStopError::ChildStopError() : std::runtime_error("Child stop error")
{

}

ChildStopInterrupt::ChildStopInterrupt() : std::runtime_error("Child stop Interrupted")
{

}

void TryThrowFromStatusCode(status::ThreadStatusCode status)
{
    switch(status){
    case status::Error:
        throw ChildStopError();
        break;
    case status::Interrupted:
        throw ChildStopInterrupt();
        break;
    case status::Ready:
        throw ChildStopSuccess();
        break;
     default:
        return;
    }
}
}
