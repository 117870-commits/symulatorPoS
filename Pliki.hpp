#pragma once

#include "Dane.hpp"
#include <string>
#include <vector>

class Pliki {
public:
    static Konfiguracja wczytajKonfiguracje(const std::string& nazwa);
    static std::vector<Transakcja> wczytajTransakcje(const std::string& nazwa);
    static void zapiszWyniki(const std::string& nazwa,
                             const std::vector<WynikTransakcji>& wyniki);
};
