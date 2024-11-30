#include "threadguard.h"

ThreadGuard::~ThreadGuard(){
    if(m_thread.joinable()){
        m_thread.join();
    }
}
