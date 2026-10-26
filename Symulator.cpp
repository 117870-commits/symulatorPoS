#include "Pliki.hpp"
#include "Symulator.hpp"

#include <exception>
#include <iostream>

int main() {
    try {
        Konfiguracja konfiguracja = Pliki::wczytajKonfiguracje("konfiguracja.txt");
        std::vector<Transakcja> transakcje = Pliki::wczytajTransakcje("zgloszenia.txt");

        Symulator fifo(konfiguracja, transakcje, Strategia::FIFO);
        std::vector<WynikTransakcji> wynikiFifo = fifo.uruchom();
        Pliki::zapiszWyniki("wyniki_fifo.txt", wynikiFifo);
        std::cout << "Zapisano wyniki_fifo.txt\n";

        Symulator fee(konfiguracja, transakcje, Strategia::FEE);
        std::vector<WynikTransakcji> wynikiFee = fee.uruchom();
        Pliki::zapiszWyniki("wyniki_fee.txt", wynikiFee);
        std::cout << "Zapisano wyniki_fee.txt\n";

        return 0;
    } catch (const std::exception& blad) {
        std::cerr << "Blad: " << blad.what() << '\n';
        return 1;
    }
}
