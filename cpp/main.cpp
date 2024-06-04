#include <iostream>
#include <string>
#include <stdexcept>

#include "func.h" // Подключаем заголовочный файл с функциями шифрования
using namespace std;
const char* SYSTEM_CLEAR = "cls";

int main() {
    system("chcp 1251"); // Установка кодировки (1251 для кириллицы)
    
    string userpasssys, password = "12345", adminpass = "6789"; // пароль для входа 6789, для кодирование/декодирования 12345
    char choice;

    cout << "Enter the password to access the system: ";
    getline(cin, userpasssys);

    while (userpasssys != adminpass) {
        cout << "Incorrect password!" << endl;
        cout << "Enter the password to access the system: ";
        getline(cin, userpasssys);
    }

    system(SYSTEM_CLEAR);

    while (true) {
        try {
            system(SYSTEM_CLEAR);
            cout << "------------------MENU------------------" << endl;
            cout << "1. Polibiy Cipher" << endl;
            cout << "2. Atbash Cipher" << endl;
            cout << "3. RC6 Cipher" << endl;
            cout << "4. Exit" << endl;
            cout << "Enter the number of the cipher: ";
            cin >> choice;

            if (choice != '1' && choice != '2' && choice != '3' && choice != '4') {
                throw logic_error("This operation is not in the list");
            }

            if (choice == '4') {
                break;
            }

            switch (choice) {
                case '1':
                    system(SYSTEM_CLEAR);
                    polibiy(password);
                    break;

                case '2':
                    system(SYSTEM_CLEAR);
                    atbash(password); // Вызов функции для шифра Атбаш
                    break;

                case '3':
                    system(SYSTEM_CLEAR);
                    rc6(password);
                    break;
            }
        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(256, '\n');
        }
    }

    return 0;
}
