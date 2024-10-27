#ifndef THREADGUARD_H
#define THREADGUARD_H
#include <thread>

class ThreadGuard final
{
private:
    std::thread m_thread;
public:
    template <class Functor>
    ThreadGuard(Functor&& functor) : m_thread(std::forward<Functor>(functor)){

    }
    ~ThreadGuard();
};

#endif // THREADGUARD_H
