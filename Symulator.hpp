#pragma once

#include "Dane.hpp"
#include "Walidator.hpp"
#include <cstddef>
#include <queue>
#include <vector>

class Symulator {
public:
    Symulator(const Konfiguracja& konfiguracja,
              const std::vector<Transakcja>& wejscie);
    std::vector<WynikTransakcji> uruchom();
    long long pobierzCzasKonca() const;

private:
    Strategia strategia;
    std::vector<Transakcja> transakcje;
    std::vector<Walidator> walidatorzy;
    std::vector<WynikTransakcji> wyniki;
    std::queue<Transakcja> fifo;
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
