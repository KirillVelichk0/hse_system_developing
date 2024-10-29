#include "gammagenerator.h"
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <cstring>
#include <iostream>

namespace gammagenerator {
template <unsigned int keySize, unsigned int ivSize>
void derive_key_iv(const std::string& password, unsigned char* key, unsigned char* iv) {
    unsigned char hash[SHA384_DIGEST_LENGTH];
    static_assert(keySize + ivSize == SHA384_DIGEST_LENGTH, "");
    SHA384(reinterpret_cast<const unsigned char*>(password.c_str()), password.size(), hash);
    std::memcpy(key, hash, keySize);
    std::memcpy(iv, hash + keySize, ivSize);
}

std::vector<unsigned char> encrypt(const std::string& password, const std::span<unsigned char> plaintext) {
    constexpr unsigned int key_size = 32;
    constexpr unsigned int iv_size = 16;
    unsigned char key[key_size];
    unsigned char iv[iv_size];

    derive_key_iv<key_size, iv_size>(password, key, iv);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), nullptr, key, iv);

    std::vector<unsigned char> ciphertext(plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_ctr()));
    int len = 0;
    int ciphertext_len = 0;

    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size());
    ciphertext_len += len;

    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(ciphertext_len);
    return ciphertext;
}

std::vector<unsigned char> decrypt(const std::string& password, const std::span<unsigned char> ciphered){
    constexpr unsigned int key_size = 32;
    constexpr unsigned int iv_size = 16;
    unsigned char key[key_size];
    unsigned char iv[iv_size];

    derive_key_iv<key_size, iv_size>(password, key, iv);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_ctr(), nullptr, key, iv);

    std::vector<unsigned char> ciphertext(ciphered.size() + EVP_CIPHER_block_size(EVP_aes_256_ctr()));
    int len = 0;
    int ciphertext_len = 0;

    EVP_DecryptUpdate(ctx, ciphertext.data(), &len, ciphered.data(), ciphered.size());
    ciphertext_len += len;

    EVP_DecryptFinal_ex(ctx, ciphertext.data() + len, &len);
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(ciphertext_len);
    return ciphertext;
}
}
