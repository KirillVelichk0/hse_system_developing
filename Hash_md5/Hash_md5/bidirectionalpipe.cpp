#include "bidirectionalpipe.h"

BidirectionalPipe::BidirectionalPipe()
{
    auto createPipe = [](int (&fds)[2]){
        if(pipe(fds) == -1){
                throw std::runtime_error("Cant create fds");
        }
    };
    createPipe(m_childToParent.m_fds);
    createPipe(m_parentToChild.m_fds);

}
