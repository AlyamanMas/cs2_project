#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

int brute_force_search(const string &text, const string &pattern) {
    int n = text.length();
    int m = pattern.length();

    for (int i = 0; i <= n - m; i++) {
        int j = 0;
        while (j < m && pattern[j] == text[i + j]) {
            j++;
        }
        if (j == m) {
            return i; // match found at index i
        }
    }
    return -1; // no match found
}

int rabin_karp_search(string &text, string &pattern) {
    int n = text.length();
    int m = pattern.length();
    int d = 256; // number of characters in the input alphabet
    int q = 101; // a prime number

    int h = 1;
    for (int i = 0; i < m - 1; i++) {
        h = (h * d) % q;
    }

    int p = 0; // hash value for pattern
    int t = 0; // hash value for text
    for (int i = 0; i < m; i++) {
        p = (d * p + pattern[i]) % q;
        t = (d * t + text[i]) % q;
    }

    for (int i = 0; i <= n - m; i++) {
        if (p == t) {
            int j = 0;
            while (j < m && pattern[j] == text[i + j]) {
                j++;
            }
            if (j == m) {
                return i; // match found at index i
            }
        }
        if (i < n - m) {
            t = (d * (t - text[i] * h) + text[i + m]) % q;
            if (t < 0) {
                t = t + q;
            }
        }
    }
    return -1; // no match found
}

// Iterator for corpus_full_path from files in folder 'corpus'
map<string, string> return_files(string& corpus_full_path) {
    map<string, string> files;
    auto files_iterator = filesystem::directory_iterator(corpus_full_path);

    for (const auto &entry: files_iterator) {
        ifstream file(entry.path());
        string file_string;
        string line;
        while (getline(file, line)) {
            file_string.append(line);
        }
        files[entry.path().filename().string()] = file_string;
    }
    return files;

//    vector<string> files;
//    namespace fs = filesystem;
//    auto files_iterator = fs::directory_iterator(corpus_full_path);
//    vector<string> files;
//
//    for (const auto &entry: files_iterator) {
//        ifstream file(entry.path());
//        string file_string;
//        string line;
//        while (getline(file, line)) {
//            file_string.append(line);
//        }
//        files.push_back(file_string);
//    }
//    return files;
}

int main() {
    string text = "Hello World! How are you?";
    string pattern = "How are yo";
    string corpus_full_path = "/home/yaman/CLionProjects/cs2-project/corpus";

    map<string, string> files = return_files(corpus_full_path);
    int index = rabin_karp_search(text, pattern);

    if (index != -1) {
        cout << "Pattern found at index: " << index << endl;
    } else {
        cout << "Pattern not found in the text." << endl;
    }
    return 0;
}
