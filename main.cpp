#include "Pliki.hpp"
#include "Statystyki.hpp"
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
        std::vector<Transakcja> transakcje = Pliki::wczytajTransakcje("zgloszenia.txt");
        std::vector<double> losowania = przygotujLosowania(transakcje.size(), konfiguracja.ziarno);
        Symulator fifo(konfiguracja, transakcje, losowania, Strategia::FIFO);
        std::vector<WynikTransakcji> wynikiFifo = fifo.uruchom();
        Podsumowanie statystykiFifo = Statystyki::oblicz(wynikiFifo, konfiguracja.walidatorzy, fifo.pobierzCzasKonca());
        Pliki::zapiszWyniki("wyniki_fifo.txt", wynikiFifo,statystykiFifo, konfiguracja.walidatorzy);
        Symulator fee(konfiguracja, transakcje, losowania, Strategia::FEE);
        std::vector<WynikTransakcji> wynikiFee = fee.uruchom();
        Podsumowanie statystykiFee = Statystyki::oblicz(wynikiFee, konfiguracja.walidatorzy, fee.pobierzCzasKonca());
        Pliki::zapiszWyniki("wyniki_fee.txt", wynikiFee,statystykiFee, konfiguracja.walidatorzy);
        std::cout << "Zapisano wyniki_fifo.txt i wyniki_fee.txt\n";
        return 0;
    } 
    catch (const std::exception& blad) {
        std::cerr << "Blad: " << blad.what() << '\n';
        return 1;
    }
}
