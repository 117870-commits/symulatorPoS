# Plan pracy nad symulatorem PoS

Mamy przygotowany schemat działania i diagram klas. Przygotowujemy opis zasad, dane testowe i program w C++.

Przy każdym kroku zapisujemy działania i oczekiwany wynik. Odhaczamy kroki po sprawdzeniu wyniku. Na koniec spotkania notujemy jedną rzecz, od której zaczniemy następnym razem.

## Co budujemy

Użytkownik wysyła transakcję. Transakcja czeka w kolejce, aż zwolni się walidator. Walidator wykonuje jedno zadanie naraz. Porównujemy dwie kolejki: według kolejności nadejścia i według opłaty.

W projekcie korzystamy z uproszczonego pomysłu PoS: większy stake daje większą szansę wyboru wykonawcy. Nie odtwarzamy całego blockchaina ani konsensusu.

| Nazwa | Co oznacza u nas |
|---|---|
| FIFO | Najpierw obsługujemy transakcję, która wcześniej przyszła |
| FEE | Najpierw obsługujemy oczekującą transakcję z najwyższą opłatą |
| Stake | Liczba określająca szansę wyboru walidatora |
| Deadline | Termin, do którego transakcja powinna skończyć obsługę |
| Czas symulacji t | Licznik kroków: 0, 1, 2, 3… |

Przy równym fee wybieramy wcześniejsze nadejście, a potem mniejsze ID. Dla FIFO transakcje z jednakowym czasem nadejścia porządkujemy po ID.

Walidatorzy mają jednakową szybkość. Każdy walidator zachowuje swój stake przez cały przebieg. Walidatorzy mogą mieć różne liczby monet. Rozpoczętej pracy nie przerywamy. Spóźnioną transakcję wykonujemy do końca.

## Etap 3. Dodajemy FEE i losowanie walidatora

Etap 3 jest już zrealizowany i zatwierdzony w repozytorium. Dodatkowo w plikach wejściowych i wynikowych wprowadzono czytelne nagłówki z nazwami kolumn.

### Zrealizowane elementy etapu 3

- dodanie strategii `Strategia::FIFO` i `Strategia::FEE`
- kolejka `std::priority_queue` z komparatorem po fee, następnie nadejściu, potem ID
- wybór wolnego walidatora według stake i ziarna losowania
- uruchomienie dwóch przebiegów: FIFO oraz FEE
- zapis do `wyniki_fifo.txt` i `wyniki_fee.txt`
- pliki tekstowe w formacie czytelnym, z nazwami kolumn i opisami

### Oczekiwany rezultat etapu 3

Dla danych testowych z `konfiguracja.txt` i `zgloszenia.txt` oba przebiegi pracują poprawnie, a w wynikach widać kolumny z nazwami i wartościami opisanymi wprost.

### Uwaga

Etap 3 jest zakończony. Etapy 4, 5 i 6 nie były wykonywane, zgodnie z prośbą ograniczenia do samego etapu 3.
