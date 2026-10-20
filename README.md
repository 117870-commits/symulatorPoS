# Symulator PoS — obsługa transakcji

Projekt w C++ na przedmiot Podstawy informatyki. W etapie 2 zaimplementowaliśmy podstawę symulacji obsługi transakcji przez walidatorów z wykorzystaniem kolejki FIFO.

## Aktualny stan

Etap 2 jest zakończony. W repozytorium są już gotowe:
- `Dane.hpp` — struktury danych transakcji i walidatora,
- `Walidator.hpp` i `Walidator.cpp` — logika walidatora,
- `Pliki.hpp` i `Pliki.cpp` — odczyt i zapis danych z plików,
- `Symulator.hpp` i `Symulator.cpp` — przebieg symulacji z FIFO,
- `main.cpp` — uruchomienie programu i zapis wyników do `wyniki_fifo.txt`.

## Podsumowanie plików projektu

Poniżej znajduje się krótkie podsumowanie najważniejszych plików w repozytorium oraz ich rola w całym projekcie.

### main.cpp
Do czego służy:
- Jest to punkt wejścia programu.
- Uruchamia symulację, wczytuje dane z plików, wywołuje logikę symulatora i zapisuje wynik do `wyniki_fifo.txt`.

Główne elementy:
- utworzenie obiektu symulatora,
- odczyt konfiguracji i zleceń,
- uruchomienie pętli symulacji,
- wywołanie zapisu wyników.

Relacje:
- współpracuje z `Symulator.hpp` / `Symulator.cpp`,
- wykorzystuje `Pliki.hpp` / `Pliki.cpp` do odczytu i zapisu danych,
- wykorzystuje struktury z `Dane.hpp` i klasy z `Walidator.hpp`.

### Dane.hpp
Do czego służy:
- Definiuje podstawowe typy danych używane w całym projekcie.
- Zawiera struktury opisujące transakcję, walidatora, konfigurację i wynik wykonania.

Główne elementy:
- `Transakcja`,
- `WalidatorDane` / dane walidatora,
- `Konfiguracja`,
- `WynikTransakcji`,
- pola typu `id`, `nadejscie`, `start`, `koniec`, `deadline`, `fee`, `stake` itp.

Relacje:
- jest wykorzystywany przez `Walidator.cpp`, `Pliki.cpp`, `Symulator.cpp` i `main.cpp`,
- dostarcza wspólnej definicji danych dla całego programu.

### Walidator.hpp
Do czego służy:
- Definiuje interfejs klasy walidatora.
- Określa, jak walidator wykonuje zadania, rejestruje czas i zwalnia się po pracy.

Główne elementy:
- deklaracja klasy `Walidator`,
- metody: rozpoczęcie pracy, zakończenie pracy, zwolnienie, przygotowanie wyniku,
- pola opisujące status walidatora i jego czas wykonania.

Relacje:
- implementacja znajduje się w `Walidator.cpp`,
- jest używany przez `Symulator.cpp` do obsługi wielu walidatorów,
- korzysta z definicji danych z `Dane.hpp`.

### Walidator.cpp
Do czego służy:
- Realizuje logikę działania walidatora.
- Obsługuje rozpoczęcie zlecenia, czas wykonania, zakończenie zadania i zapis wyniku.

Główne elementy:
- metoda wykonania zadania,
- obliczanie czasu startu i końca,
- zapis danych transakcji po zakończeniu,
- zwalnianie walidatora po wykonaniu pracy.

Relacje:
- implementuje deklaracje z `Walidator.hpp`,
- używa struktur z `Dane.hpp`,
- jest używany w `Symulator.cpp` przy symulacji wielu walidatorów.

### Pliki.hpp
Do czego służy:
- Definiuje interfejs do odczytu plików wejściowych i zapisu wyników.
- Odpowiada za komunikację programu z danymi zapisanymi w plikach tekstowych.

Główne elementy:
- deklaracja funkcji odczytu konfiguracyjnych i zleceń,
- deklaracja funkcji zapisu wyników do pliku,
- typy danych wejściowych i wyjściowych.

Relacje:
- implementacja znajduje się w `Pliki.cpp`,
- jest używana przez `main.cpp` oraz `Symulator.cpp`,
- współpracuje ze strukturami z `Dane.hpp`.

### Pliki.cpp
Do czego służy:
- Odczytuje dane z plików `konfiguracja.txt` i `zgloszenia.txt`.
- Zapisuje wyniki symulacji do `wyniki_fifo.txt`.

Główne elementy:
- parsowanie plików tekstowych,
- walidacja danych wejściowych,
- odczyt konfiguracji walidatorów i transakcji,
- zapis wyników w wymaganym formacie.

Relacje:
- realizuje interfejs z `Pliki.hpp`,
- pobiera dane do `Symulator.cpp`,
- zapisuje dane dla `main.cpp`,
- odwołuje się do definicji z `Dane.hpp`.

### Symulator.hpp
Do czego służy:
- Definiuje klasę symulatora.
- Zawiera główną logikę modelowania pracy walidatorów i kolejki zadań.

Główne elementy:
- kolejka zadań,
- lista walidatorów,
- lista wyników symulacji,
- metody do uruchomienia symulacji, obsługi czasu i sprawdzania czy zadania są wykonane.

Relacje:
- implementacja znajduje się w `Symulator.cpp`,
- wykorzystywana przez `main.cpp`,
- współpracuje z `Walidator.hpp`, `Walidator.cpp`, `Dane.hpp` i `Pliki.hpp`.

### Symulator.cpp
Do czego służy:
- Implementuje przebieg symulacji.
- Zajmuje się odpowiednim dobieraniem transakcji z kolejki FIFO, przydziałem zadań wolnym walidatorom i kontrolą czasu.

Główne elementy:
- kolejka FIFO,
- wybór pierwszego wolnego walidatora,
- aktualizacja czasu symulacji,
- zapisanie wyniku transakcji do listy,
- zakończenie wykonania i zwolnienie walidatora.

Relacje:
- implementuje deklaracje z `Symulator.hpp`,
- korzysta z klas `Walidator`,
- wykorzystuje struktury danych z `Dane.hpp`,
- jest wywoływany z `main.cpp`,
- współpracuje z `Pliki.cpp` przy zapisie wyników.

### konfiguracja.txt
Do czego służy:
- Zawiera parametry konfiguracyjne symulacji.
- Określa liczbę walidatorów i ich właściwości wstępne, np. stake, czas pracy lub zasady uruchamiania.

Główne elementy:
- dane walidatorów,
- dane środowiska symulacji,
- ustawienia wejściowe dla programu.

Relacje:
- jest odczytywana przez `Pliki.cpp`,
- jej dane wypełniają struktury z `Dane.hpp`,
- wyniki tej konfiguracji są wykorzystywane przez `Symulator.cpp`.

### zgloszenia.txt
Do czego służy:
- Zawiera listę transakcji wejściowych.
- Określa kolejność nadejścia zleceń, ich opłaty, deadline’y oraz inne parametry wymagane do symulacji.

Główne elementy:
- identyfikatory transakcji,
- czas nadejścia,
- wymagany czas wykonania,
- deadline,
- fee / wartość transakcji.

Relacje:
- jest odczytywana przez `Pliki.cpp`,
- dane są wprowadzane do struktur z `Dane.hpp`,
- to w niej powstają zadania, które trafiają do kolejki w `Symulator.cpp`.

### oczekiwane_wyniki.txt
Do czego służy:
- Jest to plik kontrolny z ręcznie policzonymi wynikami testowymi.
- Służy do porównania z wynikami wygenerowanymi przez program.

Główne elementy:
- opis strategii FIFO i FEE,
- oczekiwane czasy startu i końca zadań,
- oczekiwania i deadline’y,
- wyniki referencyjne do weryfikacji.

Relacje:
- porównywany z `wyniki_fifo.txt`,
- służy jako punkt odniesienia dla poprawności działania programu,
- potwierdza zgodność działania `Symulator.cpp` i `Pliki.cpp`.

### wyniki_fifo.txt
Do czego służy:
- Jest to plik wyjściowy z wynikami wygenerowanymi przez program dla strategii FIFO.
- Zawiera listę przeprowadzonych transakcji wraz z czasem wykonania i statusem terminowości.

Główne elementy:
- `id_transakcji`,
- `id_walidatora`,
- `nadejscie`,
- `start`,
- `koniec`,
- `oczekiwanie`,
- `deadline`,
- `opoznienie`,
- `terminowo`.

Relacje:
- generowany przez `Pliki.cpp` po uruchomieniu programu,
- powstaje na podstawie danych z `zgloszenia.txt` i `konfiguracja.txt`,
- jest porównywany z `oczekiwane_wyniki.txt`,
- odzwierciedla pracę `Symulator.cpp` i `Walidator.cpp`.

### Relacja między plikami w projekcie
Najważniejsze zależności są następujące:
- `main.cpp` uruchamia cały program.
- `Pliki.cpp` odczytuje pliki wejściowe i zapisuje wynik.
- `Dane.hpp` definiuje wspólne struktury danych.
- `Walidator.hpp` i `Walidator.cpp` opisują i implementują zachowanie walidatora.
- `Symulator.hpp` i `Symulator.cpp` zarządzają kolejką zadań i przebiegiem symulacji.
- `wyniki_fifo.txt` jest efektem pracy całego programu i stanowi wynik symulacji dla strategii FIFO.

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
