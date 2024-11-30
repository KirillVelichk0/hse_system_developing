#ifndef IPCEXCEPTIONS_H
#define IPCEXCEPTIONS_H
#include <stdexcept>
#include "threadstatus.h"

namespace child{
class ChildStopSuccess : public std::runtime_error{
public:
    ChildStopSuccess();
};

class ChildStopError : public std::runtime_error{
public:
    ChildStopError();
};

class ChildStopInterrupt : public std::runtime_error{
public:
    ChildStopInterrupt();
};

void TryThrowFromStatusCode(status::ThreadStatusCode status);
}

#endif // IPCEXCEPTIONS_H
