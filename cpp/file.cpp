#include <iostream>
#include <string>
#include <fstream>
#include "func.h"
using namespace std;
void writeToFile(const string& filename, const string& text) {
    ofstream file(filename);
    if (file.is_open()) {
        file << text;
        file.close();
    }
    else {
        throw runtime_error("Unable to write to file: " + filename);
    }
}

string readFromFile(const string& filename) {
    ifstream file(filename);
    string content;
    if (file.is_open()) {
        string line;
        while (getline(file, line))
        {
            content += line + "\n";
        }
        file.close();
        return content;
    }
    else {
        throw runtime_error("Unable to read from file: " + filename);
    }
}