# Symulator PoS — obsługa transakcji

Projekt w C++ na przedmiot Podstawy informatyki. W etapie 3 rozszerzyliśmy podstawę symulacji o strategię FEE oraz wybór walidatora według stake.

## Aktualny stan

Etap 3 jest zakończony. W repozytorium są gotowe:
- `Dane.hpp` — struktury danych transakcji, walidatora, konfiguracji i wyniku,
- `Walidator.hpp` i `Walidator.cpp` — logika walidatora,
- `Pliki.hpp` i `Pliki.cpp` — odczyt i zapis danych z plików,
- `Symulator.hpp` i `Symulator.cpp` — przebieg symulacji dla FIFO i FEE,
- `main.cpp` — uruchomienie programu i zapis wyników do `wyniki_fifo.txt` oraz `wyniki_fee.txt`.

## Co zostało dodane w etapie 3

- `Strategia::FIFO` i `Strategia::FEE`
- kolejka priorytetowa `std::priority_queue` z komparatorem po fee, potem nadejściu, potem ID
- wybór wolnego walidatora z uwzględnieniem jego stake i ziarna losowania
- dwa osobne przebiegi symulacji: FIFO i FEE
- czytelne pliki wejściowe i wynikowe z nazwami kolumn

## Pliki wejściowe i wynikowe

- `konfiguracja.txt` — konfiguracja ziarna i walidatorów
- `zgloszenia.txt` — listy transakcji z parametrami wejściowymi
- `wyniki_fifo.txt` — wyniki dla strategii FIFO
- `wyniki_fee.txt` — wyniki dla strategii FEE
- `oczekiwane_wyniki.txt` — przykładowe wyniki kontrolne

## Kompilacja

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Walidator.cpp Pliki.cpp Symulator.cpp -o symulator.exe
./symulator.exe
```

## Zasady zgodności z planem

Repozytorium jest zgodne z planem etapu 3:
- dodano FEE,
- dodano wybór walidatora według stake,
- zapisano wyniki dla obu strategii,
- poprawiono czytelność plików tekstowych.

Etapy 4, 5 i 6 nie były wykonywane, zgodnie z ustaleniem ograniczenia do samego etapu 3.
