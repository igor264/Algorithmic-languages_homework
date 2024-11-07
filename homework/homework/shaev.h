#pragma once
#ifndef RC4_H
#define RC4_H

#include <vector>
#include <iostream>

class RC4 {
public:
    RC4(const std::vector<unsigned char>& key) {
        // Инициализация S-блока
        for (int i = 0; i < 256; ++i) {
            S[i] = i;
        }

        int j = 0;
        for (int i = 0; i < 256; ++i) {
            j = (j + S[i] + key[i % key.size()]) % 256;
            std::swap(S[i], S[j]);
        }

        i = 0;
        j = 0;
    }

    unsigned char getNextByte() {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        std::swap(S[i], S[j]);
        return S[(S[i] + S[j]) % 256];
    }

private:
    unsigned char S[256];
    int i, j;
};

// Функция для выполнения RC4 шифрования или дешифровки
void rc4EncryptDecrypt(std::vector<unsigned char>& data, const std::vector<unsigned char>& key) {
    RC4 rc4(key);
    for (auto& byte : data) {
        byte ^= rc4.getNextByte();
    }
}

#endif
