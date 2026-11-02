#pragma once
#include "Dane.hpp"
class Statystyki {
public:
    static Podsumowanie oblicz(const std::vector<WynikTransakcji>& wyniki,
                              const std::vector<DaneWalidatora>& walidatorzy,
                              long long czasKonca);
};