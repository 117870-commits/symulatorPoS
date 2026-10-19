#include "Pliki.hpp"
#include <exception>
#include <iostream>

int main() {
    try {
        Konfiguracja konfiguracja = Pliki::wczytajKonfiguracje("konfiguracja.txt");
        std::vector<Transakcja> transakcje = Pliki::wczytajTransakcje("zgloszenia.txt");
        std::cout << konfiguracja.walidatorzy.size() << ' ' << konfiguracja.ziarno << ' ' << transakcje.size() << '\n';
        for (const Transakcja& t : transakcje) {
            std::cout << t.id << ' ' << t.nadejscie << ' ' << t.czasWykonania << ' ' << t.fee << ' ' << t.deadline << '\n';
        }
        return 0;
    } catch (const std::exception& blad) {
        std::cerr << "Blad: " << blad.what() << '\n';
        return 1;
    }
}
