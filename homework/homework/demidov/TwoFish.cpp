#include "TwoFish.h"
 
/********************************Конструктор*********************************/
TwoFish::TwoFish(char *key, size_t length) {
    BYTE *u_key = new BYTE[length];
    for (int i = 0; i < length; i++) {
        u_key[i] = (BYTE) key[i]; // Преобразование символов ключа в байты
    }
    keySchedule(u_key, length); // Инициализация ключей (расписание ключей)
}
 
TwoFish::TwoFish(BYTE *key, size_t length){
    keySchedule(key, length);
}
 
/*********************************Деструктор*********************************/
TwoFish::~TwoFish() {
    delete[] SBox; // Освобождение памяти, выделенной для SBox
}
 
/*********************************Интерфейс**********************************/
BYTE* TwoFish::encrypt(BYTE *plain){
 
    UINT // Разбиваем входные данные на 4 блока по 32 бита (A, B, C, D)
        A = (plain[0] << 24) + (plain[1] << 16) + (plain[2] << 8) + plain[3],
        B = (plain[4] << 24) + (plain[5] << 16) + (plain[6] << 8) + plain[7],
        C = (plain[8] << 24) + (plain[9] << 16) + (plain[10] << 8) + plain[11],
        D = (plain[12] << 24) + (plain[13] << 16) + (plain[14] << 8) + plain[15];
    
    // Входное отбеливание (выполняет XOR блоков данных с первыми четырьмя подключами) 
    A ^= keys[0];
    B ^= keys[1];
    C ^= keys[2];
    D ^= keys[3];
 
    //16 раундов шифрования. Реализация функции F
    for(int i = 0; i < 16; i++){
        unsigned long long tA = h(A, SBox, k);
        unsigned long long tB = h(ROL(B, 8), SBox, k); // ROL
 
        D = ROL(D, 1); //ROL - Циклический сдвиг D на 1 бит влево
        C ^= ((tA + tB + keys[2 * i + 8]) & 0xFFFFFFFF); 
        D ^= ((tA + 2*tB + keys[2 * i + 9]) & 0xFFFFFFFF);
        C = ROR(C, 1); //ROR - Циклический сдвиг D на 1 бит вправо
 
        // Обмен значениями между A, B, C и D (за исключением последнего раунда)
        // Обеспечение диффузии данных
        if (i != 15) {
            UINT tmp = C;
            C = A;
            A = tmp;
            tmp = D;
            D = B;
            B = tmp;
        }
    }
 
    // Выходное отбеливание (выполняет XOR блоков данных с последними четырьмя подключами) 
    A ^= keys[4];
    B ^= keys[5];
    C ^= keys[6];
    D ^= keys[7];
    
    // Собираем зашифрованные блоки A, B, C и D обратно в 16 байтов и возвращаем их
    plain[0] = (A >> 24) & 0xFF;
    plain[1] = (A >> 16) & 0xFF;
    plain[2] = (A >> 8) & 0xFF;
    plain[3] = A & 0xFF;
    plain[4] = (B >> 24) & 0xFF;
    plain[5] = (B >> 16) & 0xFF;
    plain[6] = (B >> 8) & 0xFF;
    plain[7] = B & 0xFF;
    plain[8] = (C >> 24) & 0xFF;
    plain[9] = (C >> 16) & 0xFF;
    plain[10] = (C >> 8) & 0xFF;
    plain[11] = C & 0xFF;
    plain[12] = (D >> 24) & 0xFF;
    plain[13] = (D >> 16) & 0xFF;
    plain[14] = (D >> 8) & 0xFF;
    plain[15] = D & 0xFF;
    return plain;
}
 
void TwoFish::printSubkeys(){
    for(int i=0; i< 40; i++){
        cout<<hex<<keys[i]<<endl;
    }
}

/*******************************Системные методы*******************************/
void TwoFish::keySchedule(BYTE *user_key, size_t length){ // 
    short N = 0; // Определяем размер ключа
    if ( length > 192 ) {
        N = 256;
    } else if ((length > 128) && (length <=192)) {
        N = 192;
    } else N = 128;
    
    // Преобразуем ключ пользователя в формат, необходимый алгоритму
    char *temp_key = new char[N];
    for(int i = 0; i < N; i++){
        temp_key[i] = ( i < length ) ? user_key[i] : 0; // Дополняет ключ нулями
    }   
 
    //Инициализация переменных
    k = N / 64;
 
    BYTE RS[4][8] = {
        {0x01, 0xA4, 0x55, 0x87, 0x5A, 0x58, 0xDB, 0x9E},
        {0xA4, 0x56, 0x82, 0xF3, 0x1E, 0xC6, 0x68, 0xE5},
        {0x02, 0xA1, 0xFC, 0xC1, 0x47, 0xAE, 0x3D, 0x19},
        {0xA4, 0x55, 0x87, 0x5A, 0x58, 0xDB, 0x9E, 0x03}
    };
    UINT *Me = new UINT[k];
    UINT *Mo = new UINT[k];
 
    for(int c1 = 0, c2 = 0, i = 0; i < 2 * k; i++){
        if (i % 2 == 0) {
            Me[c1] = 0;
            for (int j = 4*i, shift = 3 ; j < 4*(i+1); j++, shift--){
                Me[c1] += (temp_key[j] << (shift * 8));             
            }           
            c1++;
        } else {
            Mo[c2] = 0;
            for (int j = 4*i, shift = 3 ; j < 4*(i+1); j++, shift--){
                Mo[c2] = temp_key[j] << (shift * 8);
            }                       
            c2++;
        }
    }
 
    SBox = new UINT[k];
    for( int i = 0; i < k; i++){
        SBox[k - 1 - i] = 0;        
        for (int j = 0; j < 4; j++){            
            UINT v = 0;
            for (int t = 0; t < 8; t++){
                v += RS[j][t] * temp_key[ 8*i + t ];
            }
            SBox[k - 1 - i] += ( v * (1 << (8 * j)));
        }
    }
 
    // Генерируем 40 подключей (32-битных значений) с использованием функции h
    UINT ro = (1 << 24) + (1 << 16) + (1 << 8) + 1;
    for(int i = 0; i < 20; i++){
        unsigned long long A = h( 2 * i * ro, Me, k); 
        unsigned long long B = h( (2 * i + 1) * ro, Mo, k); 
        B = ROL(B,8); //ROL
        keys[2 * i] = (A + B) & 0xFFFFFFFF;
        keys[2 * i + 1] = ROL(((A + 2 * B) & 0xFFFFFFFF), 9); //ROL
    }
}
 
unsigned long long TwoFish::h(UINT inputWord, UINT *inputWordArray, short ArraySize){ // Метод h – нелинейное преобразование
    BYTE x[4]; // Входное слово разбивается на 4 байта для применения функции q
    x[0] = (inputWord >> 24) & 0xFF;
    x[1] = (inputWord >> 16) & 0xFF;
    x[2] = (inputWord >> 8) & 0xFF;
    x[3] = inputWord & 0xFF;
    
    BYTE **l = new BYTE*[ArraySize]; // Применяет несколько слоев функции q с таблицей SBox и массивом l
    for (int i = 0; i < ArraySize; i++) {
        l[i] = new BYTE[4];
        l[i][0] = (inputWordArray[i] >> 24) && 0xFF;
        l[i][1] = (inputWordArray[i] >> 16) && 0xFF;
        l[i][2] = (inputWordArray[i] >> 8) && 0xFF;
        l[i][3] = inputWordArray[i] && 0xFF;
    }
 
    BYTE y[4] = {0};
    if (ArraySize == 4) {
        y[0] = q(x[0], 1) ^ l[3][0];
        y[1] = q(x[1], 0) ^ l[3][1];
        y[2] = q(x[2], 0) ^ l[3][2];
        y[3] = q(x[3], 1) ^ l[3][3];
    } else {
        y[0] = x[0];
        y[1] = x[1];
        y[2] = x[2];
        y[3] = x[3];
    }
 
    if (ArraySize >= 3) {
        y[0] = q(y[0], 1) ^ l[2][0];
        y[1] = q(y[1], 0) ^ l[2][1];
        y[2] = q(y[2], 0) ^ l[2][2];
        y[3] = q(y[3], 1) ^ l[2][3];
    } else {
        y[0] = x[0];
        y[1] = x[1];
        y[2] = x[2];
        y[3] = x[3];
    }
    //__________________qx_____________qx_____________qx
    y[0] = q((q((q(y[0], 0) ^ l[1][0]), 0) ^ l[0][0]), 1);
    y[1] = q((q((q(y[1], 1) ^ l[1][1]), 0) ^ l[0][1]), 0);
    y[2] = q((q((q(y[2], 0) ^ l[1][2]), 1) ^ l[0][2]), 1);
    y[3] = q((q((q(y[3], 1) ^ l[1][3]), 1) ^ l[0][3]), 0);
    
    // Умножает результат функции q на матрицу MDS (Multiplication-Diffusion Scaling) для обеспечения диффузии.
    BYTE MDS[4][4] = {
        {0x01, 0xEF, 0x5B, 0x5B},
        {0x5B, 0xEF, 0xEF, 0x01},
        {0xEF, 0x5B, 0x01, 0xEF},
        {0xEF, 0x01, 0xEF, 0x5B}
    };
 
    unsigned long long H = 0;       
    for (int j = 0; j < 4; j++){                    
        unsigned long long v = 0; 
        for (int t = 0; t < 4; t++){
            v += MDS[j][t] * y[t];
        }
        H += v * ((unsigned long long)(1 << (8 * j)));
    }   
 
    return H;
}
 
BYTE TwoFish::q(BYTE x, int op){ // Метод q – нелинейное преобразование с использованием таблицы
    assert( (op == 0) || (op == 1));
    const BYTE qt[2][4][16] = { 
         // Таблицы перестановок для q0
        {
            { 0x8, 0x1, 0x7, 0xD, 0x6, 0xF, 0x3, 0x2, 0x0, 0xB, 0x5, 0x9, 0xE, 0xC, 0xA, 0x4 },
            { 0xE, 0xC, 0xB, 0x8, 0x1, 0x2, 0x3, 0x5, 0xF, 0x4, 0xA, 0x6, 0x7, 0x0, 0x9, 0xD },
            { 0xB, 0xA, 0x5, 0xE, 0x6, 0xD, 0x9, 0x0, 0xC, 0x8, 0xF, 0x3, 0x2, 0x4, 0x7, 0x1 },
            { 0xD, 0x7, 0xF, 0x4, 0x1, 0x2, 0x6, 0xE, 0x9, 0xB, 0x3, 0x0, 0x8, 0x5, 0xC, 0xA }
        },
 
         // Таблицы перестановок для q1
        {
            { 0x2, 0x8, 0xB, 0xD, 0xF, 0x7, 0x6, 0xE, 0x3, 0x1, 0x9, 0x4, 0x0, 0xA, 0xC, 0x5 },
            { 0x1, 0xE, 0x2, 0xB, 0x4, 0xC, 0x3, 0x7, 0x6, 0xD, 0xA, 0x5, 0xF, 0x9, 0x0, 0x8 },
            { 0x4, 0xC, 0x7, 0x5, 0x1, 0x6, 0x9, 0xA, 0x0, 0xE, 0xD, 0x8, 0x2, 0xB, 0x3, 0xF },
            { 0xB, 0x9, 0x5, 0x1, 0xC, 0x3, 0xD, 0xE, 0x6, 0x4, 0x7, 0xF, 0x2, 0x0, 0x8, 0xA }
        },
    };
 
    // Разбиваем байт на два полубайта (a и b) и применяет перестановки согласно таблицам q
    BYTE a0 = x / 16; 
    BYTE b0 = x % 16;
 
    BYTE a1 = a0 ^ b0;
    BYTE b1 = a0 ^ ROR4(b0) ^ ((8 * a0) % 16); //ROR4
 
    BYTE a2 = qt[op][0][a1];
    BYTE b2 = qt[op][1][b1];
 
    BYTE a3 = a2 ^ b2;
    BYTE b3 = a2 ^ ROR4(b2) ^ ((8 * a2) % 16);  //ROR4
    BYTE a4 = qt[op][2][a3];
    BYTE b4 = qt[op][3][b3];
    
    return (16*b4 + a4);
}
 
// Циклический сдвиг вправо на 4 бит 
BYTE TwoFish::ROR4(BYTE x){
    return (((x << 3) & 0xF) | ( (x & 0xF) >> 1));
}
 
// Циклический сдвиг влево
UINT TwoFish::ROL(UINT x, BYTE shift){
    BYTE shl = shift % (sizeof(x)*8);
    if (shl == 0) return x;
    return ((x << shl) | (x >> (sizeof(x)*8 - shl)));
}
 
// Циклический сдвиг вправо
UINT TwoFish::ROR(UINT x, BYTE shift){
    BYTE shl = shift % (sizeof(x)*8);
    if (shl == 0) return x;
    return ((x >> shl) | (x << (sizeof(x)*8 - shl)));
}