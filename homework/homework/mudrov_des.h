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

// Функция для расширения ключа (просто для демонстрации)
Key64 extendKey(Key64 key) {
    // Просто возвращаем ключ без изменений для простоты
    return key;
}

// Функция для простого шифрования одного блока (64 бита) с ключом (64 бита)
uint64_t desEncryptBlock(uint64_t block, Key64 key) {
    // На самом деле здесь должна быть сложная логика DES
    // Для упрощения будем использовать XOR для демонстрации
    return block ^ key;
}

uint64_t desDecryptBlock(uint64_t block, Key64 key) {
    // Аналогично, дешифрование это просто XOR с тем же ключом
    return block ^ key;
}

// Функция для чтения WAV-файла
bool readWav(const string& filename, vector<uint8_t>& buffer) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error opening file for reading!" << endl;
        return false;
    }

    // Пропускаем заголовок WAV файла (44 байта)
    file.seekg(44, ios::beg);
    
    // Читаем все данные аудио
    buffer = vector<uint8_t>((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    
    file.close();
    return true;
}

// Функция для записи WAV-файла
bool writeWav(const string& filename, const vector<uint8_t>& buffer) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error opening file for writing!" << endl;
        return false;
    }

    // Заголовок WAV файла
    uint8_t header[44] = {
        'R', 'I', 'F', 'F', 0, 0, 0, 0, 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ' , 16, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'd', 'a', 't', 'a', 0, 0, 0, 0
    };
    uint32_t dataSize = buffer.size();
    uint32_t fileSize = dataSize + 44 - 8;
    
    // Заполнение заголовка
    memcpy(header + 4, &fileSize, 4);
    memcpy(header + 40, &dataSize, 4);
    
    // Записываем заголовок и аудиоданные
    file.write(reinterpret_cast<char*>(header), 44);
    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    
    file.close();
    return true;
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

    // Для демонстрации, дешифруем обратно
    // vector<uint8_t> decryptedData = audioData;
    // processAudio(decryptedData, key, false);

    // cout << "Audio decryption complete." << endl;
}
