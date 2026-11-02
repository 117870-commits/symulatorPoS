#pragma once

#include "Dane.hpp"
#include <filesystem>
#include <vector>

class Pliki {
public:
    static Konfiguracja wczytajKonfiguracje(const std::filesystem::path& nazwa);
    static std::vector<Transakcja> wczytajTransakcje(const std::filesystem::path& nazwa);
    static void zapiszWyniki(const std::filesystem::path& nazwa,
                            const std::vector<WynikTransakcji>& wyniki,
                            const Podsumowanie& podsumowanie,
                            const std::vector<DaneWalidatora>& walidatorzy);
};
