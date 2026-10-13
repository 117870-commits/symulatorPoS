# Symulator PoS — obsługa transakcji

Projekt w C++ na przedmiot Podstawy informatyki. Symulujemy obsługę transakcji przez walidatorów i porównujemy dwie strategie wyboru oczekujących zadań: FIFO oraz FEE.

## Aktualny stan

Repozytorium zawiera opis projektu, szczegółowy plan etapów oraz dane testowe. Pliki źródłowe C++ będziemy dodawać podczas realizacji kolejnych etapów — sam plan nie oznacza jeszcze, że program jest gotowy.

## Pliki

- `opis_projektu.md` — założenia, reguły symulacji, dane i architektura.
- `PLAN_PROJEKTU_POS.md` — instrukcja pracy krok po kroku.
- `konfiguracja.txt` — przykładowa konfiguracja walidatorów i ziarno losowania.
- `zgloszenia.txt` — przykładowe transakcje wejściowe.
- `oczekiwane_wyniki.txt` — ręcznie policzone wyniki kontrolne dla FIFO i FEE.

## Główne zasady

- Czas symulacji liczymy w krokach całkowitych.
- FIFO wybiera najwcześniej przybyłą transakcję, a przy remisie niższe ID.
- FEE wybiera transakcję z najwyższą opłatą; przy remisie rozstrzygają wcześniejsze nadejście i niższe ID.
- Stake wpływa na prawdopodobieństwo wyboru wolnego walidatora, ale nie na szybkość wykonania.
- Dla porównania FIFO i FEE używamy tych samych danych i wspólnej listy losowań.

## Planowane pliki kodu

`main.cpp`, `Dane.hpp`, `Walidator.hpp`, `Walidator.cpp`, `Pliki.hpp`, `Pliki.cpp`, `Symulator.hpp`, `Symulator.cpp`, a w etapie statystyk także `Statystyki.hpp` i `Statystyki.cpp`.

## Kompilacja

Projekt docelowo korzysta ze standardu C++17. Instrukcję kompilacji i uruchomienia uzupełnimy, gdy będą gotowe odpowiednie pliki źródłowe. Szczegółowe kroki znajdują się w `PLAN_PROJEKTU_POS.md`.

## Praca w parze

1. Przed rozpoczęciem pracy zsynchronizuj zmiany z repozytorium.
2. Pracuj nad uzgodnionym plikiem lub zadaniem; unikajcie równoczesnej edycji tego samego pliku.
3. Zapisuj małe, opisane zmiany (commity), np. `Dodaj odczyt konfiguracji`.
4. Po wysłaniu zmian do repozytorium druga osoba pobiera aktualną wersję.
5. Przed oddaniem projektu oboje kompilujecie program i sprawdzacie wyniki testów.

## Uwaga

To uproszczona symulacja idei Proof of Stake, a nie pełna implementacja blockchaina.
