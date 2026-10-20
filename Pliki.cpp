#include "Pliki.hpp"

#include <algorithm>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace {
void blad(const std::string& nazwa, int numer, const std::string& powod) {
    throw std::runtime_error(nazwa + " (wiersz " + std::to_string(numer)
                             + "): " + powod);
}

std::istringstream wczytajWiersz(std::ifstream& plik,
                                const std::string& nazwa, int numer) {
    std::string linia;
    if (!std::getline(plik, linia)) {
        blad(nazwa, numer, "brak wymaganego wiersza");
    }
    return std::istringstream(linia);
}

void sprawdzKoniecWiersza(std::istringstream& wiersz,
                          const std::string& nazwa, int numer) {
    std::string nadmiar;
    if (wiersz >> nadmiar) {
        blad(nazwa, numer, "za duzo danych w wierszu");
    }
}

void sprawdzKoniecPliku(std::ifstream& plik,
                        const std::string& nazwa, int numer) {
    std::string linia;
    while (std::getline(plik, linia)) {
        std::istringstream wiersz(linia);
        std::string nadmiar;
        if (wiersz >> nadmiar) {
            blad(nazwa, numer, "nadmiarowe dane po ostatnim rekordzie");
        }
        ++numer;
    }
}
}

Konfiguracja Pliki::wczytajKonfiguracje(const std::string& nazwa) {
    std::ifstream plik(nazwa);
    if (!plik) {
        throw std::runtime_error("Nie mozna otworzyc " + nazwa);
    }
    auto naglowek = wczytajWiersz(plik, nazwa, 1);

    int liczba = 0;
    long long ziarno = 0;
    if (!(naglowek >> liczba >> ziarno)) {
        blad(nazwa, 1, "oczekujemy liczby walidatorow i ziarna");
    }
    sprawdzKoniecWiersza(naglowek, nazwa, 1);
    if (liczba < 1 || ziarno < 0
        || static_cast<unsigned long long>(ziarno)
           > std::numeric_limits<unsigned int>::max()) {
        blad(nazwa, 1, "nieprawidlowa liczba walidatorow lub ziarno");
    }
    Konfiguracja konfiguracja;
    konfiguracja.ziarno = static_cast<unsigned int>(ziarno);
    for (int i = 0; i < liczba; ++i) {
        auto wiersz = wczytajWiersz(plik, nazwa, i + 2);
        DaneWalidatora dane;
        if (!(wiersz >> dane.id >> dane.stake)) {
            blad(nazwa, i + 2, "oczekujemy ID i stake");
        }
        sprawdzKoniecWiersza(wiersz, nazwa, i + 2);
        if (dane.id <= 0 || dane.stake <= 0) {
            blad(nazwa, i + 2, "ID i stake musza byc dodatnie");
        }
        konfiguracja.walidatorzy.push_back(dane);
    }
    sprawdzKoniecPliku(plik, nazwa, liczba + 2);
    std::sort(konfiguracja.walidatorzy.begin(), konfiguracja.walidatorzy.end(),
              [](const DaneWalidatora& a, const DaneWalidatora& b) {
                  return a.id < b.id;
              });
    for (std::size_t i = 1; i < konfiguracja.walidatorzy.size(); ++i) {
        if (konfiguracja.walidatorzy[i - 1].id == konfiguracja.walidatorzy[i].id) {
            throw std::runtime_error(nazwa + ": powtorzone ID walidatora");
        }
    }
    return konfiguracja;
}

std::vector<Transakcja> Pliki::wczytajTransakcje(const std::string& nazwa) {
    std::ifstream plik(nazwa);
    if (!plik) {
        throw std::runtime_error("Nie mozna otworzyc " + nazwa);
    }
    auto naglowek = wczytajWiersz(plik, nazwa, 1);

    int liczba = 0;
    if (!(naglowek >> liczba)) {
        blad(nazwa, 1, "oczekujemy liczby transakcji");
    }
    sprawdzKoniecWiersza(naglowek, nazwa, 1);
    if (liczba < 0) {
        blad(nazwa, 1, "liczba transakcji nie moze byc ujemna");
    }
    std::vector<Transakcja> transakcje;
    for (int i = 0; i < liczba; ++i) {
        auto wiersz = wczytajWiersz(plik, nazwa, i + 2);
        Transakcja transakcja;
        if (!(wiersz >> transakcja.id >> transakcja.nadejscie
              >> transakcja.czasWykonania >> transakcja.fee >> transakcja.deadline)) {
            blad(nazwa, i + 2, "oczekujemy pieciu liczb transakcji");
        }
        sprawdzKoniecWiersza(wiersz, nazwa, i + 2);
        if (transakcja.id <= 0 || transakcja.nadejscie < 0
            || transakcja.czasWykonania <= 0 || transakcja.fee < 0
            || transakcja.deadline < transakcja.nadejscie) {
            blad(nazwa, i + 2, "nieprawidlowe wartosci transakcji");
        }
        transakcje.push_back(transakcja);
    }
    sprawdzKoniecPliku(plik, nazwa, liczba + 2);
    std::sort(transakcje.begin(), transakcje.end(),
              [](const Transakcja& a, const Transakcja& b) {
                  return a.id < b.id;
              });
    for (std::size_t i = 1; i < transakcje.size(); ++i) {
        if (transakcje[i - 1].id == transakcje[i].id) {
            throw std::runtime_error(nazwa + ": powtorzone ID transakcji");
        }
    }
    std::sort(transakcje.begin(), transakcje.end(),
              [](const Transakcja& a, const Transakcja& b) {
                  if (a.nadejscie != b.nadejscie) {
                      return a.nadejscie < b.nadejscie;
                  }
                  return a.id < b.id;
              });
    return transakcje;
}

void Pliki::zapiszWyniki(const std::string& nazwa,
                         const std::vector<WynikTransakcji>& wyniki) {
    std::ofstream plik(nazwa);
    if (!plik) {
        throw std::runtime_error("Nie mozna zapisac " + nazwa);
    }
    for (const auto& wynik : wyniki) {
        plik << wynik.idTransakcji << ' ' << wynik.idWalidatora << ' '
             << wynik.nadejscie << ' ' << wynik.start << ' ' << wynik.koniec << ' '
             << wynik.oczekiwanie << ' ' << wynik.deadline << ' ' << wynik.opoznienie << ' '
             << wynik.terminowo << '\n';
    }
}
