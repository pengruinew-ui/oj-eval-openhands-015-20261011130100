#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdint>

using namespace std;

class FileStorage {
private:
    string dataFile = "storage.dat";
    
    struct Record {
        string index;
        int value;
        bool deleted;
        
        Record() : value(0), deleted(false) {}
        Record(const string& idx, int val) : index(idx), value(val), deleted(false) {}
    };
    
public:
    FileStorage() {
        // Check if file exists, create if not
        ifstream check(dataFile);
        if (!check.good()) {
            ofstream create(dataFile, ios::binary);
            create.close();
        }
    }
    
    void insert(const string& index, int value) {
        // Check if record already exists
        if (findRecord(index, value)) {
            return; // Don't insert duplicate
        }
        
        // Append new record
        ofstream file(dataFile, ios::binary | ios::app);
        if (!file) return;
        
        // Write record format: [index_len][index][value][deleted_flag]
        uint8_t indexLen = index.length();
        file.write(reinterpret_cast<const char*>(&indexLen), 1);
        file.write(index.c_str(), indexLen);
        file.write(reinterpret_cast<const char*>(&value), sizeof(value));
        bool deleted = false;
        file.write(reinterpret_cast<const char*>(&deleted), sizeof(deleted));
        
        file.close();
    }
    
    void deleteRecord(const string& index, int value) {
        fstream file(dataFile, ios::binary | ios::in | ios::out);
        if (!file) return;
        
        streampos pos = 0;
        while (file) {
            uint8_t indexLen;
            file.read(reinterpret_cast<char*>(&indexLen), 1);
            if (file.eof()) break;
            
            char indexBuf[65];
            file.read(indexBuf, indexLen);
            if (file.eof()) break;
            string currentIndex(indexBuf, indexLen);
            
            int currentValue;
            file.read(reinterpret_cast<char*>(&currentValue), sizeof(currentValue));
            if (file.eof()) break;
            
            bool deleted;
            file.read(reinterpret_cast<char*>(&deleted), sizeof(deleted));
            if (file.eof()) break;
            
            if (!deleted && currentIndex == index && currentValue == value) {
                // Mark as deleted
                file.seekp(static_cast<streampos>(pos) + 1 + indexLen + sizeof(currentValue));
                deleted = true;
                file.write(reinterpret_cast<const char*>(&deleted), sizeof(deleted));
                break;
            }
            
            pos = file.tellg();
        }
        
        file.close();
    }
    
    vector<int> find(const string& index) {
        vector<int> results;
        ifstream file(dataFile, ios::binary);
        if (!file) return results;
        
        while (file) {
            uint8_t indexLen;
            file.read(reinterpret_cast<char*>(&indexLen), 1);
            if (file.eof()) break;
            
            char indexBuf[65];
            file.read(indexBuf, indexLen);
            if (file.eof()) break;
            string currentIndex(indexBuf, indexLen);
            
            int value;
            file.read(reinterpret_cast<char*>(&value), sizeof(value));
            if (file.eof()) break;
            
            bool deleted;
            file.read(reinterpret_cast<char*>(&deleted), sizeof(deleted));
            if (file.eof()) break;
            
            if (!deleted && currentIndex == index) {
                results.push_back(value);
            }
        }
        
        file.close();
        
        // Sort results in ascending order
        sort(results.begin(), results.end());
        return results;
    }
    
private:
    bool findRecord(const string& index, int value) {
        ifstream file(dataFile, ios::binary);
        if (!file) return false;
        
        while (file) {
            uint8_t indexLen;
            file.read(reinterpret_cast<char*>(&indexLen), 1);
            if (file.eof()) break;
            
            char indexBuf[65];
            file.read(indexBuf, indexLen);
            if (file.eof()) break;
            string currentIndex(indexBuf, indexLen);
            
            int currentValue;
            file.read(reinterpret_cast<char*>(&currentValue), sizeof(currentValue));
            if (file.eof()) break;
            
            bool deleted;
            file.read(reinterpret_cast<char*>(&deleted), sizeof(deleted));
            if (file.eof()) break;
            
            if (!deleted && currentIndex == index && currentValue == value) {
                file.close();
                return true;
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
    cin >> n;
    cin.ignore(); // Ignore newline after n
    
    for (int i = 0; i < n; i++) {
        string line;
        getline(cin, line);
        
        size_t space1 = line.find(' ');
        if (space1 == string::npos) continue;
        
        string command = line.substr(0, space1);
        string rest = line.substr(space1 + 1);
        
        if (command == "insert") {
            size_t space2 = rest.find(' ');
            if (space2 == string::npos) continue;
            
            string index = rest.substr(0, space2);
            int value = stoi(rest.substr(space2 + 1));
            storage.insert(index, value);
            
        } else if (command == "delete") {
            size_t space2 = rest.find(' ');
            if (space2 == string::npos) continue;
            
            string index = rest.substr(0, space2);
            int value = stoi(rest.substr(space2 + 1));
            storage.deleteRecord(index, value);
            
        } else if (command == "find") {
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