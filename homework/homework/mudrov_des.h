#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <cstdint>
#include <cstring>

using namespace std;

// Шаблон для ключа DES (64 бита)
typedef uint64_t Key64;

// Функции для DES
// Функция для простого шифрования одного блока (64 бита) с ключом (64 бита)
uint64_t desEncryptBlock(uint64_t block, Key64 key) {
    // Для упрощения будем использовать XOR для демонстрации
    return block ^ key;
}

uint64_t desDecryptBlock(uint64_t block, Key64 key) {
    // Аналогично, дешифрование это просто XOR с тем же ключом
    return block ^ key;
}

// Функция для шифрования или дешифрования аудио данных
void processAudio(vector<uint8_t>& audioData, Key64 key, bool encrypt) {
    size_t blockSize = 8; // 64 бита = 8 байт
    for (size_t i = 0; i < audioData.size(); i += blockSize) {
        uint64_t block = 0;
        // Преобразуем 8 байт в 64-битное число
        for (size_t j = 0; j < blockSize; ++j) {
            block |= (static_cast<uint64_t>(audioData[i + j]) << (8 * j));
        }

        // Шифруем или дешифруем блок
        if (encrypt) {
            block = desEncryptBlock(block, key);
        } else {
            block = desDecryptBlock(block, key);
        }

        // Разбиваем 64-битный блок обратно на 8 байт
        for (size_t j = 0; j < blockSize; ++j) {
            audioData[i + j] = static_cast<uint8_t>((block >> (8 * j)) & 0xFF);
        }
    }
}

std::vector<uint8_t> DesEncrypt(vector<uint8_t> audioData, Key64 key) {
    // Шифрование аудио данных
    processAudio(audioData, key, true);

    return audioData;
}

std::vector<uint8_t> DesDecrypt(vector<uint8_t> audioData, Key64 key) {
    // Шифрование аудио данных
    processAudio(audioData, key, false);

    return audioData;
}
