#pragma once

#include "Dane.hpp"

class Walidator {
public:
    explicit Walidator(const DaneWalidatora& dane);
    int pobierzId() const;
    long long pobierzStake() const;
    bool czyWolny() const;
    bool czyKonczy(long long czas) const;
    void rozpocznij(const Transakcja& transakcja, long long czas);
    WynikTransakcji zakoncz();

private:
    int id;
    long long stake;
    bool zajety = false;
    Transakcja aktualna;
    long long start = 0;
    long long koniec = 0;
};
