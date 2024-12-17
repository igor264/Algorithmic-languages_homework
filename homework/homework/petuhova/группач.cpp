#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "wav_encryption.h"

int main() {
    std::string inputFile, outputFile;
    char mode;

    std::cout << "Enter input RAW audio file path: ";
    std::cin >> inputFile;
    std::cout << "Enter output WAV file path: ";
    std::cin >> outputFile;
    std::cout << "Mode (e for encryption, d for decryption): ";
    std::cin >> mode;

    // Чтение RAW-файла
    std::ifstream infile(inputFile, std::ios::binary);
    if (!infile) {
        std::cerr << "Error: Cannot open input file.\n";
        return 1;
    }

    std::vector<short> buffer(BUFFER_SIZE);
    std::vector<Complex> freq_data(BUFFER_SIZE);

    // Оценка общего числа сэмплов
    infile.seekg(0, std::ios::end);
    int totalSamples = infile.tellg() / sizeof(short);
    infile.seekg(0, std::ios::beg);

    // Запись заголовка WAV
    std::ofstream outfile(outputFile, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error: Cannot open output file.\n";
        return 1;
    }

    WavHeader header(totalSamples);
    outfile.write(reinterpret_cast<char*>(&header), sizeof(WavHeader));

    int samplesProcessed = 0;
    while (samplesProcessed < totalSamples) {
        size_t readcount = std::min(BUFFER_SIZE, totalSamples - samplesProcessed);
        infile.read(reinterpret_cast<char*>(buffer.data()), readcount * sizeof(short));

        for (size_t i = 0; i < BUFFER_SIZE; ++i) {
            freq_data[i] = (i < readcount) ? Complex(buffer[i], 0.0) : Complex(0.0, 0.0);
        }

        // БПФ, шифрование/расшифровка и обратное БПФ
        FFT(freq_data, false);
        if (mode == 'e') {
            multiplyCipher(freq_data);
        }
        else if (mode == 'd') {
            multiplyDecipher(freq_data);
        }
        else {
            std::cerr << "Invalid mode selected.\n";
            return 1;
        }
        FFT(freq_data, true);

        // Нормализация и запись
        for (size_t i = 0; i < readcount; ++i) {
            double sample = freq_data[i].real();
            if (sample > 32767.0) sample = 32767.0;
            if (sample < -32768.0) sample = -32768.0;
            buffer[i] = static_cast<short>(sample);
        }

        outfile.write(reinterpret_cast<char*>(buffer.data()), readcount * sizeof(short));
        samplesProcessed += readcount;
    }

    infile.close();
    outfile.close();

    std::cout << (mode == 'e' ? "Encrypted" : "Decrypted") << " WAV file saved to: " << outputFile << "\n";
    return 0;
}
