#include <iostream>
#include <cstdint>
#include <cmath>
#include <iomanip>
#include <random>
#include <fstream>
#include "func.h"

using namespace std;
extern const char* SYSTEM_CLEAR;

// Константы
const uint32_t P = 0xb7e15163; //odd((e - 2)*2^w) - округление до большего, e - экспонента, f - золотое сечение 
const uint32_t Q = 0x9e3779b9; //odd((f - 1)*2^w) для w = 32
const int w = 32;
const int r = 20; // Количество раундов, Key - размер ключа 128 бита

// Этап 1: Конвертация секретного ключа
void KeyExpansion(uint8_t* K, int b, uint32_t* L, int& c) {
    c = (b + (w / 8 - 1)) / (w / 8);
    for (int i = b - 1; i >= 0; i--) {
        L[i / (w / 8)] = (L[i / (w / 8)] << 8) + K[i];
    }
    if (b % (w / 8) != 0) {
        for (int i = b; i < c * (w / 8); i++) {
            L[i / (w / 8)] <<= 8;
        }
    }
}

// Этап 2: Инициализация массива ключей
void InitializeS(uint32_t* S, int r) {
    S[0] = P;
    for (int i = 1; i <= 2 * r + 3; i++) {
        S[i] = S[i - 1] + Q;
    }
}

// Этап 3: Перемешивание
void MixKeys(uint32_t* L, int c, uint32_t* S, int r) {
    uint32_t A = 0, B = 0, i = 0, j = 0;
    int v = 3 * max(c, 2 * r + 4);
    for (int s = 1; s <= v; s++) {
        A = S[i] = (S[i] + A + B) << 3;
        B = L[j] = (L[j] + A + B) << (A + B);
        i = (i + 1) % (2 * r + 4);
        j = (j + 1) % c;
    }
}

// Шифрование
void Encrypt(uint32_t& A, uint32_t& B, uint32_t& C, uint32_t& D, uint32_t* S) {
    B = B + S[0];
    D = D + S[1];
    for (int i = 1; i <= r; i++) {
        uint32_t t = (B * (2 * B + 1)) << (int)log2(w);
        uint32_t u = (D * (2 * D + 1)) << (int)log2(w);
        A = ((A ^ t) << u) + S[2 * i];
        C = ((C ^ u) << t) + S[2 * i + 1];
        (A, B, C, D) = (B, C, D, A);
    }
    A = A + S[2 * r + 2];
    C = C + S[2 * r + 3];
}

//подготовка к шифрованию
vector<vector <uint32_t>> rc6_encrypt(string& encodedText2, uint8_t Key[16]){
    wstring plaintext(encodedText2.begin(), encodedText2.end());
    vector<vector <uint32_t>> encrypt;

    // Генерация случайного ключа
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255);

    for (int i = 0; i < 16; i++) {
        Key[i] = static_cast<uint8_t>(dis(gen));
    }

    // Запись ключа в файл
    ofstream keyFile("key.txt");
    if (keyFile.is_open()) {
        for (int i = 0; i < 16; i++) {
            keyFile << hex << setw(2) << setfill('0') << static_cast<int>(Key[i]);
            if (i < 15) {
                keyFile << " ";
            }
        }
        keyFile.close();
        cout << "Key saved in key.txt" << endl;
    } else {
        cout << "Unable to open key.txt" << endl;
    }

    const int b = 16;
    uint32_t L[b / (w / 8)];
    int c;
    KeyExpansion(Key, b, L, c);

    uint32_t S[2 * r + 4];
    InitializeS(S, r);
    MixKeys(L, c, S, r);

    cout << "Encrypted: ";
    for (wchar_t c : plaintext) {
        uint32_t A = c, B = 0, C = 0, D = 0;
        Encrypt(A, B, C, D, S);
        encrypt.push_back({A, B, C, D});
        cout << hex << setw(8) << setfill('0') << A << " " << setw(8) << setfill('0') << B << " " << setw(8) << setfill('0') << C << " " << setw(8) << setfill('0') << D << endl;
    }
    return encrypt;
}

//шифрование
void Decrypt(uint32_t& A, uint32_t& B, uint32_t& C, uint32_t& D, uint32_t* S) {
    C = C - S[2 * r + 3];
    A = A - S[2 * r + 2];
    for (int i = r; i >= 1; i--) {
        (A, B, C, D) = (D, A, B, C);
        uint32_t u = (D * (2 * D + 1)) << (int)log2(w);
        uint32_t t = (B * (2 * B + 1)) << (int)log2(w);
        C = ((C - S[2 * i + 1]) >> t) ^ u;
        A = ((A - S[2 * i]) >> u) ^ t;
    }
    D = D - S[1];
    B = B - S[0];
}

//подготовка к дешифровки
string rc6_decrypt(vector<vector <uint32_t>>& encodedText1, uint8_t Key[16]){
    const int b = 16;
    uint32_t L[b / (w / 8)];
    int c;
    KeyExpansion(Key, b, L, c);

    uint32_t S[2 * r + 4];
    InitializeS(S, r);
    MixKeys(L, c, S, r);
    
    string decryptedText;
    for (auto elementMas: encodedText1){
        uint32_t A = elementMas[0], B = elementMas[1], C = elementMas[2], D = elementMas[3];
        Decrypt(A, B, C, D, S);
        decryptedText += static_cast<char>(A);
    }
    return decryptedText;
}

void rc6(string& password){
    
    int pick, pickencode;
    string pass1, pass2, message, filename;
    uint8_t Key[16];
    vector<vector <uint32_t>> encoded;

    while (true) {
        try {
           cout << "Select an operation: 1 - encryption, 2 - decryption, 3 - clear console, 0 - exit" << endl;
            cout << "Enter the operation number: ";
            cin >> pick;
            while (cin.fail() || cin.peek()!='\n') {
                throw logic_error("Input error!");
                cin >> pick;
            }
            if (pick!= 1 && pick != 2 && pick != 3 && pick != 0) {
                throw logic_error("You entered the wrong command!");
            }

            if (pick == 1) {
                cout << "Enter the password: ";
                cin >> pass1;
                if (pass1 == password) {
                   cout << "Select input: 1 - from console, 2 - from file" << endl;
                cout << "Enter the operation number: ";
                    cin >> pickencode;
                    if (cin.fail() || cin.peek() != '\n') {
                        throw logic_error("Input error!");
                    }
                    if (pickencode != 1 && pickencode != 2) {
                        throw logic_error("You entered the wrong command!");
                    }
                    if (pickencode == 1) {
                        cout << "Input text: ";
                        cin.ignore();
                        getline(cin, message);
                        
                        writeToFile("rc6.txt", message);
                        string encodedText = readFromFile("rc6.txt");

                        encoded = rc6_encrypt(encodedText, Key);
                        string str;
                        for (auto i : encoded){
                            for (auto element: i){
                                str += to_string(element) + ' ';
                            }
                            str += '\n';
                        }
                        writeToFile("decoded.txt", str);
                    }
                    if (pickencode == 2) {
                        cout << "Enter the name of the file with the extension: ";
                        cin >> filename;
                        string encodedText = readFromFile(filename);

                        encoded = rc6_encrypt(encodedText, Key);
                        string str;
                        for (auto i : encoded){
                            for (auto element: i){
                                str += to_string(element) + ' ';
                            }
                            str += '\n';
                        }
                        writeToFile("decoded.txt", str);
                    }
                } else {
                    throw logic_error("Invalid encoding password");
                }

            } else if (pick == 2) {
                cout << "Enter the decoding password: ";
                cin >> pass2;

                if (pass2 == password) {
                    // Чтение ключа из файла
                    ifstream keyFile("key.txt");
                    if (keyFile.is_open()) {
                        string keyStr;
                        getline(keyFile, keyStr);
                        istringstream iss(keyStr);
                        int i = 0;
                        while (iss && i < 16) {
                            char byte[3];
                            iss >> byte;
                            Key[i++] = static_cast<uint8_t>(strtol(byte, nullptr, 16));
                        }
                        keyFile.close();
                    } else {
                        cout << "Не удалось открыть файл key.txt" << endl;
                        return;
                    }

                    vector<vector <uint32_t>> encodedText1 = encoded;
                    string decoded1 = rc6_decrypt(encodedText1, Key);
                    writeToFile("decoded1.txt", decoded1);

                    cout << "The decrypted text: " << decoded1 << endl;
                } else {
                    throw logic_error("Invalid decoding password");
                }
                
            } else if (pick == 3) {
                system(SYSTEM_CLEAR);
            } else if (pick == 0 ){
                system(SYSTEM_CLEAR);
                break;
            }
        }
        catch (const exception& e) {
            cerr << e.what() << endl;
            cin.clear();
            cin.ignore(256, '\n');
        }
    }
    return;
}
