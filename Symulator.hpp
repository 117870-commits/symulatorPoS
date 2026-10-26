#pragma once

#include "Dane.hpp"
#include "Walidator.hpp"
#include <cstddef>
#include <queue>
#include <random>
#include <vector>

struct PorownajFee {
    bool operator()(const Transakcja& a, const Transakcja& b) const;
};

class Symulator {
public:
    Symulator(const Konfiguracja& konfiguracja,
              const std::vector<Transakcja>& wejscie,
              Strategia wybranaStrategia);
    std::vector<WynikTransakcji> uruchom();
    long long pobierzCzasKonca() const;

private:
    Strategia strategia;
    std::vector<Transakcja> transakcje;
    std::vector<Walidator> walidatorzy;
    std::vector<WynikTransakcji> wyniki;
    std::queue<Transakcja> fifo;
    std::priority_queue<Transakcja, std::vector<Transakcja>, PorownajFee> fee;
    std::mt19937 generator;
    long long czas = 0;
    std::size_t nastepnaTransakcja = 0;

    void dodajNadejscia();
    bool kolejkaPusta() const;
    Transakcja pobierzTransakcje();
    int wybierzWalidatora() const;
    void przydzielZadania();
    void sprawdzZakonczenia();
    bool czyPozostalaPraca() const;
};
