#include <iostream>
#include "gammagenerator.h"
#include <string>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>

std::fstream OpenFile(const std::string& userPath){
    std::error_code error_code;
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

void ChangeMagicNumber(const std::string& command, const std::string& path, const std::string& password){
    std::error_code error_code;
    auto fileStream = OpenFile(path);
    auto fileType = ReadFileType(fileStream);
    std::span<unsigned char> fileTypeSpan((unsigned char*)fileType.data(), fileType.size());
    std::vector<unsigned char> data;
    if(command == "encrypt"){
        data = gammagenerator::encrypt(password, fileTypeSpan);
    } else{
        data = gammagenerator::decrypt(password, fileTypeSpan);
    }
    WriteFileType(fileStream, std::span<unsigned char>(data.data(), data.size()));
}

int main(int argc, char* argv[])
{
    int returnCode = 0;
    if(argc == 3){
        std::string command(argv[1]);
        std::string path(argv[2]);
        if(command == "encrypt" || command == "decrypt"){
            std::cout << "Please enter password" << std::endl;
            std::string password;
            std::cin >> password;
            ChangeMagicNumber(command, path, password);
            std::cout << "Successfully " << command << "ed" << std::endl;

        } else{
            std::cout << "Uncorrect command " << command << std::endl;
            returnCode = 1;
        }
    } else{
        std::cout << "Uncorrect args count" << std::endl;
        returnCode = 1;
    }
    return returnCode;
}
