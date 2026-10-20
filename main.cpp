#include "Pliki.hpp"
#include "Symulator.hpp"
#include <exception>
#include <iostream>

int main() {
    try {
        Konfiguracja konfiguracja = Pliki::wczytajKonfiguracje("konfiguracja.txt");
        std::vector<Transakcja> transakcje = Pliki::wczytajTransakcje("zgloszenia.txt");
        Symulator fifo(konfiguracja, transakcje);
        std::vector<WynikTransakcji> wynikiFifo = fifo.uruchom();
        Pliki::zapiszWyniki("wyniki_fifo.txt", wynikiFifo);
        std::cout << "Zapisano wyniki_fifo.txt\n";
        return 0;
    } catch (const std::exception& blad) {
        std::cerr << "Blad: " << blad.what() << '\n';
        return 1;
    }
}
