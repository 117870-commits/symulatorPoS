# Symulator PoS — obsługa transakcji

Projekt w C++ na przedmiot Podstawy informatyki. W etapie 2 zaimplementowaliśmy podstawę symulacji obsługi transakcji przez walidatorów z wykorzystaniem kolejki FIFO.

## Aktualny stan

Etap 2 jest zakończony. W repozytorium są już gotowe:
- `Dane.hpp` — struktury danych transakcji i walidatora,
- `Walidator.hpp` i `Walidator.cpp` — logika walidatora,
- `Pliki.hpp` i `Pliki.cpp` — odczyt i zapis danych z plików,
- `Symulator.hpp` i `Symulator.cpp` — przebieg symulacji z FIFO,
- `main.cpp` — uruchomienie programu i zapis wyników do `wyniki_fifo.txt`.

## Pliki projektu

- `konfiguracja.txt` — konfiguracja walidatorów i ziarno,
- `zgloszenia.txt` — transakcje wejściowe,
- `oczekiwane_wyniki.txt` — ręcznie policzone wyniki kontrolne,
- `wyniki_fifo.txt` — wynik wygenerowany przez program dla danych testowych.

## Główne zasady etapu 2

- Czas symulacji liczony jest w krokach całkowitych.
- Transakcje trafiają do kolejki FIFO w momencie ich nadejścia.
- Pierwszy wolny walidator otrzymuje pierwszą transakcję z kolejki.
- Po zakończeniu zadania walidator zwalnia się i zapisujemy wynik transakcji.
- Program odczytuje dane z `konfiguracja.txt` i `zgloszenia.txt`, wykonuje symulację i zapisuje wyniki do `wyniki_fifo.txt`.

## Kompilacja i uruchomienie

Kompilacja w wierszu poleceń Windows:
```bat
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Walidator.cpp Pliki.cpp Symulator.cpp -o symulator.exe
.\symulator.exe
```

Alternatywnie, jeśli uruchamiasz program z poziomu terminala bash / Linux / macOS:
```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Walidator.cpp Pliki.cpp Symulator.cpp -o symulator.exe
./symulator.exe
```

## Oczekiwany wynik dla przykładowych danych

Program zapisuje wynik FIFO do `wyniki_fifo.txt` w następującym formacie:
```text
id_transakcji id_walidatora nadejscie start koniec oczekiwanie deadline opoznienie terminowo
1 1 0 0 2 0 10 0 1
2 1 0 2 4 2 10 0 1
3 1 0 4 6 4 10 0 1
```

To odpowiada kolejności: T1 -> T2 -> T3, przy jednym walidatorze i czasie wykonania 2 dla każdej transakcji.

## Uwaga

Etap 3 (strategia FEE, priority_queue i losowanie stake) nie jest realizowany w tej wersji repozytorium. Repozytorium znajduje się na etapie 2, zgodnie z wymaganiem.
