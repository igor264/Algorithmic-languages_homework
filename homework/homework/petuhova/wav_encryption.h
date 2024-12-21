#ifndef WAV_ENCRYPTION_H
#define WAV_ENCRYPTION_H

#include <vector>
#include <complex>
#include <string>

// ��������� � ���� ������
#define BUFFER_SIZE 1024
using Complex = std::complex<double>;
const double PI = 3.14159265358979323846;

// WAV-���������
struct WavHeader {
    char riff[4] = { 'R', 'I', 'F', 'F' };
    int chunkSize;
    char wave[4] = { 'W', 'A', 'V', 'E' };
    char fmt[4] = { 'f', 'm', 't', ' ' };
    int subchunk1Size = 16;
    short audioFormat = 1;
    short numChannels = 1;
    int sampleRate = 44100;
    int byteRate;
    short blockAlign;
    short bitsPerSample = 16;
    char data[4] = { 'd', 'a', 't', 'a' };
    int dataSize;

    WavHeader(int totalSamples);
};

// ������� ��� ���
void FFT(std::vector<Complex>& data, bool inverse);

// ������� ��� ����������
void multiplyCipher(std::vector<Complex>& data);

// ������� ��� ����������� � ��������� ���������
void multiplyDecipher(std::vector<Complex>& data);

#endif // WAV_ENCRYPTION_H

