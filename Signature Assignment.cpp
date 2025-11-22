/*
===========================================================
I attest that this following code represents my own work and is subject to
the plagiarism policy found in the course syllabus.
Course: CSC 242
Assignment: Signature Assignment – Chapter 8 Streams
Group Members: Terrance McCall AND PRASCEDES TANDA
Date: NOV 20 2025
===========================================================
Description:
Solutions for Programming Projects P8.2 and P8.7
from Big C++: Late Objects, Enhanced.

- P8.2: Spell Checker
- P8.7: Random Monoalphabet Cipher

Pseudocode for each is included below. Inline comments explain logic.
===========================================================
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <cctype>

using namespace std;

/*
-----------------------------------------------------------
P8.2 Spell Checker
-----------------------------------------------------------
Pseudocode:
Algorithm SpellChecker
1. Open dictionary file.
2. Initialize container for dictionary words (lowercased).
3. For each word in dictionary file:
      Normalize to lowercase; store in set for fast lookup.
4. Open input file to be checked.
5. For each token in input file:
      Strip leading/trailing punctuation; lowercase.
      If nonempty and not found in dictionary:
          Print the word.
End Algorithm
-----------------------------------------------------------
*/

// Trim punctuation from both ends (keeps inner apostrophes/hyphens)
static string trimPunct(const string& s) {
    size_t start = 0, end = s.size();
    while (start < end && ispunct(static_cast<unsigned char>(s[start]))) start++;
    while (end > start && ispunct(static_cast<unsigned char>(s[end - 1]))) end--;
    return s.substr(start, end - start);
}

static string toLowerCopy(string s) {
    transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return s;
}
// I'm not sure why we're transforming letters for spell check; Unix library should include words with upper and lower case *Terrance
void spellChecker(const string& dictionaryFile, const string& inputFile) {
    ifstream dict(dictionaryFile);
    if (!dict) {
        cerr << "Error: Cannot open dictionary file: " << dictionaryFile << endl;
        return;
    }
// Is this where main would begin? *Terrance.
    unordered_set<string> dictionary; // lowercase words
    string w;
    while (dict >> w) {
        w = toLowerCopy(w);
        dictionary.insert(w);
    }
    dict.close();
      // You've opted to use an unordered set instead of a vector, though I'm not s

    ifstream in(inputFile);
    if (!in) {
        cerr << "Error: Cannot open input file: " << inputFile << endl;
        return;
    }

    cout << "Misspelled words:" << endl;
    while (in >> w) {
        string cleaned = trimPunct(w);
        cleaned = toLowerCopy(cleaned);
        if (!cleaned.empty() && dictionary.find(cleaned) == dictionary.end()) {
            cout << cleaned << endl;
        }
    }
    in.close();
}
// I see a spell check function, although it currently just initializes the dictionary, but no usage
// of the function in the code. *Terrance

/*
-----------------------------------------------------------
P8.7 Random Monoalphabet Cipher
-----------------------------------------------------------
Pseudocode:
Algorithm MonoalphabetCipher
Input: mode (-e for encrypt, -d for decrypt), keyword, inputFile, outputFile
1. Build cipher alphabet:
   a. Remove duplicate letters from keyword, preserve order (uppercase only).
   b. Append remaining letters A–Z in reverse order.
2. Make mappings:
   Plain A–Z → Cipher for encrypt; Cipher → Plain for decrypt.
3. Process file:
   For each character:
     - If letter: map using chosen direction, preserve original case.
     - Else: write unchanged.
4. Write to output file.
End Algorithm
-----------------------------------------------------------
*/

static string generateCipherAlphabet(const string& keywordRaw) {
    unordered_set<char> seen;
    string key;

    // Keep letters, uppercase, no duplicates
    for (char c : keywordRaw) {
        if (isalpha(static_cast<unsigned char>(c))) {
            char u = static_cast<char>(toupper(static_cast<unsigned char>(c)));
            if (!seen.count(u)) {
                seen.insert(u);
                key.push_back(u);
            }
        }
    }

    // Append remaining letters Z..A
    for (char c = 'Z'; c >= 'A'; --c) {
        if (!seen.count(c)) key.push_back(c);
    }

    // key length should be 26
    return key;
}

void cipherFile(const string& mode, const string& keyword,
    const string& inputFile, const string& outputFile) {
    if (keyword.empty()) {
        cerr << "Error: Keyword must be supplied with -k<keyword>." << endl;
        return;
    }
    string cipherAlphabet = generateCipherAlphabet(keyword);
    const string plainAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    unordered_map<char, char> enc, dec;
    for (size_t i = 0; i < plainAlphabet.size(); ++i) {
        enc[plainAlphabet[i]] = cipherAlphabet[i];
        dec[cipherAlphabet[i]] = plainAlphabet[i];
    }

    ifstream in(inputFile, ios::binary);
    if (!in) {
        cerr << "Error: Cannot open input file: " << inputFile << endl;
        return;
    }
    ofstream out(outputFile, ios::binary);
    if (!out) {
        cerr << "Error: Cannot open output file: " << outputFile << endl;
        return;
    }

    bool encrypting = (mode == "-e" || mode == "/e");
    bool decrypting = (mode == "-d" || mode == "/d");
    if (!encrypting && !decrypting) {
        cerr << "Error: Mode must be -e (encrypt) or -d (decrypt)." << endl;
        return;
    }

    char ch;
    while (in.get(ch)) {
        if (isalpha(static_cast<unsigned char>(ch))) {
            bool lower = islower(static_cast<unsigned char>(ch));
            char up = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
            char mapped = encrypting ? enc[up] : dec[up];
            if (lower) mapped = static_cast<char>(tolower(static_cast<unsigned char>(mapped)));
            out.put(mapped);
        }
        else {
            out.put(ch);
        }
    }
}

/*
-----------------------------------------------------------
Main: route to spell or crypt modes
-----------------------------------------------------------
Usage:
1) Spell Checker
   program.exe spell <dictionaryFile> <inputFile>

2) Cipher
   program.exe crypt <-e|-d> -k<keyword> <inputFile> <outputFile>
   Examples:
     program.exe crypt -e -kFEATHER plain.txt encrypted.txt
     program.exe crypt -d -kFEATHER encrypted.txt decrypted.txt
-----------------------------------------------------------
*/

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage:\n"
            << "  program.exe spell <dictionaryFile> <inputFile>\n"
            << "  program.exe crypt <-e|-d> -k<keyword> <inputFile> <outputFile>\n";
        return 1;
    }

    string cmd = argv[1];

    if (cmd == "spell") {
        if (argc != 4) {
            cerr << "Usage: program.exe spell <dictionaryFile> <inputFile>\n";
            return 1;
        }
        spellChecker(argv[2], argv[3]);
        return 0;
    }

    if (cmd == "crypt") {
        if (argc != 6) {
            cerr << "Usage: program.exe crypt <-e|-d> -k<keyword> <inputFile> <outputFile>\n";
            return 1;
        }
        string mode = argv[2];          // -e or -d
        string karg = argv[3];          // -kFEATHER
        string keyword;
        if (karg.rfind("-k", 0) == 0 || karg.rfind("/k", 0) == 0) {
            keyword = karg.substr(2);
        }
        else {
            cerr << "Error: Must supply keyword with -k<keyword>.\n";
            return 1;
        }
        cipherFile(mode, keyword, argv[4], argv[5]);
        return 0;
    }

    cerr << "Unknown command. Use 'spell' or 'crypt'.\n";
    return 1;
}


