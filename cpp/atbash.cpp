#include <iostream>
#include <string>
#include <fstream>
#include "func.h"

using namespace std;

extern const char* SYSTEM_CLEAR;

// Функция для шифрования текста методом Атбаш
string encode(string& text) {
    string result;
    for (int i = 0; i < text.size(); i++) {
        unsigned char ch = static_cast<unsigned char>(text[i]);

        if (ch >= 97 && ch <= 122) // a-z
            result += char(122 - ch + 97);
        else if (ch >= 65 && ch <= 90) // A-Z
            result += char(90 - ch + 65);
        else if (ch >= 128 && ch <= 175) // А-Я (кириллица заглавные)
            result += char(175 - ch + 128);
        else if (ch >= 224 && ch <= 255) // а-я (кириллица строчные)
            result += char(255 - ch + 224);
        else if (ch >= 176 && ch <= 223) // псевдографика и другие символы
            result += char(223 - ch + 176);
        else if (ch >= 33 && ch <= 64) // !-@
            result += char(64 - ch + 33);
        else if (ch >= 91 && ch <= 96) // [-`
            result += char(96 - ch + 91);
        else if (ch >= 123 && ch <= 126) // {-~
            result += char(126 - ch + 123);
        else
            result += ch;
    }
    return result;
}

// Функция для чтения содержимого файла и вывода на экран
void printFileContent(const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    } else {
        cerr << "Unable to open file " << filename << endl;
    }
}

// Основная функция для работы с консолью
void atbash(string& password) {
    string text, key, userpass, fname;
    int oper, vvod;

    while (true) {
        try {
            cout << "Select an operation: 1 - encryption, 2 - decryption, 3 - clear console, 0 - exit" << endl;
            cout << "Enter the operation number: ";
            cin >> oper;
            if (cin.peek() != '\n' || cin.fail()) {
                throw logic_error("You entered a string");
                cout << "Enter the operation number: ";
                cin >> oper;
            }
            if (oper == 0) {
                break;
            } else if (oper == 3) {
                system(SYSTEM_CLEAR);
            } else if (oper != 1 && oper != 2) {
                throw logic_error("This operation is not in the list!");
            } else if (oper == 1) {
                cout << "Enter the password: ";
                cin >> userpass;
                if (userpass != password) {
                    throw logic_error("Incorrect password");
                }
                cout << "Select input: 1 - from console, 2 - from file" << endl;
                cout << "Enter the operation number: ";
                cin >> vvod;
                cin.ignore(256, '\n');
                if (vvod == 1) {
                    cout << "Enter the text: ";
                    getline(cin, text);

                    writeToFile("encrypted.txt", text);
                    string encodedText = readFromFile("encrypted.txt");
                    string encoded = encode(encodedText);
                    writeToFile("decodedfile.txt", encoded);
                    cout << "Encrypted text has been written to decodedfile.txt" << endl;

                    // Display the contents of the encoded file
                    printFileContent("decodedfile.txt");
                } else if (vvod == 2) {
                    cout << "Enter the file name: ";
                    cin >> fname;
                    cin.ignore(256, '\n');
                    string encodedText = readFromFile(fname);
                    string encoded = encode(encodedText);
                    writeToFile("decodedfile.txt", encoded);
                    cout << "Encrypted text has been written to decodedfile.txt" << endl;

                    // Display the contents of the encoded file
                    printFileContent("decodedfile.txt");
                }
            } else if (oper == 2) {
                cin.ignore(256, '\n');
                cout << "Enter the password: ";
                cin >> userpass;
                if (userpass != password) {
                    throw logic_error("Incorrect password");
                }
                string encodedText = readFromFile("decodedfile.txt");
                string decoded = encode(encodedText);
                writeToFile("decrypted.txt", decoded);
                cout << "Decrypted text has been written to decrypted.txt" << endl;

                // Display the contents of the decoded file
                printFileContent("decrypted.txt");
            }
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(256, '\n');
        }
    }
}
