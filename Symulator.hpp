#pragma once
#include "Dane.hpp"
#include "Walidator.hpp"
#include <cstddef>
#include <queue>
#include <vector>

struct PorownajFee {
    bool operator()(const Transakcja& a, const Transakcja& b) const;
};

class Symulator {
public:
    Symulator(const Konfiguracja& konfiguracja, const std::vector<Transakcja>& wejscie,
              const std::vector<double>& losowania, Strategia strategia);
    std::vector<WynikTransakcji> uruchom();
    long long pobierzCzasKonca() const;
private:
    Strategia strategia;
    std::vector<Transakcja> transakcje;
    std::vector<Walidator> walidatorzy;
    std::vector<WynikTransakcji> wyniki;
    std::vector<double> losowania;
    std::size_t nastepneLosowanie = 0;
    std::queue<Transakcja> fifo;
    std::priority_queue<Transakcja, std::vector<Transakcja>, PorownajFee> fee;
    long long czas = 0;
    std::size_t nastepnaTransakcja = 0;
    void dodajNadejscia();
    bool kolejkaPusta() const;
    Transakcja pobierzTransakcje();
    int wybierzWalidatora();
    void przydzielZadania();
    void sprawdzZakonczenia();
    bool czyPozostalaPraca() const;
};
