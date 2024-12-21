#include "ECC.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <ctime>

using namespace std;

// Конструктор Point
ECC::Point::Point() : x(0), y(0), infinity(true) {}
ECC::Point::Point(int x, int y) : x(x), y(y), infinity(false) {}

// Конструктор ECC
ECC::ECC(const Curve& curve, const Point& basePoint, int privateKey)
    : curve(curve), basePoint(basePoint), privateKey(privateKey) {
    publicKey = multiply(basePoint, privateKey, curve);
}

// Проверка точки на кривой
bool ECC::isOnCurve(const Point& point, const Curve& curve) {
    if (point.infinity) return true;
    int lhs = mod(point.y * point.y, curve.p);
    int rhs = mod(point.x * point.x * point.x + curve.a * point.x + curve.b, curve.p);
    return lhs == rhs;
}

// Сложение точек
ECC::Point ECC::add(const Point& p1, const Point& p2, const Curve& curve) {
    if (p1.infinity) return p2;
    if (p2.infinity) return p1;

    if (p1.x == p2.x && p1.y != p2.y) return Point();

    int lambda;
    if (p1.x == p2.x && p1.y == p2.y) {
        lambda = mod((3 * p1.x * p1.x + curve.a) * modInverse(2 * p1.y, curve.p), curve.p);
    }
    else {
        lambda = mod((p2.y - p1.y) * modInverse(p2.x - p1.x, curve.p), curve.p);
    }

    int x3 = mod(lambda * lambda - p1.x - p2.x, curve.p);
    int y3 = mod(lambda * (p1.x - x3) - p1.y, curve.p);

    return Point(x3, y3);
}

// Умножение точки на число
ECC::Point ECC::multiply(const Point& p, int k, const Curve& curve) {
    Point result;
    Point addend = p;

    while (k) {
        if (k & 1) result = add(result, addend, curve);
        addend = add(addend, addend, curve);
        k >>= 1;
    }
    return result;
}

// Шифрование
ECC::Point ECC::encrypt(const Point& message) {
    if (!isOnCurve(message, curve)) throw runtime_error("Сообщение не на кривой.");
    Point kG = multiply(basePoint, privateKey, curve);
    return add(message, kG, curve);
}

// Дешифрование
ECC::Point ECC::decrypt(const Point& cipherText) {
    Point kG = multiply(basePoint, privateKey, curve);
    kG.y = mod(-kG.y, curve.p); // Инверсия y
    return add(cipherText, kG, curve);
}

// Нормализация аудиоданных
vector<int> ECC::normalizeAudioData(const vector<unsigned char>& audioBytes, int p) {
    vector<int> normalized;
    for (unsigned char byte : audioBytes) {
        normalized.push_back(mod(byte, p));
    }
    return normalized;
}

// Маппинг числа на точку кривой
ECC::Point ECC::mapToCurve(int message, const Curve& curve) {
    int x = mod(message, curve.p);
    for (int i = 0; i < curve.p; ++i) {
        int y_squared = mod(x * x * x + curve.a * x + curve.b, curve.p);
        for (int y = 0; y < curve.p; ++y) {
            if (mod(y * y, curve.p) == y_squared) {
                return Point(x, y);
            }
        }
        x = mod(x + 1, curve.p);
    }
    throw runtime_error("Не удалось найти точку на кривой.");
}

// Вспомогательные методы
int ECC::mod(int a, int b) { return (a % b + b) % b; }
int ECC::modInverse(int k, int p) {
    k = mod(k, p);
    for (int x = 1; x < p; ++x) {
        if (mod(k * x, p) == 1) return x;
    }
    throw runtime_error("Обратный элемент не существует.");
}

void ECCprocessAudioFile(ifstream& audioFile) {
    srand(static_cast<unsigned int>(time(0)));

    // Параметры эллиптической кривой
    ECC::Curve curve = { 2, 3, 97 };
    ECC::Point basePoint(3, 6);
    int privateKey = 7;

    ECC ecc(curve, basePoint, privateKey);

    if (!audioFile) {
        cerr << "Не удалось открыть файл." << endl;
        return;
    }

    vector<unsigned char> audioBytes((istreambuf_iterator<char>(audioFile)), istreambuf_iterator<char>());
    audioFile.close();

    vector<int> normalizedAudio = ECC::normalizeAudioData(audioBytes, curve.p);
    vector<ECC::Point> encryptedPoints;

    // Шифрование
    cout << "Начинаю шифрование..." << endl;
    for (int data : normalizedAudio) {
        try {
            ECC::Point message = ECC::mapToCurve(data, curve);
            ECC::Point encrypted = ecc.encrypt(message);
            encryptedPoints.push_back(encrypted);
        }
        catch (const runtime_error& e) {
            cerr << e.what() << endl;
        }
    }

    // Сохранение зашифрованных данных в формате WAV
    ofstream encryptedWav("encrypted_audio.wav", ios::binary);
    if (!encryptedWav) {
        cerr << "Не удалось создать файл encrypted_audio.wav" << endl;
        return;
    }

    // Запись минимального заголовка WAV
    uint32_t fileSize = 44 + encryptedPoints.size() * sizeof(int) * 2;
    encryptedWav.write("RIFF", 4);
    encryptedWav.write(reinterpret_cast<const char*>(&fileSize), sizeof(fileSize));
    encryptedWav.write("WAVEfmt ", 8);

    uint32_t subchunk1Size = 16;
    uint16_t audioFormat = 1;
    uint16_t numChannels = 1;
    uint32_t sampleRate = 44100;
    uint32_t byteRate = sampleRate * numChannels * sizeof(int);
    uint16_t blockAlign = numChannels * sizeof(int);
    uint16_t bitsPerSample = sizeof(int) * 8;

    encryptedWav.write(reinterpret_cast<const char*>(&subchunk1Size), sizeof(subchunk1Size));
    encryptedWav.write(reinterpret_cast<const char*>(&audioFormat), sizeof(audioFormat));
    encryptedWav.write(reinterpret_cast<const char*>(&numChannels), sizeof(numChannels));
    encryptedWav.write(reinterpret_cast<const char*>(&sampleRate), sizeof(sampleRate));
    encryptedWav.write(reinterpret_cast<const char*>(&byteRate), sizeof(byteRate));
    encryptedWav.write(reinterpret_cast<const char*>(&blockAlign), sizeof(blockAlign));
    encryptedWav.write(reinterpret_cast<const char*>(&bitsPerSample), sizeof(bitsPerSample));

    encryptedWav.write("data", 4);
    uint32_t subchunk2Size = encryptedPoints.size() * sizeof(int) * 2;
    encryptedWav.write(reinterpret_cast<const char*>(&subchunk2Size), sizeof(subchunk2Size));

    // Запись данных
    for (const ECC::Point& encrypted : encryptedPoints) {
        encryptedWav.write(reinterpret_cast<const char*>(&encrypted.x), sizeof(encrypted.x));
        encryptedWav.write(reinterpret_cast<const char*>(&encrypted.y), sizeof(encrypted.y));
    }
    encryptedWav.close();
    cout << "Шифрование завершено, данные сохранены в encrypted_audio.wav" << endl;

    // Дешифрование
    cout << "Начинаю дешифрование..." << endl;
    vector<unsigned char> decryptedBytes;
    for (const ECC::Point& cipherText : encryptedPoints) {
        try {
            ECC::Point decrypted = ecc.decrypt(cipherText);
            decryptedBytes.push_back(static_cast<unsigned char>(decrypted.x % 256)); // Восстановление байта
        }
        catch (const runtime_error& e) {
            cerr << e.what() << endl;
        }
    }

    // Сохранение дешифрованных данных
    ofstream decryptedFile("decrypted_audio.wav", ios::binary);
    decryptedFile.write(reinterpret_cast<char*>(decryptedBytes.data()), decryptedBytes.size());
    decryptedFile.close();
    cout << "Дешифрование завершено, данные сохранены в decrypted_audio.wav" << endl;
}
