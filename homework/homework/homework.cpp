#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "shaev.h" 
#include "demidov/demidov.cpp"
#include "mudrov_des.h"
// #include "petuhova/группач.cpp"
#include "HW_Verlevsky.cpp"

using std::cin;

int main()
{
    const std::string inputFilePath = "./homework/homework/input_audio.wav";

    // Открываем входной файл
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Ошибка открытия файла!" << std::endl;
        return 1;
    }

    // Читаем заголовок (первые 44 байта WAV) (для стандартного формата wav там находится заголовочные данные)
    std::vector<unsigned char> header(44);
    inputFile.read(reinterpret_cast<char*>(header.data()), header.size());

    // Читаем аудиоданные
    std::vector<unsigned char> audioData((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();

    short int menu_choice;
    std::cout << "1 - Шаев\n" << "2 - Демидов\n" << "3 - Петухова\n" << "4 - Верлевский\n" << "5 - Мудров\n";

    while (!(cin >> menu_choice) || (cin.peek() != '\n') || (menu_choice < 1 || menu_choice > 5)) {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "" << "Select correct item from menu:\n";
    };

    // Замер времени шифрования
    auto start = std::chrono::high_resolution_clock::now();

    switch (menu_choice) 
    {
        case 1: // shaev RC4
        {
            // Пример ключа для шифрования RC4
            std::vector<unsigned char> key_shaev = { 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F };  // Пример ключа

            // Шифруем данные RC4
            rc4EncryptDecrypt(audioData, key_shaev);
        }
        case 2: // demidov TwoFish
        {
            // Ключ (16 байт для 128-битного ключа)
            BYTE key[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};

            // Преобразуем vector<unsigned char> в vector<BYTE>
            std::vector<BYTE> byteAudioData(audioData.begin(), audioData.end());

            // Шифруем данные TwoFish 
            audioData = twoFishEncrypt(byteAudioData, key, sizeof(key));
        }
        case 3: // Petuhova Furie
        {
            // start(audioData);
        }
        case 4: //Verlevsky ECC
        {
            ECCprocessAudioFile(audioData);
        }
        case 5: // Mudrov DES
        {
            // (8 байт для 64-битного ключа)
            Key64 key = 0x133457799BBCDFF1; // Пример 64-битного ключа DES

            // Шифруем данные DES
            audioData = DesEncrypt(audioData, key);
        }
    }
     // Записываем результат (зашифрованные данные) в новый WAV файл
    std::ofstream outputFile("./homework/homework/outfile_encrypted.wav", std::ios::binary);
    if (!outputFile) {
        std::cerr << "Ошибка при открытии выходного файла!" << std::endl;
        return 1;
    }

    // Записываем заголовок и измененные аудиоданные
    outputFile.write(reinterpret_cast<char*>(header.data()), header.size());
    outputFile.write(reinterpret_cast<char*>(audioData.data()), audioData.size());
    outputFile.close();
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Время шифрования: " << duration.count() << " секунд" << std::endl;

    return 0;
}
