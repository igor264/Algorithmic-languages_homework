#include <iostream>
#include <fstream>
#include <vector>
#include "TwoFish.h"
#include "TwoFish.cpp"

void encryptWavFile(const char* inputFile, const char* outputFile, BYTE* key, size_t keyLength) {
    // Открытие входного .wav файла
    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile) {
        std::cerr << "Ошибка открытия входного файла!" << std::endl;
        return;
    }

    // Чтение данных из входного файла
    std::vector<BYTE> fileData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    // Проверка, что файл имеет заголовок .wav (RIFF и WAVE метки)
    if (fileData.size() < 44 || fileData[0] != 'R' || fileData[1] != 'I' || fileData[2] != 'F' || fileData[8] != 'W' || fileData[9] != 'A' || fileData[10] != 'V' || fileData[11] != 'E') {
        std::cerr << "Файл не является .wav!" << std::endl;
        return;
    }

    // Создание экземпляра TwoFish
    TwoFish twofish(key, keyLength);

    // Шифрование только данных аудио (после заголовка)
    size_t headerSize = 44;
    size_t dataSize = fileData.size() - headerSize;
    size_t blockSize = 16;

    for (size_t i = headerSize; i < fileData.size(); i += blockSize) {
        BYTE block[blockSize] = {0}; // Блок данных
        size_t blockEnd = std::min(i + blockSize, fileData.size());
        std::copy(fileData.begin() + i, fileData.begin() + blockEnd, block);

        // Шифрование блока
        twofish.encrypt(block);

        // Запись зашифрованного блока обратно
        std::copy(block, block + blockSize, fileData.begin() + i);
    }

    // Запись зашифрованного файла
    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile) {
        std::cerr << "Ошибка открытия выходного файла!" << std::endl;
        return;
    }
    outFile.write(reinterpret_cast<char*>(fileData.data()), fileData.size());
    outFile.close();

    std::cout << "Файл успешно зашифрован!" << std::endl;
}