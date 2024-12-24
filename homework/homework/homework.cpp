#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "shaev.h" 
#include "demidov/demidov.cpp"
#include "mudrov_des.h"
// #include "petuhova/�������.cpp"
#include "HW_Verlevsky.cpp"

using std::cin;

int main()
{
    setlocale(LC_ALL, "RUS");
    const std::string inputFilePath = "./homework/homework/input_audio.wav";

    // ��������� ������� ����
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    if (!inputFile) {
        std::cerr << "������ �������� �����!" << std::endl;
        return 1;
    }

    // ������ ��������� (������ 44 ����� WAV) (��� ������������ ������� wav ��� ��������� ������������ ������)
    std::vector<unsigned char> header(44);
    inputFile.read(reinterpret_cast<char*>(header.data()), header.size());

    // ������ �����������
    std::vector<unsigned char> audioData((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();

    std::vector<unsigned char> decryptedAudioData;

    short int menu_choice;
    std::cout << "1 - ����\n" << "2 - �������\n" << "3 - ��������\n" << "4 - ����������\n" << "5 - ������\n";

    while (!(cin >> menu_choice) || (cin.peek() != '\n') || (menu_choice < 1 || menu_choice > 5)) {
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "" << "Select correct item from menu:\n";
    };

    // ����� ������� ����������
    auto start = std::chrono::high_resolution_clock::now();

    switch (menu_choice) 
    {
        case 1: // shaev RC4
        {
            // ������ ����� ��� ���������� RC4
            std::vector<unsigned char> key_shaev = { 0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F };  // ������ �����

            // ������� ������ RC4
            rc4EncryptDecrypt(audioData, key_shaev);
        }
        case 2: // demidov TwoFish
        {
            // ���� (16 ���� ��� 128-������� �����)
            BYTE key[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};

            // ����������� vector<unsigned char> � vector<BYTE>
            std::vector<BYTE> byteAudioData(audioData.begin(), audioData.end());
            
            // ������� ������ TwoFish 
            audioData = twoFishEncrypt(byteAudioData, key, sizeof(key));

            copy(audioData.begin(), audioData.end(), std::back_inserter(decryptedAudioData));

            // ��������� ����������� ������ TwoFish
            decryptedAudioData = twoFishDecrypt(decryptedAudioData, key, sizeof(key)); 
        }
        case 3: // Petuhova Furie
        {
            // start(audioData);
        }
        case 4: //Verlevsky ECC
        {
            // ECCprocessAudioFile(audioData); // ���� ��� ���������� ��� ����� ������ �� ����, �� ���
        }
        case 5: // Mudrov DES
        {
            // (8 ���� ��� 64-������� �����)
            Key64 key = 0x133457799BBCDFF1; // ������ 64-������� ����� DES

            // ������� ������ DES
            audioData = DesEncrypt(audioData, key);

            copy(audioData.begin(), audioData.end(), std::back_inserter(decryptedAudioData));

            decryptedAudioData = DesDecrypt(decryptedAudioData, key); 
        }
    }
     // ���������� ��������� (������������� ������) � ����� WAV ����
    std::ofstream outputFile("./homework/homework/outfile_encrypted.wav", std::ios::binary);
    if (!outputFile) {
        std::cerr << "������ ��� �������� ��������� �����!" << std::endl;
        return 1;
    }

    // ���������� ��������� � ���������� ����������� ��� ��������������
    outputFile.write(reinterpret_cast<char*>(header.data()), header.size());
    outputFile.write(reinterpret_cast<char*>(audioData.data()), audioData.size());
    outputFile.close();

     // ���������� ��������� (�������������� ������) � ����� WAV ����
    std::ofstream outputFile2("./homework/homework/outfile_decrypted.wav", std::ios::binary);
    if (!outputFile) {
        std::cerr << "������ ��� �������� ��������� �����!" << std::endl;
        return 1;
    }

    // ���������� ��������� � ���������� ����������� ��� ���������������
    outputFile2.write(reinterpret_cast<char*>(header.data()), header.size());
    outputFile2.write(reinterpret_cast<char*>(decryptedAudioData.data()), decryptedAudioData.size());
    outputFile2.close();
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "����� ����������: " << duration.count() << " ������" << std::endl;

    return 0;
}
