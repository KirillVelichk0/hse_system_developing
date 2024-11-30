#include "md5calcer.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <openssl/md5.h>

Md5Calcer::Md5Calcer(const std::initializer_list<std::string>& initialWords) : m_gen(initialWords){
}

std::string Md5Calcer::GetNextWord()
{
    return m_gen.GetNext();
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
    return result;
}

WordsGenerator::WordsGenerator(const std::initializer_list<std::string> &initialWords) : m_words(initialWords.begin(), initialWords.end())
{

}

std::string WordsGenerator::GetNext()
{
    auto index = this->GetCurrentIndex();
    auto& wordCont = m_words[index];
    return wordCont.GoNext();
}
