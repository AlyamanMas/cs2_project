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
map<string, string> return_files(string &corpus_full_path) {
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
}

vector<string> split_paragraph_into_sentences(string paragraph) {
    vector<string> sentences;
    string sentence;
    for (char i: paragraph) {
        if (i == '.' || i == '?' || i == '!' || i == '\n') {
            sentence += i;
            sentences.push_back(sentence);
            sentence = "";
        } else {
            sentence += i;
        }
    }
    return sentences;
}

string read_pattern_file(string &test_file_path) {
    ifstream file(test_file_path);
    string pattern_string;
    string line;
    while (getline(file, line)) {
        pattern_string.append(line);
    }
    return pattern_string;
}

int main() {
    string corpus_full_path = "/home/yaman/CLionProjects/cs2-project/corpus";
    string test_file_full_path = "/home/yaman/CLionProjects/cs2-project/test.txt";
    vector<string> pattern_sentences_v = split_paragraph_into_sentences(read_pattern_file(test_file_full_path));
    // map<filename, file_content_string> from corpus
    map<string, string> files_map = return_files(corpus_full_path);

    for (auto pattern_sentence: pattern_sentences_v) {
        bool found = false;
        for (auto file: files_map) {
            // calculate time for searching
            int index = rabin_karp_search(file.second, pattern_sentence);
            int index_brute = brute_force_search(file.second, pattern_sentence);
            if (index != -1) {
                cout << "Sentence: " << pattern_sentence << "\nwas found at index " << index << " in file: "
                     << file.first << "\n\n";
//                cout << "Sentence: " << pattern_sentence << "\nwas found at index " << index_brute << " in file: "
//                     << file.first << "\n\n";
                found = true;
            }
        }
        if (!found) {
            cout << "Pattern \"" << pattern_sentence << "\" not found in any file" << endl;
        }
    }
    return 0;
}
