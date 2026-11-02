#include "Pliki.hpp"
#include "Przebiegi.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <stdexcept>
#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

unsigned long long liczba(const std::string& tekst) {
    if (tekst.empty() || tekst.find_first_not_of("0123456789") != std::string::npos)
        throw std::invalid_argument("Oczekiwano nieujemnej liczby calkowitej: " + tekst);
    return std::stoull(tekst);
}
int main(int argc, char** argv) {
    try {
        std::vector<std::filesystem::path> argumenty;
#ifdef _WIN32
        int ile = 0;
        auto szerokie = CommandLineToArgvW(GetCommandLineW(), &ile);
        if (!szerokie) throw std::runtime_error("Nie mozna odczytac argumentow");
        for (int i = 1; i < ile; ++i) argumenty.emplace_back(szerokie[i]);
        LocalFree(szerokie);
        (void)argc; (void)argv;
#else
        for (int i = 1; i < argc; ++i) argumenty.emplace_back(argv[i]);
#endif
        std::filesystem::path konfiguracja = "konfiguracja.txt", zgloszenia = "zgloszenia.txt", folder = ".";
        std::size_t co = 100;
        std::optional<unsigned int> ziarno;
        bool cicho = false;
        for (std::size_t i = 0; i < argumenty.size(); ++i) {
            auto opcja = argumenty[i].u8string();
            if (opcja == "--pomoc" || opcja == "--help") {
                std::cout << "Symulator PoS - FIFO i FEE\n"
                             "--konfiguracja PLIK --zgloszenia PLIK --wyniki FOLDER\n"
                             "--ziarno LICZBA --co LICZBA (domyslnie 100) --cicho\n"
                             "Raporty sa narastajace; wykorzystanie obejmuje prace w toku.\n";
                return 0;
            }
            if (opcja == "--cicho") { cicho = true; continue; }
            if (opcja != "--konfiguracja" && opcja != "--zgloszenia" && opcja != "--wyniki" && opcja != "--ziarno" && opcja != "--co")
                throw std::invalid_argument("Nieznana opcja: " + opcja);
            if (++i == argumenty.size()) throw std::invalid_argument("Brak wartosci dla " + opcja);
            const auto& wartosc = argumenty[i];
            if (opcja == "--konfiguracja") konfiguracja = wartosc;
            else if (opcja == "--zgloszenia") zgloszenia = wartosc;
            else if (opcja == "--wyniki") folder = wartosc;
            else {
                auto n = liczba(wartosc.u8string());
                if (opcja == "--ziarno") {
                    if (n > std::numeric_limits<unsigned int>::max()) throw std::invalid_argument("Ziarno poza zakresem");
                    ziarno = static_cast<unsigned int>(n);
                } else {
                    if (n == 0 || n > std::numeric_limits<std::size_t>::max()) throw std::invalid_argument("Nieprawidlowy odstep raportowania");
                    co = static_cast<std::size_t>(n);
                }
            }
        }
        auto k = Pliki::wczytajKonfiguracje(konfiguracja);
        auto t = Pliki::wczytajTransakcje(zgloszenia);
        if (ziarno) k.ziarno = *ziarno;
        RaportPostepu raport;
        if (!cicho) {
            std::cout << "Transakcje=" << t.size() << " walidatorzy=" << k.walidatorzy.size() << " ziarno=" << k.ziarno << '\n';
            raport = [](Strategia strategia, const Podsumowanie& s) {
                const auto& u = s.wykorzystanie;
                const double srednie = u.empty() ? 0 : std::accumulate(u.begin(), u.end(), 0.0) / u.size();
                std::cout << std::fixed << std::setprecision(2) << '[' << nazwaStrategii(strategia)
                          << "] obsluzone=" << s.liczbaObsluzonych << " czas=" << s.czasKonca
                          << " srednie_oczekiwanie=" << s.srednieOczekiwanie
                          << " maksymalne_oczekiwanie=" << s.maksymalneOczekiwanie
                          << " terminowe=" << s.terminowe << " spoznione=" << s.spoznione
                          << " wykorzystanie_srednie=" << srednie << "% min="
                          << (u.empty() ? 0 : *std::min_element(u.begin(), u.end())) << "% max="
                          << (u.empty() ? 0 : *std::max_element(u.begin(), u.end())) << "%\n";
            };
        }
        wykonajPrzebiegi(k, t, folder, co, raport);
        if (!cicho) std::cout << "Zapisano wyniki_fifo.txt i wyniki_fee.txt w " << folder.u8string()
                             << ". Wykorzystanie kazdego walidatora jest w plikach wynikow.\n";
        return 0;
    } catch (const std::exception& blad) {
        std::cerr << "Blad: " << blad.what() << '\n';
        return 1;
    }
}
