#include "Walidator.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

Walidator::Walidator(const DaneWalidatora& dane)
    : id(dane.id), stake(dane.stake) {
}

int Walidator::pobierzId() const {
    return id;
}

long long Walidator::pobierzStake() const {
    return stake;
}

bool Walidator::czyWolny() const {
    return !zajety;
}

bool Walidator::czyKonczy(long long czas) const {
    return zajety && koniec == czas;
}

void Walidator::rozpocznij(const Transakcja& transakcja, long long czas) {
    if (zajety) {
        throw std::logic_error("Walidator jest zajety");
    }
    if (transakcja.czasWykonania > std::numeric_limits<long long>::max() - czas) {
        throw std::overflow_error("Czas zakonczenia przekracza zakres liczby");
    }
    aktualna = transakcja;
    start = czas;
    koniec = czas + transakcja.czasWykonania;
    zajety = true;
}

WynikTransakcji Walidator::zakoncz() {
    if (!zajety) {
        throw std::logic_error("Walidator nie ma zadania");
    }
    WynikTransakcji wynik;
    wynik.idTransakcji = aktualna.id;
    wynik.idWalidatora = id;
    wynik.nadejscie = aktualna.nadejscie;
    wynik.start = start;
    wynik.koniec = koniec;
    wynik.oczekiwanie = start - aktualna.nadejscie;
    wynik.deadline = aktualna.deadline;
    wynik.opoznienie = std::max(0LL, koniec - aktualna.deadline);
    wynik.terminowo = koniec <= aktualna.deadline;
    zajety = false;
    return wynik;
}
