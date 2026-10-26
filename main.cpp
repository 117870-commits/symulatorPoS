#include "Pliki.hpp"
#include "Symulator.hpp"
#include <exception>
#include <iostream>
#include <random>

std::vector<double> przygotujLosowania(std::size_t liczba, unsigned int ziarno) {
    std::mt19937 generator(ziarno);
    std::uniform_real_distribution<double> losuj(0.0, 1.0);
    std::vector<double> losowania;
    for (std::size_t i = 0; i < liczba; ++i) losowania.push_back(losuj(generator));
    return losowania;
}
int main() {
    try {
        Konfiguracja konfiguracja = Pliki::wczytajKonfiguracje("konfiguracja.txt");
        auto transakcje = Pliki::wczytajTransakcje("zgloszenia.txt");
        auto losowania = przygotujLosowania(transakcje.size(), konfiguracja.ziarno);
        Symulator fifo(konfiguracja, transakcje, losowania, Strategia::FIFO);
        Symulator fee(konfiguracja, transakcje, losowania, Strategia::FEE);
        Pliki::zapiszWyniki("wyniki_fifo.txt", fifo.uruchom());
        Pliki::zapiszWyniki("wyniki_fee.txt", fee.uruchom());
        std::cout << "Zapisano wyniki_fifo.txt i wyniki_fee.txt\n";
        return 0;
    } catch (const std::exception& blad) {
        std::cerr << "Blad: " << blad.what() << '\n';
        return 1;
    }
}
