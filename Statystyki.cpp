#include "Statystyki.hpp"
#include <algorithm>

Podsumowanie Statystyki::oblicz(const std::vector<WynikTransakcji>& wyniki,
                              const std::vector<DaneWalidatora>& walidatorzy,
                              long long czasKonca) {
    Podsumowanie podsumowanie;
    podsumowanie.liczbaObsluzonych = static_cast<long long>(wyniki.size());
    podsumowanie.czasKonca = czasKonca;
    double sumaOczekiwania = 0;
    for (const WynikTransakcji& wynik : wyniki) {
        sumaOczekiwania += wynik.oczekiwanie;
        podsumowanie.maksymalneOczekiwanie = std::max(
            podsumowanie.maksymalneOczekiwanie, wynik.oczekiwanie);
        if (wynik.terminowo) {
            ++podsumowanie.terminowe;
        } else {
            ++podsumowanie.spoznione;
        }
    }
    if (!wyniki.empty()) {
        podsumowanie.srednieOczekiwanie = sumaOczekiwania / wyniki.size();
    }
    for (const DaneWalidatora& walidator : walidatorzy) {
        long long czasPracy = 0;
        for (const WynikTransakcji& wynik : wyniki) {
            if (wynik.idWalidatora == walidator.id) {
                czasPracy += wynik.koniec - wynik.start;
            }
        }
        double procent = 0;
        if (czasKonca > 0) {
            procent = 100.0 * czasPracy / czasKonca;
        }
        podsumowanie.wykorzystanie.push_back(procent);
    }
    return podsumowanie;
}
