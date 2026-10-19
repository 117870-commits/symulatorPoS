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

Transakcja Symulator::pobierzTransakcje() {
    Transakcja transakcja = fifo.front();
    fifo.pop();
    return transakcja;
}

int Symulator::wybierzWalidatora() const {
    for (std::size_t i = 0; i < walidatorzy.size(); ++i) {
        if (walidatorzy[i].czyWolny()) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
