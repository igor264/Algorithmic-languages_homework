#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include "wav_encryption.h"

int processAudioData(const std::vector<unsigned char>& audioData) {
    if (audioData.empty()) {
        std::cerr << "Error: Input data is empty or could not be read.\n";
        return 1;
    }

    // Преобразование в short (16-битные значения)
    size_t totalSamples = audioData.size() / sizeof(short);
    std::vector<short> buffer(totalSamples);
    std::memcpy(buffer.data(), audioData.data(), audioData.size());

    // Подготовка данных для обработки
    std::vector<Complex> freq_data(totalSamples);
    for (size_t i = 0; i < totalSamples; ++i) {
        freq_data[i] = Complex(buffer[i], 0.0);
    }

    // Выполнение шифрования
    FFT(freq_data, false);
    multiplyCipher(freq_data);
    FFT(freq_data, true);

    // Конвертация результатов обратно в short
    for (size_t i = 0; i < totalSamples; ++i) {
        double sample = freq_data[i].real();
        if (sample > 32767.0) sample = 32767.0;
        if (sample < -32768.0) sample = -32768.0;
        buffer[i] = static_cast<short>(sample);
    }

    // Запись зашифрованного файла
    WavHeader encryptedHeader(totalSamples);
    std::ofstream encryptedFile("end.wav", std::ios::binary);
    encryptedFile.write(reinterpret_cast<char*>(&encryptedHeader), sizeof(WavHeader));
    encryptedFile.write(reinterpret_cast<char*>(buffer.data()), buffer.size() * sizeof(short));
    encryptedFile.close();

    // Расшифровка
    for (size_t i = 0; i < totalSamples; ++i) {
        freq_data[i] = Complex(buffer[i], 0.0);
    }
    FFT(freq_data, false);
    multiplyDecipher(freq_data);
    FFT(freq_data, true);

    // Конвертация результатов обратно в short
    for (size_t i = 0; i < totalSamples; ++i) {
        double sample = freq_data[i].real();
        if (sample > 32767.0) sample = 32767.0;
        if (sample < -32768.0) sample = -32768.0;
        buffer[i] = static_cast<short>(sample);
    }

    // Запись расшифрованного файла
    WavHeader decryptedHeader(totalSamples);
    std::ofstream decryptedFile("deco.wav", std::ios::binary);
    decryptedFile.write(reinterpret_cast<char*>(&decryptedHeader), sizeof(WavHeader));
    decryptedFile.write(reinterpret_cast<char*>(buffer.data()), buffer.size() * sizeof(short));
    decryptedFile.close();
    std::remove("deco.wav");

    std::cout << "Encrypted file saved as: end.wav\n";
    std::cout << "Decrypted file saved as: dec.wav\n";
    return 0;
}

int main() {
    const std::string inputFile = "start.wav";

    std::ifstream inputFileStream(inputFile, std::ios::binary);
    if (!inputFileStream) {
        std::cerr << "Error: Cannot open input file.\n";
        return 1;
    }

    // Загрузка данных в память
    std::vector<unsigned char> audioData((std::istreambuf_iterator<char>(inputFileStream)),
        std::istreambuf_iterator<char>());
    inputFileStream.close();

    return processAudioData(audioData);
}

#include "wav_encryption.h"

int start() {
    const std::string inputFile = "start.wav";

    std::ifstream inputFileStream(inputFile, std::ios::binary);
    if (!inputFileStream) {
        std::cerr << "Error: Cannot open input file.\n";
        return 1;
    }

    // Загрузка данных в память
    std::vector<unsigned char> audioData((std::istreambuf_iterator<char>(inputFileStream)),
        std::istreambuf_iterator<char>());
    inputFileStream.close();

    if (audioData.empty()) {
        std::cerr << "Error: Input file is empty or could not be read.\n";
        return 1;
    }

    // Преобразование в short (16-битные значения)
    size_t totalSamples = audioData.size() / sizeof(short);
    std::vector<short> buffer(totalSamples);
    std::memcpy(buffer.data(), audioData.data(), audioData.size());

    // Подготовка данных для обработки
    std::vector<Complex> freq_data(totalSamples);
    for (size_t i = 0; i < totalSamples; ++i) {
        freq_data[i] = Complex(buffer[i], 0.0);
    }

    // Выполнение шифрования
    FFT(freq_data, false);
    multiplyCipher(freq_data);
    FFT(freq_data, true);

    // Конвертация результатов обратно в short
    for (size_t i = 0; i < totalSamples; ++i) {
        double sample = freq_data[i].real();
        if (sample > 32767.0) sample = 32767.0;
        if (sample < -32768.0) sample = -32768.0;
        buffer[i] = static_cast<short>(sample);
    }

    // Запись зашифрованного файла
    WavHeader encryptedHeader(totalSamples);
    std::ofstream encryptedFile("end.wav", std::ios::binary);
    encryptedFile.write(reinterpret_cast<char*>(&encryptedHeader), sizeof(WavHeader));
    encryptedFile.write(reinterpret_cast<char*>(buffer.data()), buffer.size() * sizeof(short));
    encryptedFile.close();

    // Расшифровка
    for (size_t i = 0; i < totalSamples; ++i) {
        freq_data[i] = Complex(buffer[i], 0.0);
    }
    FFT(freq_data, false);
    multiplyDecipher(freq_data);
    FFT(freq_data, true);

    // Конвертация результатов обратно в short
    for (size_t i = 0; i < totalSamples; ++i) {
        double sample = freq_data[i].real();
        if (sample > 32767.0) sample = 32767.0;
        if (sample < -32768.0) sample = -32768.0;
        buffer[i] = static_cast<short>(sample);
    }

    // Запись расшифрованного файла
    WavHeader decryptedHeader(totalSamples);
    std::ofstream decryptedFile("deco.wav", std::ios::binary);
    decryptedFile.write(reinterpret_cast<char*>(&decryptedHeader), sizeof(WavHeader));
    decryptedFile.write(reinterpret_cast<char*>(buffer.data()), buffer.size() * sizeof(short));
    decryptedFile.close();
    std::remove("deco.wav");

    std::cout << "Encrypted file saved as: end.wav\n";
    std::cout << "Decrypted file saved as: dec.wav\n";
    return 0;
}
