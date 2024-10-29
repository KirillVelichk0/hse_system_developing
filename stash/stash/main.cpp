#include <iostream>
#include "gammagenerator.h"
#include <string>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>

std::fstream OpenFile(const std::string& userPath, std::error_code& error_code){
    const std::filesystem::path path(userPath);
    if (!std::filesystem::is_regular_file(path, error_code)) {
        throw std::invalid_argument("Is not regular file");
    }
    if (error_code) {
        std::runtime_error("Error in process");
    }

    std::fstream file;
    file.exceptions(std::fstream::failbit | std::fstream::badbit | std::fstream::eofbit);
    file.open(userPath, std::ios::in | std::ios::out | std::ios::binary);
    return file;
}

std::string ReadFileType(std::fstream& file){
    std::string result;
    file.seekg(std::ios::beg);
    result += file.get();
    result += file.get();
    return result;
}

void WriteFileType(std::fstream& file, std::span<unsigned char> data){
    file.seekp(std::ios::beg);
    for(char symbol: data){
        file.put(symbol);
    }
}

int main()
{
    std::string password = "Danila crazy!";
    std::uint64_t number = 123431325;
    std::string input = "Some interesing data";
    std::span inputSpan((unsigned char*)&number, sizeof(number));
    auto encrypted = gammagenerator::encrypt(password, inputSpan);
    std::span encrSpan(encrypted.data(), encrypted.size());
    std::cout << std::string(encrSpan.begin(), encrSpan.end()) << std::endl;
    auto result = gammagenerator::decrypt(password, encrSpan);
    std::uint64_t outputNumber = *((std::uint64_t*)result.data());
    std::string output(result.begin(), result.end());
    std::cout << outputNumber << std::endl;
}
