#include "Symulator.hpp"

Symulator::Symulator(const Konfiguracja& konfiguracja,
                     const std::vector<Transakcja>& wejscie)
    : transakcje(wejscie) {
    for (const DaneWalidatora& dane : konfiguracja.walidatorzy) {
        walidatorzy.push_back(Walidator(dane));
    }
}
