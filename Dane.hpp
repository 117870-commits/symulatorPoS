#pragma once

#include <vector>

struct Transakcja {
    int id = 0;
    long long nadejscie = 0;
    long long czasWykonania = 0;
    long long fee = 0;
    long long deadline = 0;
};

struct DaneWalidatora {
    int id = 0;
    long long stake = 0;
};

struct Konfiguracja {
    unsigned int ziarno = 0;
    std::vector<DaneWalidatora> walidatorzy;
};

struct WynikTransakcji {
    int idTransakcji = 0;
    int idWalidatora = 0;
    long long nadejscie = 0;
    long long start = 0;
    long long koniec = 0;
    long long oczekiwanie = 0;
    long long deadline = 0;
    long long opoznienie = 0;
    bool terminowo = false;
};
