#include <iostream>
#include <vector>
#include <algorithm>    
#include <numeric>
#include <cmath>    
#include <string>
#include <queue>
#include <map>

using namespace std;

// Węzeł drzewa Huffmana
struct Wezel {
    char litera;
    int czestotliwosc;
    Wezel* lewy;
    Wezel* prawy;
    Wezel(char l, int c) : litera(l), czestotliwosc(c), lewy(nullptr), prawy(nullptr) {}
};

// Komparator do kolejki priorytetowej (najmniejsza częstotliwość na górze)
struct Porownaj {
    bool operator()(Wezel* a, Wezel* b){
        return a->czestotliwosc > b->czestotliwosc;
    }
};

// Zliczanie liter
map<char, int> zlicz_litery(const string& slowo){
    map<char, int> wynik;
    for(char c : slowo) wynik[c]++;
    return wynik;
}

// Budowanie drzewa Huffmana
Wezel* buduj_drzewo(map<char, int>& czestotliwosci){
    priority_queue<Wezel*, vector<Wezel*>, Porownaj> kolejka;
    for(auto& p : czestotliwosci)
        kolejka.push(new Wezel(p.first, p.second));

    while(kolejka.size() > 1){
        Wezel* lewy = kolejka.top(); kolejka.pop();
        Wezel* prawy = kolejka.top(); kolejka.pop();
        Wezel* rodzic = new Wezel('\0', lewy->czestotliwosc + prawy->czestotliwosc);
        rodzic->lewy = lewy;
        rodzic->prawy = prawy;
        kolejka.push(rodzic);
    }
    return kolejka.top();
}

// Generowanie kodów binarnych (0 = lewo, 1 = prawo)
void generuj_kody(Wezel* wezel, string kod, map<char, string>& kody){
    if(!wezel) return;
    if(wezel->litera != '\0'){
        kody[wezel->litera] = kod;
        return;
    }
    generuj_kody(wezel->lewy,  kod + "0", kody);
    generuj_kody(wezel->prawy, kod + "1", kody);
}

int main(){
    string slowo = "alljoviiawwwe";

    map<char, int> czestotliwosci = zlicz_litery(slowo);
    cout << "Czestotliwosci:" << endl;
    for(auto& p : czestotliwosci)
        cout << p.first << ": " << p.second << endl;

    Wezel* korzen = buduj_drzewo(czestotliwosci);

    map<char, string> kody;
    generuj_kody(korzen, "", kody);

    cout << "\nKody Huffmana:" << endl;
    for(auto& p : kody)
        cout << p.first << ": " << p.second << endl;

    cout << "\nZakodowane slowo: ";
    for(char c : slowo)
        cout << kody[c];
    cout << endl;
}

vector<char> podzial_slowa(string slowo){
    vector<char> wynik;
    for(int i = 0; i < slowo.size(); i++){
        wynik.push_back(slowo[i]);
    }
    return wynik;
}

vector<pair<int, char>> podzial_slowa2(vector<char> slowo){
    vector<pair<int, char>> wynik;
    for(int i = 0; i < slowo.size(); i++){
        bool znaleziono = false;
        for(int j = 0; j < wynik.size(); j++){
            if(wynik[j].second == slowo[i]){
                wynik[j].first++;
                znaleziono = true;
                break;
            }
        }
        if(!znaleziono){
            wynik.push_back(make_pair(1, slowo[i]));
        }
    }
    return wynik;
}


vector<pair<int, char>> drzewo_dzielenie(vector<pair<int, char>>& wektor){
    if(wektor.empty()) return {};
    int najwiekszy = 0;
    vector<pair<int, char>> wynik;
    for(int b = 0; b < wektor.size(); b++){
        if(wektor[b].first > wektor[najwiekszy].first){
            najwiekszy = b;
        }
    }
    wynik.push_back(wektor[najwiekszy]);
    wektor.erase(wektor.begin() + najwiekszy);
    if(wektor.size() > 0){
        vector<pair<int, char>> lewy = drzewo_dzielenie(wektor);
        vector<pair<int, char>> prawy = drzewo_dzielenie(wektor);
        wynik.insert(wynik.end(), lewy.begin(), lewy.end());
        wynik.insert(wynik.end(), prawy.begin(), prawy.end());
    }
    return wynik;
}


char* kodowanie(vector<pair<int, char>>& wektor){
    static char wynik[999999];
    for(int i = 0; i < wektor.size(); i++){
        for(int j = 0; j < wektor[i].first; j++){
            wynik[j] = wektor[i].second;
        }
    }
    return wynik;
}

int main(){
    string slowo = "alljoviiawwwe";
    vector<char> wynik = podzial_slowa(slowo);
    vector<pair<int, char>> wynik2 = podzial_slowa2(wynik);
    for(int i = 0; i < wynik2.size(); i++){
        cout << wynik2[i].second << ": " << wynik2[i].first << endl;
    }
    vector<pair<int, char>> wynik3 = drzewo_dzielenie(wynik2);
    cout << "Drzewo dzielenia: " << endl;
    for(int i = 0; i < wynik3.size(); i++){
        cout << wynik3[i].second << ": " << wynik3[i].first << endl;
    }
    char* wynik4 = kodowanie(wynik3);
    cout << "Zakodowane slowo: " << wynik4 << endl;
}