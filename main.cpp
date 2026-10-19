#include "Walidator.hpp"
#include <iostream>

int main() {
    Walidator w(DaneWalidatora{1, 10});
    Transakcja t{1, 0, 2, 1, 10};
    w.rozpocznij(t, 0);

    std::cout << w.czyWolny() << ' ' << w.czyKonczy(2) << '\n';

    WynikTransakcji wynik = w.zakoncz();
    std::cout << wynik.start << ' ' << wynik.koniec << ' ' << wynik.oczekiwanie << ' ' << w.czyWolny() << '\n';
    return 0;
}
