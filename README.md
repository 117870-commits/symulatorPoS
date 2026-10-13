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

1. Podział obowiązków

## Osoba A — dane, walidator i pliki
Odpowiedzialność za przygotowanie danych i ich poprawność
Pliki:
- Dane.hpp
- Walidator.hpp
- Walidator.cpp
- Pliki.hpp
- Pliki.cpp
Zadania:
1. Utworzyć rekordy danych: Transakcja, DaneWalidatora, Konfiguracja i WynikTransakcji.
2. Zaimplementować walidator — rozpoczęcie zadania, czas zakończenia, utworzenie wyniku i zwolnienie wykonawcy.
3. Zaimplementować wczytywanie konfiguracja.txt i zgloszenia.txt.
4. Sprawdzać poprawność danych, kompletność kolumn, powtarzające się ID i dodatkowe rekordy.
5. Posortować walidatorów po ID, a transakcje po nadejściu i ID.
6. Przygotować zapis wyników FIFO w klasie Pliki, zgodnie z interfejsem przewidzianym w planie.




## Osoba B — uruchomienie, symulator i FIFO
Odpowiedzialność za przebieg symulacji i połączenie programu
Pliki:
- main.cpp
- Symulator.hpp
- Symulator.cpp
- README.md
Zadania:
1. Uruchomić pierwszy program w C++17 i sprawdzić kompilację.
2. Przygotować Symulator, który przechowuje transakcje, walidatorów, wyniki i kolejkę FIFO.
3. Dodać transakcje do kolejki dopiero w ich czasie nadejścia.
4. Pobierać pierwszą transakcję z FIFO i wybierać pierwszego wolnego walidatora według ID.
5. Obsłużyć upływ czasu, kończenie zadań i zwalnianie walidatorów.
6. Połączyć symulator z klasą Pliki w main.cpp, zapisać wyniki FIFO i przygotować instrukcję kompilacji oraz uruchomienia w README.md.



## Ten podział bazuje na klasach i kolejności prac z rozdziałów 2.2–2.5 planu. PLAN_PROJEKTU_POS.md
 PLAN_PROJEKTU_POS.md

2. Jak podzielić pracę, żeby nie blokować się nawzajem?
Najważniejsze jest to, że osoba B potrzebuje typów danych i metod przygotowanych przez osobę A. Dlatego ustalcie wspólne interfejsy na początku, zanim zaczniecie pisać niezależne fragmenty.
1. Najpierw wspólnie: ustalcie nazwy pól, metod i plików dokładnie według planu. Nie zmieniajcie samodzielnie interfejsów klas.
2. Osoba A: zaczyna od Dane.hpp, a potem przygotowuje Walidator.hpp/.cpp i Pliki.hpp/.cpp zgodnie z kolejnością instrukcji.
3. Osoba B: może równolegle przygotować początkowy main.cpp, kompilację i README.md. Następnie tworzy Symulator.hpp/.cpp, korzystając z ustalonych deklaracji.
4. Po połączeniu: razem sprawdzacie kompilację, działanie odczytu, przebieg FIFO i zapis wyników. Wspólnie poprawiacie błędy integracji.
Plan wymaga wpisywania kodu małymi fragmentami, po 1–6 linii, oraz kompilowania na wskazanych przystankach. Nie musicie więc czekać z całym testowaniem do końca etapu. opis_projektu.md
To uproszczona symulacja idei Proof of Stake, a nie pełna implementacja blockchaina.
