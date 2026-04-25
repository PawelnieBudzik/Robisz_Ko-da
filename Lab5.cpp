#include <fstream>
#include <iostream>
#include <queue>
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
  string data = "";

  std::ifstream file("input.txt");
  string line = "";

  if (file.is_open()) {
    getline(file, line);
    file.close();
  } else {
    cout << "Unable to open file" << endl;
  }

  return line;
}

void printCodes(Node *root, string str,
                unordered_map<char, string> &huffmanCode) {

  if (root == nullptr) {
    return;
  }

  if (!root->left && !root->right) {
    huffmanCode[root->ch] = str;
  }

  printCodes(root->left, str + "0", huffmanCode);
  printCodes(root->right, str + "1", huffmanCode);
}

void returnCodesOnConsole(unordered_map<char, string> huffmanCode) {
  for (auto pair : huffmanCode) {
    cout << pair.first << " " << pair.second << endl;
  }
};

string hashText(string text, unordered_map<char, string> huffmanCode) {
  string hashedStr = "";
  for (char ch : text) {
    hashedStr += huffmanCode[ch];
  }

  return hashedStr;
};

void unhashText(string text, Node *root) {
  Node *curr = root;
  for (char bit : text) {
    if (bit == '0') {
      curr = curr->left;
    } else {
      curr = curr->right;
    }

    if (!curr->left && !curr->right) {
      cout << curr->ch;
      curr = root;
    }
  }
  cout << endl;
}

void buildTree(string text) {
  unordered_map<char, int> frequency;

  for (char ch : text) {
    frequency[ch]++;
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
  printCodes(root, "", huffmanCode);

  cout << "Codes: " << endl;
  returnCodesOnConsole(huffmanCode);

  cout << "Original string: " << text << endl;

  cout << "hashed: " << hashText(text, huffmanCode) << endl;

  string hashed = hashText(text, huffmanCode);

  cout << "Decoded: " << endl;
  unhashText(hashed, root);
}

int main() {

  string text = loadFromFile();
  buildTree(text);

  return 0;
}
