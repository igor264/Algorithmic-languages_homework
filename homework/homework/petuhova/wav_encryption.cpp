#include "wav_encryption.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>

// Конструктор WavHeader
WavHeader::WavHeader(int totalSamples) {
    blockAlign = numChannels * bitsPerSample / 8;
    byteRate = sampleRate * blockAlign;
    dataSize = totalSamples * blockAlign;
    chunkSize = 36 + dataSize;
}

// Функция для БПФ
void FFT(std::vector<Complex>& data, bool inverse) {
    size_t N = data.size();
    if (N <= 1) return;

    std::vector<Complex> even(N / 2), odd(N / 2);
    for (size_t i = 0; i < N / 2; ++i) {
        even[i] = data[i * 2];
        odd[i] = data[i * 2 + 1];
    }

    FFT(even, inverse);
    FFT(odd, inverse);

    for (size_t k = 0; k < N / 2; ++k) {
        double angle = 2 * PI * k / N * (inverse ? 1 : -1);
        Complex t = std::polar(1.0, angle) * odd[k];
        data[k] = even[k] + t;
        data[k + N / 2] = even[k] - t;
    }

    if (inverse) {
        for (auto& x : data) x /= 2;
    }
}

// Функция для шифрования
void multiplyCipher(std::vector<Complex>& data) {
    size_t dataSize = data.size();

    for (size_t i = 0; i < dataSize; ++i) {
        if ((i + 1) % 2 == 0) data[i] /= 4;
        if ((i + 1) % 3 == 0) data[i] *= i;
        if ((i + 1) % 5 == 0) data[i] -= 45;
        if ((i + 1) % 7 == 0) data[i] *= 0.5;
        if ((i + 1) % 11 == 0) data[i] /= 12;
        if ((i + 1) % 17 == 0) data[i] -= 100;
        if ((i + 1) % 19 == 0) data[i] *= -70;
        if ((i + 1) % 23 == 0) data[i] += 150;
        if ((i + 1) % 29 == 0) data[i] /= 20;
        if ((i + 1) % 31 == 0) data[i] *= 1.2;
        if ((i + 1) % 37 == 0) data[i] /= -1.8;
    }
}

// Функция для расшифровки с обратными правилами
void multiplyDecipher(std::vector<Complex>& data) {
    size_t dataSize = data.size();

    for (size_t i = 0; i < dataSize; ++i) {
        if ((i + 1) % 37 == 0) data[i] *= -1.8;
        if ((i + 1) % 31 == 0) data[i] /= 1.2;
        if ((i + 1) % 29 == 0) data[i] *= 20;
        if ((i + 1) % 23 == 0) data[i] -= 150;
        if ((i + 1) % 19 == 0) data[i] /= -70;
        if ((i + 1) % 17 == 0) data[i] += 100;
        if ((i + 1) % 13 == 0) data[i] /= 100;
        if ((i + 1) % 11 == 0) data[i] *= 12;
        if ((i + 1) % 7 == 0) data[i] /= 0.5;
        if ((i + 1) % 5 == 0) data[i] += 45;
        if ((i + 1) % 3 == 0) data[i] /= i;
        if ((i + 1) % 2 == 0) data[i] *= 4;
    }
}
