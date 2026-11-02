#pragma once
#include "Dane.hpp"
#include <algorithm>
#include <numeric>
#include <stdexcept>

inline long long wartoscKolumny(const WynikTransakcji& w, int kolumna) {
    switch (kolumna) {
    case 0: return w.idTransakcji;
    case 1: return w.idWalidatora;
    case 2: return w.nadejscie;
    case 3: return w.start;
    case 4: return w.koniec;
    case 5: return w.oczekiwanie;
    case 6: return w.deadline;
    case 7: return w.opoznienie;
    case 8: return w.terminowo;
    default: throw std::invalid_argument("Nieznana kolumna wynikow");
    }
}

inline std::vector<std::size_t> uporzadkujWidok(
    const std::vector<WynikTransakcji>& wyniki, int kolumna, bool rosnaco) {
    std::vector<std::size_t> indeksy(wyniki.size());
    std::iota(indeksy.begin(), indeksy.end(), 0);
    if (kolumna == -1) return indeksy;
    if (kolumna < 0 || kolumna > 8) throw std::invalid_argument("Nieznana kolumna wynikow");
    std::stable_sort(indeksy.begin(), indeksy.end(), [&](auto a, auto b) {
        auto x=wartoscKolumny(wyniki[a],kolumna), y=wartoscKolumny(wyniki[b],kolumna);
        return rosnaco ? x<y : x>y;
    });
    return indeksy;
}
