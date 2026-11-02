#pragma once
#include "Dane.hpp"
#include <filesystem>
#include <functional>

struct RezultatPrzebiegu {
    Strategia strategia;
    std::vector<WynikTransakcji> wyniki;
    Podsumowanie podsumowanie;
};
using RaportPostepu = std::function<void(Strategia, const Podsumowanie&)>;
std::vector<double> przygotujLosowania(std::size_t liczba, unsigned int ziarno);
std::vector<RezultatPrzebiegu> wykonajPrzebiegi(
    const Konfiguracja& konfiguracja, const std::vector<Transakcja>& transakcje,
    const std::filesystem::path& folder, std::size_t co = 100,
    const RaportPostepu& raport = {});
const char* nazwaStrategii(Strategia strategia);
