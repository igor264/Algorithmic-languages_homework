#include <iostream>
#include <fstream>
#include <vector>
#include "TwoFish.h"
#include "TwoFish.cpp"

 std::vector<unsigned char> twoFishEncrypt(std::vector<unsigned char>& audioData, BYTE* key, size_t keyLength) {
    // Создание класса TwoFish
    TwoFish twofish(key, keyLength);

    // Шифрование только данных аудио (после заголовка)
    size_t headerSize = 44;
    size_t dataSize = audioData.size() - headerSize;
    size_t blockSize = 16;

    for (size_t i = headerSize; i < audioData.size(); i += blockSize) {
        BYTE block[blockSize] = {0}; // Блок данных
        size_t blockEnd = std::min(i + blockSize, audioData.size());
        std::copy(audioData.begin() + i, audioData.begin() + blockEnd, block);

        // Шифрование блока
        twofish.encrypt(block);

        // Запись зашифрованного блока обратно
        std::copy(block, block + blockSize, audioData.begin() + i);
    }

    return audioData;
}