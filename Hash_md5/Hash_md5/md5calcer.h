#ifndef MD5CALCER_H
#define MD5CALCER_H
#include <queue>
#include <string>
#include <string_view>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <initializer_list>


class ThreadQueue{
private:
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::queue<std::string> m_queue;
    bool m_isAcive = true;
public:
    ThreadQueue() = default;
    ~ThreadQueue();
    bool Push(std::string word);
    bool Pop(std::string& word);
    void Stop();
};

struct CalcResult{
    std::string m_word;
    std::string m_hash_16;
};

class Md5Calcer : public std::enable_shared_from_this<Md5Calcer>
{
private:
    ThreadQueue m_queue;
private:
    Md5Calcer(const std::string& initialWord);
    Md5Calcer(const std::initializer_list<std::string>& initialWords);

private:
    std::string GetNextWord();
    void GenerateNextWords(const std::string& word);
public:
    static std::shared_ptr<Md5Calcer> Create(const std::initializer_list<std::string>& initialWord);
    ~Md5Calcer() = default;
    CalcResult GoNext();
public:
    Md5Calcer(const Md5Calcer&)=delete;
    Md5Calcer(Md5Calcer&&)=delete;
    Md5Calcer& operator=(const Md5Calcer&)=delete;
    Md5Calcer& operator=(Md5Calcer&&)=delete;

};

CalcResult GenerateHash(const std::string& input);

#endif // MD5CALCER_H
