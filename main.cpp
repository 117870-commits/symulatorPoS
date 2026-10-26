#include "Pliki.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::string trim(const std::string& text) {
    std::size_t start = 0;
    while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start]))) {
        ++start;
    }

    std::size_t end = text.size();
    while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1]))) {
        --end;
    }
    return text.substr(start, end - start);
}

bool isCommentOrEmpty(const std::string& line) {
    std::string value = trim(line);
    return value.empty() || value[0] == '#' || value[0] == ';';
}

bool looksLikeHeader(const std::string& line) {
    std::istringstream ss(line);
    std::string token;
    bool sawLetter = false;
    while (ss >> token) {
        if (std::any_of(token.begin(), token.end(), [](unsigned char ch) { return std::isalpha(ch) != 0; })) {
            sawLetter = true;
        }
    }
    return sawLetter;
}

std::vector<std::string> readNonCommentLines(const std::string& nazwa) {
    std::ifstream plik(nazwa);
    if (!plik) {
        throw std::runtime_error("Nie mozna otworzyc " + nazwa);
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(plik, line)) {
        if (!isCommentOrEmpty(line)) {
            lines.push_back(trim(line));
        }
    }
    return lines;
}

} // namespace

Konfiguracja Pliki::wczytajKonfiguracje(const std::string& nazwa) {
    std::vector<std::string> lines = readNonCommentLines(nazwa);
    Konfiguracja konfiguracja;

    int liczbaWalidatorow = -1;
    long long ziarno = -1;

    for (const std::string& line : lines) {
        if (line.find('=') != std::string::npos) {
            std::string key = trim(line.substr(0, line.find('=')));
            std::string value = trim(line.substr(line.find('=') + 1));

            if (key == "seed" || key == "ziarno" || key == "seed_losowania") {
                std::istringstream ss(value);
                ss >> ziarno;
            } else if (key == "validator_count" || key == "liczba_walidatorow") {
                std::istringstream ss(value);
                ss >> liczbaWalidatorow;
            }
            continue;
        }

        if (looksLikeHeader(line)) {
            continue;
        }

        std::istringstream ss(line);
        long long a = 0;
        long long b = 0;
        if (ss >> a >> b) {
            if (liczbaWalidatorow == -1 && ziarno == -1) {
                liczbaWalidatorow = static_cast<int>(a);
                ziarno = b;
                continue;
            }

            DaneWalidatora dane;
            dane.id = static_cast<int>(a);
            dane.stake = b;
            if (dane.id <= 0 || dane.stake <= 0) {
                throw std::runtime_error(nazwa + ": nieprawidlowe dane walidatora");
            }
            konfiguracja.walidatorzy.push_back(dane);
        }
    }

    if (liczbaWalidatorow < 0) {
        throw std::runtime_error(nazwa + ": brak liczby walidatorow");
    }
    if (ziarno < 0) {
        throw std::runtime_error(nazwa + ": brak ziarna losowania");
    }
    if (static_cast<int>(konfiguracja.walidatorzy.size()) != liczbaWalidatorow) {
        throw std::runtime_error(nazwa + ": liczba walidatorow nie zgadza sie z danymi");
    }

    konfiguracja.ziarno = static_cast<unsigned int>(ziarno);
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
    std::vector<std::string> lines = readNonCommentLines(nazwa);
    std::vector<Transakcja> transakcje;
    int liczbaTransakcji = -1;

    for (const std::string& line : lines) {
        if (line.find('=') != std::string::npos) {
            std::string key = trim(line.substr(0, line.find('=')));
            std::string value = trim(line.substr(line.find('=') + 1));
            if (key == "transaction_count" || key == "liczba_transakcji") {
                std::istringstream ss(value);
                ss >> liczbaTransakcji;
            }
            continue;
        }

        if (looksLikeHeader(line)) {
            continue;
        }

        std::istringstream ss(line);
        Transakcja transakcja;
        if (ss >> transakcja.id >> transakcja.nadejscie >> transakcja.czasWykonania
               >> transakcja.fee >> transakcja.deadline) {
            if (transakcja.id <= 0 || transakcja.nadejscie < 0
                || transakcja.czasWykonania <= 0 || transakcja.fee < 0
                || transakcja.deadline < transakcja.nadejscie) {
                throw std::runtime_error(nazwa + ": nieprawidlowe wartosci transakcji");
            }
            transakcje.push_back(transakcja);
        }
    }

    if (liczbaTransakcji >= 0 && static_cast<int>(transakcje.size()) != liczbaTransakcji) {
        throw std::runtime_error(nazwa + ": liczba transakcji nie zgadza sie z danymi");
    }

    std::sort(transakcje.begin(), transakcje.end(),
              [](const Transakcja& a, const Transakcja& b) {
                  if (a.nadejscie != b.nadejscie) {
                      return a.nadejscie < b.nadejscie;
                  }
                  return a.id < b.id;
              });

    for (std::size_t i = 1; i < transakcje.size(); ++i) {
        if (transakcje[i - 1].id == transakcje[i].id) {
            throw std::runtime_error(nazwa + ": powtorzone ID transakcji");
        }
    }

    return transakcje;
}

void Pliki::zapiszWyniki(const std::string& nazwa,
                         const std::vector<WynikTransakcji>& wyniki) {
    std::ofstream plik(nazwa);
    if (!plik) {
        throw std::runtime_error("Nie mozna zapisac " + nazwa);
    }

    plik << "# Wynik symulacji\n";
    plik << "# kolumny: transaction_id validator_id arrival_time start_time end_time waiting_time deadline delay is_on_time\n";
    plik << "transaction_id validator_id arrival_time start_time end_time waiting_time deadline delay is_on_time\n";

    for (const WynikTransakcji& wynik : wyniki) {
        plik << wynik.idTransakcji << ' ' << wynik.idWalidatora << ' ' << wynik.nadejscie
             << ' ' << wynik.start << ' ' << wynik.koniec << ' ' << wynik.oczekiwanie << ' '
             << wynik.deadline << ' ' << wynik.opoznienie << ' '
             << (wynik.terminowo ? "true" : "false") << '\n';
    }

    plik.flush();
    if (!plik) {
        throw std::runtime_error("Blad zapisu " + nazwa);
    }
}
