#include "md5calcer.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <openssl/md5.h>

bool ThreadQueue::Push(const std::string& word){
    std::lock_guard lock(m_mutex);
    if(!m_isAcive){
        return false;
    }
    m_queue.push(word);
    m_cond.notify_one();
    return true;
}
bool ThreadQueue::Pop(std::string& word){
    std::unique_lock lock(m_mutex);
    if(!m_isAcive){
        return false;
    }
    if(m_queue.empty()){
        m_cond.wait(lock, [this](){
            return !m_isAcive || !m_queue.empty();
        });
    }
    if(!m_isAcive){
        return false;
    }
    auto& wordInQueue = m_queue.front();
    word = std::move(wordInQueue);
    m_queue.pop();
    return true;
}

void ThreadQueue::Stop(){
    std::lock_guard lock(m_mutex);
    m_isAcive = false;
    m_cond.notify_all();
}

ThreadQueue::~ThreadQueue(){
    this->Stop();
}

Md5Calcer::Md5Calcer(const std::string& initialWord){
    m_queue.Push(initialWord);
}

Md5Calcer::Md5Calcer(const std::initializer_list<std::string>& initialWords){
    if(initialWords.size() == 0){
        throw std::runtime_error("Cant pass empty initial list to calcer");
    }
    for(const auto& word: initialWords){
        m_queue.Push(word);
    }
}

std::string Md5Calcer::GetNextWord()
{
    std::string next;
    m_queue.Pop(next);
    return next;
}

void Md5Calcer::GenerateNextWords(const std::string &word)
{
    auto wordCopy = word;
    constexpr auto diff1 = 'z' - 'a';
    constexpr auto diff2 = 10;
    auto setVars = [this](std::string& word, char initSymb, int diff){
        auto it = std::next(word.end(), -1);
        for(int i = 0; i <= diff; i++){
            char symb = initSymb + diff;
            *it = symb;
            m_queue.Push(word);
        }
    };
    wordCopy += 'a';
    setVars(wordCopy, 'a', diff1);
    setVars(wordCopy, 'A', diff1);
    setVars(wordCopy, '0', diff2);
}

CalcResult GenerateHash(const std::string &input)
{
    unsigned char hash[MD5_DIGEST_LENGTH];

    MD5(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

    std::ostringstream hexStream;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        hexStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    CalcResult result;
    result.m_word = input;
    result.m_hash_16 = hexStream.str();
    return result;
}

std::shared_ptr<Md5Calcer> Md5Calcer::Create(const std::initializer_list<std::string>& initialWord){
    return std::shared_ptr<Md5Calcer>(new Md5Calcer(initialWord));
}

CalcResult Md5Calcer::GoNext(){
    std::string cur = this->GetNextWord();
    auto result = GenerateHash(cur);
    this->GenerateNextWords(cur);
    return result;
}
