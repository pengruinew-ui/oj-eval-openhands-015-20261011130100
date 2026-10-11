#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cstdio>

using namespace std;

class FileStorage {
private:
    string dataFile = "storage.txt";
    
public:
    FileStorage() {
        // Check if file exists, create if not
        ifstream check(dataFile);
        if (!check.good()) {
            ofstream create(dataFile);
            create.close();
        }
    }
    
    void insert(const string& index, int value) {
        // Check if record already exists
        if (findRecord(index, value)) {
            return; // Don't insert duplicate
        }
        
        // Append new record in format: index value active
        ofstream file(dataFile, ios::app);
        if (!file) return;
        
        file << index << " " << value << " 1" << "\n";
        file.close();
    }
    
    void deleteRecord(const string& index, int value) {
        // Use a temporary file to avoid loading everything into memory
        string tempFile = dataFile + ".tmp";
        ifstream inFile(dataFile);
        ofstream outFile(tempFile);
        string line;
        bool found = false;
        
        while (getline(inFile, line)) {
            istringstream iss(line);
            string fileIndex;
            int fileValue;
            int active;
            
            if (iss >> fileIndex >> fileValue >> active) {
                if (fileIndex == index && fileValue == value && active == 1 && !found) {
                    // Mark as deleted (only first occurrence)
                    outFile << fileIndex << " " << fileValue << " 0" << "\n";
                    found = true;
                } else {
                    outFile << line << "\n";
                }
            }
        }
        
        inFile.close();
        outFile.close();
        
        // Replace original file with temp file
        remove(dataFile.c_str());
        rename(tempFile.c_str(), dataFile.c_str());
    }
    
    vector<int> find(const string& index) {
        vector<int> results;
        ifstream file(dataFile);
        string line;
        
        while (getline(file, line)) {
            istringstream iss(line);
            string fileIndex;
            int value;
            int active;
            
            if (iss >> fileIndex >> value >> active) {
                if (fileIndex == index && active == 1) {
                    results.push_back(value);
                }
            }
        }
        
        file.close();
        
        // Sort results in ascending order
        sort(results.begin(), results.end());
        return results;
    }
    
    // More efficient insert that doesn't check for duplicates first
    // to avoid scanning the entire file
    void insertFast(const string& index, int value) {
        ofstream file(dataFile, ios::app);
        if (!file) return;
        
        file << index << " " << value << " 1" << "\n";
        file.close();
    }
    
private:
    bool findRecord(const string& index, int value) {
        ifstream file(dataFile);
        string line;
        
        while (getline(file, line)) {
            istringstream iss(line);
            string fileIndex;
            int fileValue;
            int active;
            
            if (iss >> fileIndex >> fileValue >> active) {
                if (fileIndex == index && fileValue == value && active == 1) {
                    file.close();
                    return true;
                }
            }
        }
        
        file.close();
        return false;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    FileStorage storage;
    
    int n;
    if (!(cin >> n)) return 0;
    cin.ignore(); // Ignore newline after n
    
    for (int i = 0; i < n; i++) {
        string line;
        if (!getline(cin, line)) break;
        
        // Trim leading/trailing whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == string::npos) continue;
        size_t end = line.find_last_not_of(" \t\r\n");
        line = line.substr(start, end - start + 1);
        
        if (line.empty()) continue;
        
        size_t space1 = line.find(' ');
        if (space1 == string::npos) continue;
        
        string command = line.substr(0, space1);
        string rest = line.substr(space1 + 1);
        
        // Trim rest
        start = rest.find_first_not_of(" \t");
        if (start != string::npos) {
            rest = rest.substr(start);
        }
        
        if (command == "insert") {
            size_t space2 = rest.find(' ');
            if (space2 == string::npos) continue;
            
            string index = rest.substr(0, space2);
            string valueStr = rest.substr(space2 + 1);
            
            // Trim value string
            start = valueStr.find_first_not_of(" \t");
            if (start != string::npos) {
                valueStr = valueStr.substr(start);
            }
            
            try {
                int value = stoi(valueStr);
                storage.insertFast(index, value);
            } catch (...) {
                continue;
            }
            
        } else if (command == "delete") {
            size_t space2 = rest.find(' ');
            if (space2 == string::npos) continue;
            
            string index = rest.substr(0, space2);
            string valueStr = rest.substr(space2 + 1);
            
            // Trim value string
            start = valueStr.find_first_not_of(" \t");
            if (start != string::npos) {
                valueStr = valueStr.substr(start);
            }
            
            try {
                int value = stoi(valueStr);
                storage.deleteRecord(index, value);
            } catch (...) {
                continue;
            }
            
        } else if (command == "find") {
            // Trim index
            start = rest.find_first_not_of(" \t");
            if (start != string::npos) {
                rest = rest.substr(start);
            }
            
            string index = rest;
            vector<int> results = storage.find(index);
            
            if (results.empty()) {
                cout << "null";
            } else {
                for (size_t j = 0; j < results.size(); j++) {
                    if (j > 0) cout << " ";
                    cout << results[j];
                }
            }
            cout << "\n";
        }
    }
    
    return 0;
}