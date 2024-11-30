#include "bidirectionalpipe.h"
#include <fcntl.h>

BidirectionalPipe::BidirectionalPipe()
{
    auto createPipe = [](int (&fds)[2]){
        if(pipe(fds) == -1){
                throw std::runtime_error("Cant create fds");
        }
        int flags = fcntl(fds[0], F_GETFL, 0);
            if (flags == -1 || fcntl(fds[0], F_SETFL, flags | O_NONBLOCK) == -1) {
                throw std::runtime_error("Cant set non block");
       }
    };
    createPipe(m_childToParent.m_fds);
    createPipe(m_parentToChild.m_fds);

}
