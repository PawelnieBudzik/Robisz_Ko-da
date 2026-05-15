#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class Node {
public:
  char ch; // litera
  int frequence;
  Node *left;
  Node *right;

  Node(char ch, int frequence)
      : ch(ch), frequence(frequence), left(nullptr), right(nullptr) {};

  Node(char ch, int frequence, Node *left, Node *right)
      : ch(ch), frequence(frequence), left(left), right(right) {};
};

struct compare {
  bool operator()(Node *l, Node *r) { return l->frequence > r->frequence; }
};

string loadFromFile() {
  std::ifstream file("input.txt");
  string line = "";

  if (file.is_open()) {
    getline(file, line);

    file.close();
  } else {
    cout << "Unable to open file" << endl;
    return "";
  }

  return line;
}

void generateCodes(Node *root, string str,
                   unordered_map<char, string> &huffmanCode) {

  if (root == nullptr) {

    return;
  }

  if (!root->left && !root->right) {
    huffmanCode[root->ch] = str;
  }

  generateCodes(root->left, str + "0", huffmanCode);
  generateCodes(root->right, str + "1", huffmanCode);
}

string decode(string text, Node *root) {
  string value = "";
  Node *curr = root;
  for (char bit : text) {
    if (bit == '0') {
      curr = curr->left;
    } else {
      curr = curr->right;
    }

    if (!curr->left && !curr->right) {
      value += curr->ch;
      curr = root;
    }
  }
  return value;
}

string encode(string text, unordered_map<char, string> huffmanCode) {
  string hashedStr = "";
  for (char ch : text) {
    hashedStr += huffmanCode[ch];
  }

  return hashedStr;
};

/*INFO:
 * Rozmiar przed kompresją - Sb
 * Rozmiar po kompresji - Sa
 * Czestosc znaku - fs
 * dlugosc kodu binarnego - cl
 * laczna liczba znakow - cc
 */

vector<string>
CalculateMetrics(const string &text, const string &hashed,
                 const unordered_map<char, string> &huffmanCode) {
  int Sb = text.size() * 8;
  int Sa = hashed.size();

  vector<string> wynik;

  double wskaznik_komp = (double)Sb / Sa;
  wynik.push_back(to_string(wskaznik_komp));

  double stopien_komp = (1.0 - (double)Sa / Sb) * 100.0;
  wynik.push_back(to_string(stopien_komp));

  int cc = text.size();
  double srednia_dlugosc = 0.0;

  for (const auto &pair : huffmanCode) {
    char ch = pair.first;
    int code_length = pair.second.size();

    int count = 0;
    for (char c : text) {
      if (c == ch)
        count++;
    }

    double probability = (double)count / cc;
    srednia_dlugosc += probability * code_length;
  }
  wynik.push_back(to_string(srednia_dlugosc));

  return wynik;
}

string formatSecondDecimal(const double value) {
  ostringstream oss;

  oss << fixed << setprecision(2) << value;

  return oss.str();
}

void saveToOutputFile(const string &text, const string &hashed,
                      const string &decoded, const long long &encode_time,
                      const long long &decode_time,
                      const unordered_map<char, string> &huffmanCode) {
  vector<string> metrics = CalculateMetrics(text, hashed, huffmanCode);

  string stop_komp = formatSecondDecimal(stod(metrics[1]));

  string sred_dlu = formatSecondDecimal(stod(metrics[2]));

  std::ofstream file("output.txt");
  if (file.is_open()) {

    string message = "Tekst wejsciowy: \n" + text + "\n\nZakodowany tekst: \n" +
                     hashed + "\n\nTekst po dekodowaniu: \n" + decoded +
                     "\n\nWskaźnik kompresji: " + metrics[0] +
                     "\nStopien kompresji: " + stop_komp + "%" +
                     "\nŚrednia długość kodu: " + sred_dlu +
                     " bitów na znak\n" +
                     "\nCzas kodowania: " + to_string(encode_time) + " ns" +
                     "\nCzas dekodowania: " + to_string(decode_time) + " ns";

    file << message;

    file.close();
  } else {
    cout << "Nie udało sie utworzyć pliku" << endl;
  }
}

void buildTree(string text) {
  unordered_map<char, int> frequency;

  for (char ch : text) {
    frequency[ch]++;
  }

  if (frequency.size() < 2) {
    cout << "Za malo unikalnych znakow" << endl;
    return;
  }

  priority_queue<Node *, vector<Node *>, compare> priority_q;

  for (auto pair : frequency) {
    priority_q.push(new Node(pair.first, pair.second));
  }

  while (priority_q.size() != 1) {
    Node *left = priority_q.top();
    priority_q.pop();

    Node *right = priority_q.top();
    priority_q.pop();

    int sum = left->frequence + right->frequence;
    priority_q.push(new Node('\0', sum, left, right));
  }

  Node *root = priority_q.top();

  unordered_map<char, string> huffmanCode;
  generateCodes(root, "", huffmanCode);

  auto start_encode = chrono::high_resolution_clock::now();
  string encoded = encode(text, huffmanCode);
  auto end_encode = chrono::high_resolution_clock::now();

  long long czas_kodowania =
      chrono::duration_cast<chrono::nanoseconds>(end_encode - start_encode)
          .count();

  auto start_decode = chrono::high_resolution_clock::now();
  string decoded = decode(encoded, root);
  auto end_decode = chrono::high_resolution_clock::now();

  long long czas_dekodowania =
      chrono::duration_cast<chrono::nanoseconds>(end_decode - start_decode)
          .count();

  saveToOutputFile(text, encoded, decoded, czas_kodowania, czas_dekodowania,
                   huffmanCode);
}

int main() {

  string text = loadFromFile();
  if (text.empty()) {
    cout << "Plik jest pusty" << endl;
    return 1;
  }

  buildTree(text);

  return 0;
}
