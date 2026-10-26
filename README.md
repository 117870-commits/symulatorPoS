# Symulator PoS — FIFO i FEE

Projekt w C++17 symuluje obsługę transakcji przez walidatorów. Etap 3: FIFO wybiera najwcześniejsze nadejście (remis: mniejsze ID); FEE wybiera największą opłatę (remis: wcześniejsze nadejście, potem mniejsze ID). Walidator jest losowany spośród wolnych proporcjonalnie do swojego stake. Obie strategie dostają tę samą listę losowań, ale działają na niezależnym stanie.

## Kompilacja i uruchomienie

W folderze projektu:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Walidator.cpp Pliki.cpp Symulator.cpp -o symulator.exe
```

Windows: `.\symulator.exe`; Linux/macOS: `./symulator.exe`. Program odczytuje pliki z bieżącego folderu i nadpisuje `wyniki_fifo.txt` oraz `wyniki_fee.txt`.

## Pliki i format

- `konfiguracja.txt`: sekcja `[symulacja]` z `ziarno` oraz sekcje `[walidator]` z `id` i `stake`.
- `zgloszenia.txt`: sekcje `[transakcja]` z polami `id`, `nadejscie`, `czas_wykonania`, `fee`, `deadline`. Pusty plik oznacza brak transakcji.
- `wyniki_fifo.txt`, `wyniki_fee.txt`: sekcje `[wynik]` z nazwanymi polami, w kolejności zakończenia. `terminowo` ma wartość `tak` lub `nie`.
- `oczekiwane_wyniki.txt`: ręcznie policzony przykład kontrolny; FIFO 1,2,3, FEE 2,3,1.
- `Dane.hpp`, `Walidator.hpp/.cpp`, `Pliki.hpp/.cpp`, `Symulator.hpp/.cpp`, `main.cpp`: dane, wykonawcy, pliki, symulacja i uruchomienie.
- `PLAN_PROJEKTU_POS.md` i `opis_projektu.md`: plan, zasady i zapis postępu. Aktualizacja formatu na początku planu zastępuje wcześniejsze przykłady plików numerycznych.

Składnia jest inspirowana plikami systemd: sekcje, `nazwa = wartość`, puste wiersze i komentarze `#` lub `;`. Program obsługuje wyłącznie opisany podzbiór, nie pełną składnię systemd. Błędne lub brakujące pola, nieznane sekcje, duplikaty i wartości poza zakresem powodują błąd z nazwą pliku i numerem wiersza. Pliki starego formatu liczbowego należy przepisać.

## Testy

```sh
python tests/test_etap3.py
g++ -std=c++17 -Wall -Wextra -pedantic -I. tests/test_losowanie.cpp Walidator.cpp Symulator.cpp -o test_losowanie.exe
./test_losowanie.exe
```

Na Windows ostatnie polecenie to `.\test_losowanie.exe`. Testy obejmują przykład z planu, remisy, dwóch walidatorów, powtarzalność, puste i błędne wejście, granice losowania stake oraz niedostępność zajętych walidatorów. Statystyki zbiorcze i eksperymenty należą do etapu 5.
