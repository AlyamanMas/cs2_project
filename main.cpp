#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <map>
#include <chrono>

using namespace std;

#define RUN 6

typedef struct {
    string file;
    string sentence;
    long time_rabin_karp;
    long time_brute_force;
    bool found;
} Record;

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

// NOTE: tsv was chosen instead of csv since many of the files in the corpus contain commas, which would make parsing
// the csv file more difficult
void write_to_tsv(vector<Record> &results, string test_file_path) {
    ofstream tsv_file;
    tsv_file.open(test_file_path + "-" + to_string(RUN) + ".tsv");
    tsv_file << "file\tsentence\ttime_rabin_karp\ttime_brute_force\tfound\n";
    for (auto result: results) {
        tsv_file << result.file << "\t" << result.sentence << "\t" << result.time_rabin_karp << "\t"
                 << result.time_brute_force << "\t" << result.found << "\n";
    }
    tsv_file.close();
}

int main() {
    string corpus_full_path = "/home/yaman/CLionProjects/cs2-project/corpus";
    string test_file_full_path = "/home/yaman/CLionProjects/cs2-project/test.txt";
    vector<string> pattern_sentences_v = split_paragraph_into_sentences(
            read_pattern_file(test_file_full_path)
    );
    // map<filename, file_content_string> from corpus
    map<string, string> files_map = return_files(corpus_full_path);
    // vector<tuple<file, sentence, time_rabin_karp, time_brute_force, found>>
    vector<Record> results;

    for (auto pattern_sentence: pattern_sentences_v) {
        bool found = false;
        for (auto file: files_map) {
            // time rabin karp in nano seconds
            auto time_rabin_karp = chrono::high_resolution_clock::now();
            int index = rabin_karp_search(file.second, pattern_sentence);
            auto time_rabin_karp_end = chrono::high_resolution_clock::now();
            auto time_rabin_karp_duration = chrono::duration_cast<chrono::nanoseconds>(
                    time_rabin_karp_end - time_rabin_karp).count();

            // time brute force in nano seconds
            auto time_brute_force = chrono::high_resolution_clock::now();
            int index_brute = brute_force_search(file.second, pattern_sentence);
            auto time_brute_force_end = chrono::high_resolution_clock::now();
            auto time_brute_force_duration = chrono::duration_cast<chrono::nanoseconds>(
                    time_brute_force_end - time_brute_force).count();

            if (index != -1) {
                cout << "Sentence: " << pattern_sentence << "\nwas found at index " << index << " in file: "
                     << file.first << "\n\n";
                found = true;
                results.push_back(
                        {file.first, // file
                         pattern_sentence, // sentence
                         time_rabin_karp_duration, // time_rabin_karp
                         time_brute_force_duration, // time_brute_force
                         true} // found
                );
            } else {
                results.push_back(
                        {file.first, // file
                         pattern_sentence, // sentence
                         time_rabin_karp_duration, // time_rabin_karp
                         time_brute_force_duration, // time_brute_force
                         false} // found
                );
            }
        }
        if (!found) {
            cout << "Pattern \"" << pattern_sentence << "\" not found in any file" << endl;
        }
    }

    write_to_tsv(results, test_file_full_path);
    return 0;
}
