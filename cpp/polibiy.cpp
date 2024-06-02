#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include "func.h"

using namespace std;
extern const char* SYSTEM_CLEAR;

struct Matrix {
    map<char, pair<int, int>> matrix;
};

void Polybius_square(Matrix& Polybius) {
    int ch = 33;
    int ch2 = -64;

    for (int i = 1; i <= 9; i++) { // Заполняем квадрат Полибия, с помощью которого будем шифровать
        for (int j = 1; j <= 17; j++) {
            if (j <= 10) {
                Polybius.matrix[char(ch)].first = i;
                Polybius.matrix[char(ch)].second = j;
                ch++;
            } else {
                Polybius.matrix[char(ch2)].first = i;
                Polybius.matrix[char(ch2)].second = j;
                ch2++;
            }
        }
    }

    // Выводим квадрат Полибия
    cout << "Polybius Square:" << endl;
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 17; j++) {
            char symbol;
            if (j <= 10) {
                symbol = char(33 + (i - 1) * 10 + (j - 1));
            } else {
                symbol = char(-64 + (i - 1) * 7 + (j - 10));
            }
            cout << symbol << " ";
        }
        cout << endl;
    }
}


string Polybius_square_Code(string text) {
    Matrix Polybius;
    Polybius_square(Polybius);
    string ctf; // зашифрованное сообщение

    for (char element : text) {
        if ((element >= '!' && element <= 'z') || (element >= -64 && element <= -1)) {
            auto code = Polybius.matrix.find(element);
            if (code != Polybius.matrix.end()) {
                ctf += to_string(code->second.first) + to_string(code->second.second) + ' ';
            } else {
                // Если символ не найден в квадрате Полибия, добавляем пробел
                ctf += ' ';
            }
        } else if (element == '\n') {
            // Обрабатываем символ новой строки, добавляем его к зашифрованному тексту
            ctf += '\n';
        } else if (element == ' ') {
            // Символ пробела остается просто пробелом
            ctf += ' ';
        } else {
            // Любой другой символ, если не входит в диапазон от '!' до 'z' и от -64 до -1, игнорируется
            continue;
        }
    }
    return ctf;
}

string Polybius_square_Decode(string text) {
    char polybius[9][17];
    int ch = 33;
    int ch2 = -64;

    for (int i = 0; i < 9; i++) { // Заполняем квадрат Полибия, с помощью которого будем дешифровать
        for (int j = 0; j < 17; j++) {
            if (j < 10) {
                polybius[i][j] = char(ch);
                ch++;
            } else {
                polybius[i][j] = char(ch2);
                ch2++;
            }
        }
    }

    text += " ";
    string decoded;

    for (long i = 0; i < text.length(); i += 3) { // Поиск символа в квадрате Полибия
        if (text[i] == ' ') {
            decoded += ' ';
            i -= 2;
            continue;
        }
        if ((text[i + 2]) != ' ') {
            if (((text[i] - '0') * 100 + (text[i + 1] - '0') * 10 + (text[i + 2] - '0')) == 918) {
                decoded += char(-1);
                i++;
                continue;
            }
            int row = text[i] - '0' - 1;
            int col = (text[i + 1] - '0') * 10 + (text[i + 2] - '0') - 1;
            decoded += polybius[row][col];
            i++;
        } else {
            int row = text[i] - '0' - 1;
            int col = text[i + 1] - '0' - 1;
            decoded += polybius[row][col];
        }
    }
    return decoded;
}


void polibiy(string& password) {
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

                    writeToFile("polibiy.txt", text);
                    string encodedText2 = readFromFile("polibiy.txt");
                    string encoded2 = Polybius_square_Code(encodedText2);

                    cout << "Encrypted text: " << encoded2 << endl;
                    writeToFile("decoded.txt", encoded2);
                } else if (vvod == 2) {
                    cout << "Enter the file name: ";
                    cin >> fname;
                    cin.ignore(256, '\n');
                    string encodedText1 = readFromFile(fname);
                    string encoded1 = Polybius_square_Code(encodedText1);

                    cout << "Encrypted text: " << encoded1 << endl;
                    writeToFile("decoded.txt", encoded1);
                }
            } else if (oper == 2) {
                cin.ignore(256, '\n');
                cout << "Enter the password: ";
                cin >> userpass;
                if (userpass != password) {
                    throw logic_error("Incorrect password");
                }
                string encodedText1 = readFromFile("decoded.txt");
                string decoded1 = Polybius_square_Decode(encodedText1);
                writeToFile("decoded1.txt", decoded1);

                cout << "The decrypted text: " << decoded1 << endl;
            }
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(256, '\n');
        }
    }
}
