#include "Symulator.hpp"

Symulator::Symulator(const Konfiguracja& konfiguracja,
                     const std::vector<Transakcja>& wejscie)
    : transakcje(wejscie) {
    for (const DaneWalidatora& dane : konfiguracja.walidatorzy) {
        walidatorzy.push_back(Walidator(dane));
    }
}

void Symulator::dodajNadejscia() {
    while (nastepnaTransakcja < transakcje.size()
           && transakcje[nastepnaTransakcja].nadejscie == czas) {
        fifo.push(transakcje[nastepnaTransakcja]);
        ++nastepnaTransakcja;
    }
}

bool Symulator::kolejkaPusta() const {
    return fifo.empty();
}
