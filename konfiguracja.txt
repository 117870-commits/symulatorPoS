#include "Symulator.hpp"

#include <random>

bool PorownajFee::operator()(const Transakcja& a, const Transakcja& b) const {
    if (a.fee != b.fee) {
        return a.fee < b.fee;
    }
    if (a.nadejscie != b.nadejscie) {
        return a.nadejscie > b.nadejscie;
    }
    return a.id > b.id;
}

Symulator::Symulator(const Konfiguracja& konfiguracja,
                     const std::vector<Transakcja>& wejscie,
                     Strategia wybranaStrategia)
    : strategia(wybranaStrategia),
      transakcje(wejscie),
      generator(konfiguracja.ziarno) {
    for (const DaneWalidatora& dane : konfiguracja.walidatorzy) {
        walidatorzy.push_back(Walidator(dane));
    }
}

void Symulator::dodajNadejscia() {
    while (nastepnaTransakcja < transakcje.size()
           && transakcje[nastepnaTransakcja].nadejscie == czas) {
        const Transakcja& transakcja = transakcje[nastepnaTransakcja];
        if (strategia == Strategia::FIFO) {
            fifo.push(transakcja);
        } else {
            fee.push(transakcja);
        }
        ++nastepnaTransakcja;
    }
}

bool Symulator::kolejkaPusta() const {
    if (strategia == Strategia::FIFO) {
        return fifo.empty();
    }
    return fee.empty();
}

Transakcja Symulator::pobierzTransakcje() {
    if (strategia == Strategia::FIFO) {
        Transakcja transakcja = fifo.front();
        fifo.pop();
        return transakcja;
    }

    Transakcja transakcja = fee.top();
    fee.pop();
    return transakcja;
}

int Symulator::wybierzWalidatora() const {
    std::vector<int> wolni;
    long long sumaStake = 0;

    for (std::size_t i = 0; i < walidatorzy.size(); ++i) {
        if (walidatorzy[i].czyWolny()) {
            wolni.push_back(static_cast<int>(i));
            sumaStake += walidatorzy[i].pobierzStake();
        }
    }

    if (wolni.empty() || sumaStake <= 0) {
        return -1;
    }

    std::uniform_real_distribution<double> rozklad(0.0, 1.0);
    const double los = rozklad(generator);
    double prog = 0.0;

    for (int indeks : wolni) {
        prog += static_cast<double>(walidatorzy[indeks].pobierzStake())
                / static_cast<double>(sumaStake);
        if (los < prog) {
            return indeks;
        }
    }

    return wolni.back();
}

void Symulator::przydzielZadania() {
    while (!kolejkaPusta()) {
        int index = wybierzWalidatora();
        if (index == -1) {
            break;
        }

        Transakcja transakcja = pobierzTransakcje();
        walidatorzy[index].rozpocznij(transakcja, czas);
    }
}

void Symulator::sprawdzZakonczenia() {
    for (Walidator& walidator : walidatorzy) {
        if (walidator.czyKonczy(czas)) {
            WynikTransakcji wynik = walidator.zakoncz();
            wyniki.push_back(wynik);
        }
    }
}

bool Symulator::czyPozostalaPraca() const {
    if (nastepnaTransakcja < transakcje.size() || !kolejkaPusta()) {
        return true;
    }

    for (const Walidator& walidator : walidatorzy) {
        if (!walidator.czyWolny()) {
            return true;
        }
    }

    return false;
}

std::vector<WynikTransakcji> Symulator::uruchom() {
    while (true) {
        dodajNadejscia();
        przydzielZadania();

        if (!czyPozostalaPraca()) {
            break;
        }

        ++czas;
        sprawdzZakonczenia();
    }

    return wyniki;
}

long long Symulator::pobierzCzasKonca() const {
    return czas;
}
