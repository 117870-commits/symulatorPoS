#include "Pliki.hpp"
#include <algorithm>
#include <fstream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>

namespace {
struct Sekcja { std::string nazwa; int linia; std::map<std::string, long long> pola; };
std::string przytnij(const std::string& s) {
    auto a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    return s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1);
}
[[noreturn]] void blad(const std::string& plik, int linia, const std::string& powod) {
    throw std::runtime_error(plik + " (wiersz " + std::to_string(linia) + "): " + powod);
}
std::vector<Sekcja> czytaj(const std::string& nazwa) {
    std::ifstream plik(nazwa);
    if (!plik) throw std::runtime_error("Nie mozna otworzyc " + nazwa);
    std::vector<Sekcja> sekcje;
    std::string linia;
    int nr = 0;
    while (std::getline(plik, linia)) {
        ++nr;
        auto komentarz = linia.find_first_of("#;");
        if (komentarz != std::string::npos) linia.erase(komentarz);
        linia = przytnij(linia);
        if (linia.empty()) continue;
        if (linia.front() == '[' && linia.back() == ']') {
            auto nazwaSekcji = przytnij(linia.substr(1, linia.size()-2));
            if (nazwaSekcji.empty()) blad(nazwa, nr, "pusta nazwa sekcji");
            sekcje.push_back({nazwaSekcji,nr,{}});
            continue;
        }
        if (sekcje.empty()) blad(nazwa, nr, "pole przed sekcja");
        auto znak = linia.find('=');
        if (znak == std::string::npos) blad(nazwa, nr, "oczekiwano nazwa = liczba");
        auto klucz = przytnij(linia.substr(0,znak));
        auto wartosc = przytnij(linia.substr(znak+1));
        long long liczba;
        std::istringstream strumien(wartosc);
        char nadmiar;
        if (klucz.empty() || !(strumien >> liczba) || (strumien >> nadmiar)) blad(nazwa,nr,"nieprawidlowa liczba lub pole");
        if (!sekcje.back().pola.emplace(klucz,liczba).second) blad(nazwa,nr,"powtorzone pole " + klucz);
    }
    if (plik.bad()) throw std::runtime_error("Blad odczytu " + nazwa);
    return sekcje;
}
void pola(const Sekcja& s, const std::string& nazwa, const std::set<std::string>& wymagane) {
    for (const auto& p : s.pola) if (!wymagane.count(p.first)) blad(nazwa,s.linia,"nieznane pole " + p.first);
    for (const auto& p : wymagane) if (!s.pola.count(p)) blad(nazwa,s.linia,"brak pola " + p);
}
int id(const Sekcja& s, const std::string& nazwa) {
    auto n = s.pola.at("id");
    if (n <= 0 || n > std::numeric_limits<int>::max()) blad(nazwa,s.linia,"ID poza zakresem");
    return static_cast<int>(n);
}
}
Konfiguracja Pliki::wczytajKonfiguracje(const std::string& nazwa) {
    auto sekcje = czytaj(nazwa);
    Konfiguracja konfiguracja;
    bool maSymulacje = false;
    std::set<int> identyfikatory;
    for (const auto& s : sekcje) {
        if (s.nazwa == "symulacja") {
            if (maSymulacje) blad(nazwa,s.linia,"powtorzona sekcja symulacja");
            pola(s,nazwa,{"ziarno"});
            auto ziarno = s.pola.at("ziarno");
            if (ziarno < 0 || static_cast<unsigned long long>(ziarno) > std::numeric_limits<unsigned int>::max()) blad(nazwa,s.linia,"ziarno poza zakresem");
            konfiguracja.ziarno = static_cast<unsigned int>(ziarno);
            maSymulacje = true;
        } else if (s.nazwa == "walidator") {
            pola(s,nazwa,{"id","stake"});
            DaneWalidatora w{id(s,nazwa),s.pola.at("stake")};
            if (w.stake <= 0) blad(nazwa,s.linia,"stake musi byc dodatni");
            if (!identyfikatory.insert(w.id).second) blad(nazwa,s.linia,"powtorzone ID walidatora");
            konfiguracja.walidatorzy.push_back(w);
        } else blad(nazwa,s.linia,"nieznana sekcja " + s.nazwa);
    }
    if (!maSymulacje || konfiguracja.walidatorzy.empty()) blad(nazwa,1,"wymagana sekcja symulacja i co najmniej jeden walidator");
    std::sort(konfiguracja.walidatorzy.begin(),konfiguracja.walidatorzy.end(),[](const auto& a,const auto& b){return a.id < b.id;});
    return konfiguracja;
}
std::vector<Transakcja> Pliki::wczytajTransakcje(const std::string& nazwa) {
    auto sekcje = czytaj(nazwa);
    std::vector<Transakcja> wynik;
    std::set<int> identyfikatory;
    for (const auto& s : sekcje) {
        if (s.nazwa != "transakcja") blad(nazwa,s.linia,"nieznana sekcja " + s.nazwa);
        pola(s,nazwa,{"id","nadejscie","czas_wykonania","fee","deadline"});
        Transakcja t{id(s,nazwa),s.pola.at("nadejscie"),s.pola.at("czas_wykonania"),s.pola.at("fee"),s.pola.at("deadline")};
        if (!identyfikatory.insert(t.id).second) blad(nazwa,s.linia,"powtorzone ID transakcji");
        if (t.nadejscie < 0 || t.czasWykonania <= 0 || t.fee < 0 || t.deadline < t.nadejscie) blad(nazwa,s.linia,"nieprawidlowe wartosci transakcji");
        wynik.push_back(t);
    }
    std::sort(wynik.begin(),wynik.end(),[](const auto& a,const auto& b){return a.nadejscie != b.nadejscie ? a.nadejscie < b.nadejscie : a.id < b.id;});
    return wynik;
}
void Pliki::zapiszWyniki(const std::string& nazwa, const std::vector<WynikTransakcji>& wyniki) {
    std::ofstream plik(nazwa);
    if (!plik) throw std::runtime_error("Nie mozna zapisac " + nazwa);
    plik << "# Wyniki w kolejnosci zakonczenia. Czas jest liczony w krokach.\n";
    for (const auto& w : wyniki) {
        plik << "\n[wynik]\n"
             << "id_transakcji = " << w.idTransakcji << '\n'
             << "id_walidatora = " << w.idWalidatora << '\n'
             << "nadejscie = " << w.nadejscie << '\n'
             << "start = " << w.start << '\n'
             << "koniec = " << w.koniec << '\n'
             << "oczekiwanie = " << w.oczekiwanie << '\n'
             << "deadline = " << w.deadline << '\n'
             << "opoznienie = " << w.opoznienie << '\n'
             << "terminowo = " << (w.terminowo ? "tak" : "nie") << '\n';
    }
    plik.flush();
    if (!plik) throw std::runtime_error("Blad zapisu " + nazwa);
}
