#ifndef MD5CALCER_H
#define MD5CALCER_H
#include <queue>
#include <string>
#include <string_view>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <initializer_list>
#include <cstdint>

class WordCont{
private:
    mutable std::mutex m_mutex;
    mutable std::string m_word;

    auto GetCurWord() const{
        return m_word;
    }
    bool TryToUpdatePassedSymbol(std::int32_t passed) const{
        char& symbol = m_word[passed];
        auto lower = tolower(symbol);
        if((lower >= 'a' && lower < 'z') || (symbol >= '0' && symbol < '9')){
            symbol++;
            return true;
        }
        if(symbol == 'z'){
            symbol = 'A';
            return true;
        }
        if(symbol == 'Z'){
            symbol = '0';
            return true;
        }
        // This is 9
        if(passed != 1){
            if(TryToUpdatePassedSymbol(passed - 1)){
                symbol = 'a';
                return true;
            }
        }
        return false;
    }
    void UpdateWord() const{
        if(m_word.size() == 1){
            m_word += 'a';
            return;
        }
        if(!this->TryToUpdatePassedSymbol(m_word.size() - 1)){
            std::fill(std::next(m_word.begin(), 1), m_word.end(), 'a');
            m_word += 'a';
        }
    }
public:
    WordCont(const std::string& word) : m_word(word){
        if(m_word.size() == 0 ){
            throw std::runtime_error("Empty word");
        }
    }
    std::string GoNext() const{
        std::lock_guard lock(m_mutex);
        auto result = GetCurWord();
        this->UpdateWord();
        return result;
    }
};

class WordsGenerator{
private:
    std::atomic<std::uint16_t> m_curWordIndex;
    const std::vector<WordCont> m_words;
private:
    std::uint16_t GetCurrentIndex() noexcept{
        //размер вектора гарантированно неизменен
        return m_curWordIndex.fetch_add(1, std::memory_order_acq_rel) % m_words.size();
    }
public:
    WordsGenerator(const std::initializer_list<std::string>& initialWords);
    std::string GetNext();
};

struct CalcResult{
    std::string m_word;
    std::string m_hash_16;
};

class Md5Calcer : public std::enable_shared_from_this<Md5Calcer>
{
private:
    WordsGenerator m_gen;
private:
    Md5Calcer(const std::initializer_list<std::string>& initialWords);

private:
    std::string GetNextWord();
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
