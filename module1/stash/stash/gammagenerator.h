#ifndef GAMMAGENERATOR_H
#define GAMMAGENERATOR_H
#include <vector>
#include <span>
#include <string>

namespace gammagenerator {

std::vector<unsigned char> encrypt(const std::string& password, const std::span<unsigned char> plaintext);

std::vector<unsigned char> decrypt(const std::string& password, const std::span<unsigned char> ciphered);

}

#endif // GAMMAGENERATOR_H
