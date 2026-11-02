#include "Przebiegi.hpp"
#include "Pliki.hpp"
#include "Statystyki.hpp"
#include "Symulator.hpp"
#include <random>
#include <stdexcept>

const char* nazwaStrategii(Strategia strategia) {
    return strategia == Strategia::FIFO ? "FIFO" : "FEE";
}
std::vector<double> przygotujLosowania(std::size_t liczba, unsigned int ziarno) {
    std::mt19937 generator(ziarno);
    std::uniform_real_distribution<double> losuj(0.0, 1.0);
    std::vector<double> losowania;
    losowania.reserve(liczba);
    for (std::size_t i = 0; i < liczba; ++i) losowania.push_back(losuj(generator));
    return losowania;
}
std::vector<RezultatPrzebiegu> wykonajPrzebiegi(
    const Konfiguracja& konfiguracja, const std::vector<Transakcja>& transakcje,
    const std::filesystem::path& folder, std::size_t co, const RaportPostepu& raport) {
    if (co == 0) throw std::invalid_argument("Odstep raportowania musi byc dodatni");
    std::filesystem::create_directories(folder);
    const auto losowania = przygotujLosowania(transakcje.size(), konfiguracja.ziarno);
    std::vector<RezultatPrzebiegu> rezultaty;
    for (const auto strategia : {Strategia::FIFO, Strategia::FEE}) {
        Symulator symulator(konfiguracja, transakcje, losowania, strategia);
        Symulator::Raport odbiorca;
        if (raport) odbiorca = [&](const auto& stan) { raport(strategia, stan); };
        auto wyniki = symulator.uruchom(co, odbiorca);
        auto podsumowanie = Statystyki::oblicz(wyniki, konfiguracja.walidatorzy, symulator.pobierzCzasKonca());
        const auto nazwa = strategia == Strategia::FIFO ? "wyniki_fifo.txt" : "wyniki_fee.txt";
        Pliki::zapiszWyniki(folder / nazwa, wyniki, podsumowanie, konfiguracja.walidatorzy);
        rezultaty.push_back({strategia, std::move(wyniki), std::move(podsumowanie)});
    }
    return rezultaty;
}
