#include "Pliki.hpp"
#include "Symulator.hpp"
#include <exception>
#include <iostream>
int main() {
    try {
        Konfiguracja konfiguracja = Pliki::wczytajKonfiguracje("konfiguracja.txt");
        std::vector<Transakcja> transakcje = Pliki::wczytajTransakcje("zgloszenia.txt");
        Symulator symulator(konfiguracja, transakcje);
        std::vector<WynikTransakcji> wyniki = symulator.uruchom();
        for (const WynikTransakcji& w : wyniki) {
            std::cout << w.idTransakcji << ' ' << w.start << ' '
                      << w.koniec << ' ' << w.oczekiwanie << '\n';
        }
        return 0;
    } catch (const std::exception& blad) {
        std::cerr << "Blad: " << blad.what() << '\n';
        return 1;
    }
}
