#include "Symulator.hpp"
#include <stdexcept>

bool PorownajFee::operator()(const Transakcja& a, const Transakcja& b) const {
    if (a.fee != b.fee) return a.fee < b.fee;
    if (a.nadejscie != b.nadejscie) return a.nadejscie > b.nadejscie;
    return a.id > b.id;
}
Symulator::Symulator(const Konfiguracja& konfiguracja, const std::vector<Transakcja>& wejscie,
                     const std::vector<double>& losy, Strategia wybrana)
    : strategia(wybrana), transakcje(wejscie), losowania(losy) {
    for (const auto& dane : konfiguracja.walidatorzy) walidatorzy.emplace_back(dane);
}
void Symulator::dodajNadejscia() {
    while (nastepnaTransakcja < transakcje.size() && transakcje[nastepnaTransakcja].nadejscie == czas) {
        if (strategia == Strategia::FIFO) fifo.push(transakcje[nastepnaTransakcja]);
        else fee.push(transakcje[nastepnaTransakcja]);
        ++nastepnaTransakcja;
    }
}
bool Symulator::kolejkaPusta() const { return strategia == Strategia::FIFO ? fifo.empty() : fee.empty(); }
Transakcja Symulator::pobierzTransakcje() {
    if (strategia == Strategia::FIFO) { auto t = fifo.front(); fifo.pop(); return t; }
    auto t = fee.top(); fee.pop(); return t;
}
int Symulator::wybierzWalidatora() {
    long double suma = 0;
    for (const auto& w : walidatorzy) if (w.czyWolny()) suma += w.pobierzStake();
    if (suma == 0) return -1;
    if (nastepneLosowanie >= losowania.size()) throw std::runtime_error("Brak losowan");
    long double prog = losowania[nastepneLosowanie++] * suma;
    long double zakres = 0;
    int ostatni = -1;
    for (std::size_t i = 0; i < walidatorzy.size(); ++i) {
        if (walidatorzy[i].czyWolny()) {
            ostatni = static_cast<int>(i);
            zakres += walidatorzy[i].pobierzStake();
            if (prog < zakres) return ostatni;
        }
    }
    return ostatni;
}
void Symulator::przydzielZadania() {
    while (!kolejkaPusta()) {
        int i = wybierzWalidatora();
        if (i == -1) break;
        walidatorzy[i].rozpocznij(pobierzTransakcje(), czas);
    }
}
void Symulator::sprawdzZakonczenia() {
    for (auto& w : walidatorzy) if (w.czyKonczy(czas)) wyniki.push_back(w.zakoncz());
}
bool Symulator::czyPozostalaPraca() const {
    if (nastepnaTransakcja < transakcje.size() || !kolejkaPusta()) return true;
    for (const auto& w : walidatorzy) if (!w.czyWolny()) return true;
    return false;
}
std::vector<WynikTransakcji> Symulator::uruchom() {
    while (true) {
        dodajNadejscia();
        przydzielZadania();
        if (!czyPozostalaPraca()) break;
        ++czas;
        sprawdzZakonczenia();
    }
    return wyniki;
}
long long Symulator::pobierzCzasKonca() const { return czas; }
