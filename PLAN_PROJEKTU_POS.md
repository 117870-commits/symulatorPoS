# Plan pracy nad symulatorem PoS

Mamy przygotowany schemat działania i diagram klas. Przygotowujemy opis zasad, dane testowe i program w C++.

Przy każdym kroku zapisujemy działania i oczekiwany wynik. Odhaczamy kroki po sprawdzeniu wyniku. Na koniec spotkania notujemy jedną rzecz, od której zaczniemy następnym razem.

## Aktualizacja formatu plików — etap 3

Od etapu 3 używamy czytelnych plików tekstowych inspirowanych konfiguracją systemd: sekcje `[nazwa]`, pola `nazwa = wartość` oraz komentarze zaczynające się od `#` lub `;`. To uproszczony format projektu, a nie pełny parser systemd. Puste wiersze i komentarze są pomijane, również po wartości. Nie używamy już liczby rekordów w nagłówku: liczymy sekcje. Stare przykłady samych liczb oraz kod parsera i zapisu w punktach 1, 2.3 i 2.5 poniżej zachowano jako historyczne instrukcje etapu 2 — w aktualnej wersji zastępują je zasady opisane tutaj i kod `Pliki.cpp`. Nie kopiuj ich jako aktualnego formatu. Pozostałe zasady algorytmów pozostają bez zmian.

### Aktualna konfiguracja

`konfiguracja.txt` ma jedną sekcję `[symulacja]` z polem `ziarno` oraz jedną lub więcej sekcji `[walidator]` z polami `id` i `stake`:

```ini
[symulacja]
ziarno = 12345
[walidator]
id = 1
stake = 10
```

`zgloszenia.txt` zawiera zero lub więcej sekcji `[transakcja]`; każda ma pięć nazwanych pól:

```ini
[transakcja]
id = 1
nadejscie = 0
czas_wykonania = 2
fee = 1
deadline = 10
```

Program sprawdza wymagane pola, zakresy, nieznane lub powtórzone nazwy, powtórzone ID i nieznane sekcje. Błąd podaje plik i numer wiersza. Pusta lista transakcji to pusty plik lub same komentarze. Starszy format liczbowy nie jest obsługiwany. Walidatorów i transakcje sortujemy jak w pierwotnym planie.

### Aktualny zapis wyników

`wyniki_fifo.txt` i `wyniki_fee.txt` zawierają po jednej sekcji `[wynik]` na zakończoną transakcję. Każda sekcja podaje `id_transakcji`, `id_walidatora`, `nadejscie`, `start`, `koniec`, `oczekiwanie`, `deadline`, `opoznienie` i `terminowo = tak/nie`. Sekcje są w kolejności zakończenia; przy jednoczesnym zakończeniu kolejność wynika z ID walidatora, więc nie musi być kolejnością przydziałów. Przy pustym wejściu zostaje komentarz bez sekcji wyników. Podsumowanie zbiorcze nadal należy do etapu 5. `oczekiwane_wyniki.txt` opisuje ręcznie policzony przykład nazwanymi polami.

### Sprawdzenie etapu 3

- Kompilujemy zgodnie z README i uruchamiamy `python tests/test_etap3.py`.
- Sprawdzamy FIFO 1,2,3 i FEE 2,3,1, remisy, dwa walidatory, brak nakładania pracy, powtarzalność, puste wejście i odrzucanie błędnych danych.
- Oba przebiegi dostają tę samą listę N losowań; każda udana decyzja zużywa jedno losowanie, a zajęci walidatorzy nie biorą udziału.
- Wypełniamy zapis postępu i wyników w `opis_projektu.md`.

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

Walidatorzy mają jednakową szybkość. Każdy walidator zachowuje swój stake przez cały przebieg. Walidatorzy mogą mieć różne liczby monet. Rozpoczętej pracy nie przerywamy. Spóźnioną transakcję kończymy i zaznaczamy opóźnienie w wyniku.

## Jak korzystamy z instrukcji

Idziemy od etapu 1 do 6. Kod wpisujemy w tym etapie, w którym dodajemy daną funkcję programu. Każdy fragment ma od 1 do 6 linii. Pod kodem wyjaśniamy zapis C++ i jego rolę w symulatorze.

Gdy piszemy „nowy plik”, tworzymy go na zajęciach. Gdy piszemy „dopisujemy”, dodajemy tylko pokazane linie w podanym miejscu. Gdy piszemy „zastępujemy funkcję”, usuwamy starą definicję od jej nazwy do zamykającej klamry i wpisujemy nową. Nie zostawiamy dwóch definicji tej samej metody. Podczas składania otwartej funkcji kilka fragmentów może jeszcze się nie kompilować; kompilujemy po zakończeniu wskazanego przystanku.

Teraz korzystamy z tej instrukcji; pliki C++ tworzymy sami później na lekcjach. README, wyniki programu i materiały do oddania również tworzymy dopiero w odpowiednich etapach.

| Etap | Co dopisujemy w programie | Co działa na końcu |
|---|---|---|
| 1 | Opis i wejście, bez kodu C++ | Mamy zasady i ręcznie policzony przykład |
| 2 | Dane, Walidator, Pliki, Symulator i main | Wczytujemy dane, obsługujemy FIFO przez pierwszego wolnego walidatora, zapisujemy wyniki |
| 3 | Strategia, priority_queue, komparator i losowanie stake | Porównujemy FIFO oraz FEE z tymi samymi losowaniami |
| 4 | Opis kosztów, bez nowej funkcji programu | Uzasadniamy czas działania i pamięć |
| 5 | Podsumowanie i Statystyki; rozszerzamy zapis i main | Zapisujemy pomiary, robimy eksperymenty i raport |
| 6 | Dokumenty do oddania, bez nowej funkcji programu | Mamy komplet plików i przećwiczony pokaz |

Docelowo mamy dziesięć plików kodu. W etapie 2 tworzymy osiem: Dane.hpp, Walidator.hpp/.cpp, Pliki.hpp/.cpp, Symulator.hpp/.cpp i main.cpp. W etapie 5 dokładamy Statystyki.hpp/.cpp. W etapie 3 rozbudowujemy istniejące pliki.

## Podstawy zapisu C++, z których korzystamy

| Zapis | Jak go czytamy |
|---|---|
| `int`, `long long`, `double`, `bool` | Liczba całkowita, większa liczba całkowita, liczba z ułamkiem, true/false |
| `std::` | Korzystamy z nazw ze standardowej biblioteki C++ |
| `.hpp` i `.cpp` | W nagłówku deklarujemy dostępne dane i metody; w .cpp definiujemy działanie metod |
| `struct` i `class` | Rekord danych oraz obiekt z metodami; pola struct są domyślnie publiczne, class prywatne |
| `void` | Funkcja niczego nie zwraca |
| `const T&` | Odczytujemy istniejący obiekt T bez kopiowania i zmieniania |
| `metoda() const` | Metoda odczytuje obiekt i nie zmienia jego pól |
| `static` przy metodzie | Wywołujemy np. Pliki::wczytajTransakcje bez tworzenia obiektu Pliki |
| `vector<T>` i `size_t` | Lista elementów typu T oraz typ do rozmiarów i indeksów list |
| `if`, `while`, `for` | Warunek, powtarzanie dopóki warunek jest prawdziwy, przejście przez listę lub zakres |
| `&&`, „lub”, `!`, `==`, `!=` | I, lub, nie, równe, różne; pojedyncze = przypisuje wartość |
| `return`, `break`, `++` | Oddajemy wynik funkcji, kończymy pętlę, zwiększamy liczbę o 1 |

Przykłady kodu wpisujemy bez komentarzy. Objaśnienia czytamy pod fragmentami. Przy nowym pliku .hpp dodajemy pragma once; do polecenia kompilacji wpisujemy pliki .cpp, ponieważ kompilator wczytuje nagłówki przez include.

Zapis `||` czytamy jako „lub”, a `&&` jako „i”. W `auto wiersz = wczytajWiersz(...)` nie wpisujemy nazwy typu ręcznie: kompilator ustala ją z wyniku funkcji. Przez `lista.size()` odczytujemy liczbę elementów, a `lista.empty()` sprawdzamy, czy jest pusta. Indeks pierwszego elementu wynosi 0.

## Etap 1. Opisujemy zasady i przygotowujemy pliki

Zadanie 1 z polecenia prowadzącego. Na ten etap rezerwujemy około 3–4 godzin pracy w parze.

### 1.1. Zapisujemy opis projektu

- [ ] Tworzymy `opis_projektu.md`.
- [ ] Piszemy, co robi transakcja, kolejka i walidator.
- [ ] Zapisujemy zasady z sekcji „Co budujemy”.

**Wynik:** krótki opis, do którego możemy zajrzeć podczas pisania kodu.

### 1.2. Ustalamy dane transakcji i walidatora

- [ ] Dla transakcji zapisujemy: ID, nadejście, czas wykonania, fee i deadline.
- [ ] Dla walidatora zapisujemy: ID, stake, wykonywane zadanie i czas jego zakończenia.
- [ ] Oglądamy diagram klas. Dopisujemy do opisu po jednym zdaniu o zadaniu każdej klasy.

**Wynik:** część „Dane i klasy” w `opis_projektu.md`. Wiemy, co przechowujemy w programie.

### 1.3. Ustalamy format wejścia

- [ ] Tworzymy `konfiguracja.txt` i `zgloszenia.txt`.
- [ ] Korzystamy z przykładów w części „Pliki wejściowe i przykład kontrolny” na końcu planu.
- [ ] W opisie wyjaśniamy znaczenie kolumn.

**Wynik:** dwa pliki, które później wczytamy w C++.

### 1.4. Liczymy mały przykład na kartce

- [ ] Przyjmujemy jednego walidatora i trzy transakcje przychodzące w czasie 0.
- [ ] Nadajemy im fee: 1, 9, 4. Niech każda trwa 2 jednostki czasu.
- [ ] Zapisujemy kolejność i czasy startu/końca dla FIFO oraz FEE.

**Wynik:** `oczekiwane_wyniki.txt`, z którym porównamy program. Przykład podaliśmy na końcu planu.

### 1.5. Sprawdzamy opis we dwoje

- [ ] Jedna osoba wyjaśnia przykład drugiej.
- [ ] Uzupełniamy zasady, o które trzeba było dopytać.
- [ ] Ustalamy, kto zaczyna od plików i FIFO, a kto od statystyk i kolejki FEE.

**Na koniec etapu:** opis, dwa pliki wejściowe i wyniki policzone na kartce.

## Etap 2. Piszemy pierwszą wersję programu z FIFO

Budujemy pierwszy działający program. Czytamy pliki, wpuszczamy transakcje zgodnie z czasem nadejścia, obsługujemy FIFO i zapisujemy wyniki. Przydzielamy pierwszego wolnego walidatora według ID. FEE i losowanie dopisujemy w etapie 3, a zbiorcze statystyki w etapie 5.

### 2.1. Uruchamiamy program C++

#### 2.1.1. Zaczynamy od main

**Plik:** `main.cpp`. **Co robimy:** nowy plik.

`main` jest miejscem rozpoczęcia programu. Na razie sprawdzamy kompilator i wypisujemy jeden komunikat.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#include <iostream>
int main() {
    std::cout << "Symulator transakcji\n";
    return 0;
}
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++. Przez `cout <<` wypisujemy dane na ekran. Spacje rozdzielają wartości, a `\n` kończy wiersz; `cerr` służy do komunikatów o błędach. W main przez return 0 kończymy program i oznaczamy, że zakończył się poprawnie.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

**Przystanek: pierwsze uruchomienie.** Kończymy podane definicje, zapisujemy pliki i kompilujemy w folderze z kodem.

```text
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o symulator.exe
.\symulator.exe
```

**Oczekiwany wynik:** Widzimy `Symulator transakcji`. W README.md zapisujemy to polecenie i informację, że korzystamy z C++17. Jeśli kompilator zgłasza błąd, poprawiamy pierwszy komunikat przed następnym krokiem.

### 2.2. Przygotowujemy dane i wykonawcę

#### 2.2.1. Tworzymy rekordy danych

**Plik:** `Dane.hpp`. **Co robimy:** nowy plik.

Tworzymy Transakcja, DaneWalidatora, Konfiguracja i WynikTransakcji. Konfiguracja zawiera listę początkowych danych wykonawców. WynikTransakcji przechowuje efekt jednej zakończonej pracy. Ziarno, fee i deadline odczytujemy już teraz, chociaż pełne porównanie strategii dodamy później. Podsumowanie dopiszemy w etapie 5.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#pragma once
```

Przez `#pragma once` prosimy kompilator, żeby dołączył ten nagłówek tylko raz w danym pliku .cpp.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
#include <vector>
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Krok 3.** Dopisujemy pod poprzednim fragmentem.

```cpp
struct Transakcja {
    int id = 0;
    long long nadejscie = 0;
    long long czasWykonania = 0;
```

Zaczynamy rekord `Transakcja`. Między jego klamrami wpisujemy pola, które przechowujemy razem. Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki.

**Krok 4.** Dopisujemy pod poprzednim fragmentem.

```cpp
    long long fee = 0;
    long long deadline = 0;
};
```

Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki.

**Krok 5.** Dopisujemy pod poprzednim fragmentem.

```cpp
struct DaneWalidatora {
    int id = 0;
    long long stake = 0;
};
```

Zaczynamy rekord `DaneWalidatora`. Między jego klamrami wpisujemy pola, które przechowujemy razem. Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki.

**Krok 6.** Dopisujemy pod poprzednim fragmentem.

```cpp
struct Konfiguracja {
    unsigned int ziarno = 0;
    std::vector<DaneWalidatora> walidatorzy;
};
```

Zaczynamy rekord `Konfiguracja`. Między jego klamrami wpisujemy pola, które przechowujemy razem. Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać.

**Krok 7.** Dopisujemy pod poprzednim fragmentem.

```cpp
struct WynikTransakcji {
    int idTransakcji = 0;
    int idWalidatora = 0;
    long long nadejscie = 0;
    long long start = 0;
    long long koniec = 0;
```

Zaczynamy rekord `WynikTransakcji`. Między jego klamrami wpisujemy pola, które przechowujemy razem. Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki.

**Krok 8.** Dopisujemy pod poprzednim fragmentem.

```cpp
    long long oczekiwanie = 0;
    long long deadline = 0;
    long long opoznienie = 0;
    bool terminowo = false;
};
```

Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.2.2. Deklarujemy klasę Walidator

**Plik:** `Walidator.hpp`. **Co robimy:** nowy plik.

Konstruktor tworzy wolnego wykonawcę z ID i stake. Metody pobierzId/pobierzStake/czyWolny/czyKonczy służą do odczytu. Rozpocznij zmienia stan na zajęty, a zakoncz zwraca wynik i zwalnia wykonawcę. W nagłówku podajemy nazwy, typy parametrów i zwracane wartości; ciała metod wpisujemy w .cpp.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#pragma once
```

Przez `#pragma once` prosimy kompilator, żeby dołączył ten nagłówek tylko raz w danym pliku .cpp.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
#include "Dane.hpp"
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Krok 3.** Dopisujemy pod poprzednim fragmentem.

```cpp
class Walidator {
public:
    explicit Walidator(const DaneWalidatora& dane);
    int pobierzId() const;
    long long pobierzStake() const;
    bool czyWolny() const;
```

Zaczynamy klasę. Łączymy w niej dane obiektu z metodami, przez które z niego korzystamy. Pod `public:` wpisujemy metody dostępne z innych części programu. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Przy `explicit` tworzymy walidatora przez jawne wywołanie konstruktora, np. `Walidator(dane)`.

**Krok 4.** Dopisujemy pod poprzednim fragmentem.

```cpp
    bool czyKonczy(long long czas) const;
    void rozpocznij(const Transakcja& transakcja, long long czas);
    WynikTransakcji zakoncz();
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania.

**Krok 5.** Dopisujemy pod poprzednim fragmentem.

```cpp
private:
    int id;
    long long stake;
    bool zajety = false;
    Transakcja aktualna;
    long long start = 0;
```

Pod `private:` trzymamy stan obiektu. Zmieniamy go przez metody, żeby nie nadpisać go przypadkiem w main. Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki.

**Krok 6.** Dopisujemy pod poprzednim fragmentem.

```cpp
    long long koniec = 0;
};
```

Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.2.3. Definiujemy metody Walidator

**Plik:** `Walidator.cpp`. **Co robimy:** nowy plik.

Zapis Walidator::nazwa oznacza metodę tej klasy. Sam Walidator::Walidator to konstruktor. W rozpocznij obliczamy koniec = start + czasWykonania. W zakoncz tworzymy wynik pojedynczej transakcji, w tym jej oczekiwanie i terminowość. Zbiorcze pomiary całego przebiegu dodamy dopiero w etapie 5.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#include "Walidator.hpp"
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
#include <algorithm>
#include <limits>
#include <stdexcept>
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Krok 3.** Dopisujemy pod poprzednim fragmentem.

```cpp
Walidator::Walidator(const DaneWalidatora& dane)
    : id(dane.id), stake(dane.stake) {
}
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Po dwukropku ustawiamy pola obiektu z danych wejściowych. To lista inicjalizacyjna konstruktora.

**Krok 4.** Dopisujemy pod poprzednim fragmentem.

```cpp
int Walidator::pobierzId() const {
    return id;
}
```

Przez return oddajemy wartość wskazanego pola osobie wywołującej metodę. Metoda z const na końcu nagłówka odczytuje stan, bez zmieniania go.

**Krok 5.** Dopisujemy pod poprzednim fragmentem.

```cpp
long long Walidator::pobierzStake() const {
    return stake;
}
```

Przez return oddajemy wartość wskazanego pola osobie wywołującej metodę. Metoda z const na końcu nagłówka odczytuje stan, bez zmieniania go.

**Krok 6.** Dopisujemy pod poprzednim fragmentem.

```cpp
bool Walidator::czyWolny() const {
    return !zajety;
}
```

Znak `!` odwraca wartość bool. Gdy zajety ma false, zwracamy true: walidator jest wolny.

**Krok 7.** Dopisujemy pod poprzednim fragmentem.

```cpp
bool Walidator::czyKonczy(long long czas) const {
    return zajety && koniec == czas;
}
```

Zwracamy true tylko wtedy, gdy walidator jest zajęty i jego zadanie kończy się w podanym czasie.

**Krok 8.** Dopisujemy pod poprzednim fragmentem.

```cpp
void Walidator::rozpocznij(const Transakcja& transakcja, long long czas) {
    if (zajety) {
        throw std::logic_error("Walidator jest zajety");
    }
    if (transakcja.czasWykonania > std::numeric_limits<long long>::max() - czas) {
        throw std::overflow_error("Czas zakonczenia przekracza zakres liczby");
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Przez `throw` przerywamy tę operację i przekazujemy opis błędu do `catch` w main. Nie pracujemy dalej na niepoprawnych danych. Z biblioteki odczytujemy największą dozwoloną wartość typu. Sprawdzamy zakres liczby przed konwersją albo dodaniem czasu.

**Krok 9.** Dopisujemy pod poprzednim fragmentem.

```cpp
    }
    aktualna = transakcja;
    start = czas;
    koniec = czas + transakcja.czasWykonania;
    zajety = true;
}
```

Zapamiętujemy wykonywaną transakcję, czas startu i koniec równy startowi plus czas wykonania. Ustawiamy zajety na true.

**Krok 10.** Dopisujemy pod poprzednim fragmentem.

```cpp
WynikTransakcji Walidator::zakoncz() {
    if (!zajety) {
        throw std::logic_error("Walidator nie ma zadania");
    }
    WynikTransakcji wynik;
    wynik.idTransakcji = aktualna.id;
```

Przez `throw` przerywamy tę operację i przekazujemy opis błędu do `catch` w main. Nie pracujemy dalej na niepoprawnych danych. Przepisujemy identyfikatory i czasy do rekordu wyniku. Nie zapisujemy go dla zadania, które jeszcze trwa.

**Krok 11.** Dopisujemy pod poprzednim fragmentem.

```cpp
    wynik.idWalidatora = id;
    wynik.nadejscie = aktualna.nadejscie;
    wynik.start = start;
    wynik.koniec = koniec;
    wynik.oczekiwanie = start - aktualna.nadejscie;
    wynik.deadline = aktualna.deadline;
```

Liczymy oczekiwanie jako start minus nadejście. Dla nadejścia 0 i startu 0 otrzymujemy 0, choć sama praca może trwać kilka kroków.

**Krok 12.** Dopisujemy pod poprzednim fragmentem.

```cpp
    wynik.opoznienie = std::max(0LL, koniec - aktualna.deadline);
    wynik.terminowo = koniec <= aktualna.deadline;
    zajety = false;
    return wynik;
}
```

Liczymy spóźnienie, wybierając większą wartość: 0 lub koniec minus deadline. `0LL` oznacza zero typu long long. Porównaniem `<=` zaliczamy także zakończenie dokładnie w deadline jako terminowe.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.2.4. Sprawdzamy jednego wykonawcę

**Plik:** `main.cpp`. **Co robimy:** zastępujemy cały dotychczasowy main.cpp testem.

Tworzymy jeden obiekt i jedno zadanie. Zapis {1, 0, 2, 1, 10} wypełnia pola Transakcja w kolejności z Dane.hpp. To test klasy: wywołujemy zakoncz po potwierdzeniu czyKonczy(2). Upływ czasu zbudujemy w symulatorze.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#include "Walidator.hpp"
#include <iostream>
int main() {
    Walidator w(DaneWalidatora{1, 10});
    Transakcja t{1, 0, 2, 1, 10};
    w.rozpocznij(t, 0);
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
    std::cout << w.czyWolny() << ' ' << w.czyKonczy(2) << '\n';
    WynikTransakcji wynik = w.zakoncz();
    std::cout << wynik.start << ' ' << wynik.koniec << ' '
              << wynik.oczekiwanie << ' ' << w.czyWolny() << '\n';
    return 0;
}
```

Przez `cout <<` wypisujemy dane na ekran. Spacje rozdzielają wartości, a `\n` kończy wiersz; `cerr` służy do komunikatów o błędach. W main przez return 0 kończymy program i oznaczamy, że zakończył się poprawnie.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

**Przystanek: walidator.** Kończymy podane definicje, zapisujemy pliki i kompilujemy w folderze z kodem.

```text
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Walidator.cpp -o symulator.exe
.\symulator.exe
```

**Oczekiwany wynik:** Pierwszy wiersz: `0 1`, czyli zajęty i kończący w 2. Drugi: `0 2 0 1`, czyli start 0, koniec 2, oczekiwanie 0, znów wolny. Jeśli kompilator zgłasza błąd, poprawiamy pierwszy komunikat przed następnym krokiem.

#### 2.2.5. Deklarujemy odczyt i zapis

**Plik:** `Pliki.hpp`. **Co robimy:** nowy plik.

WczytajKonfiguracje zwraca Konfiguracja. WczytajTransakcje zwraca listę Transakcja. ZapiszWyniki przyjmuje nazwę i listę wyników. ZapiszWyniki zdefiniujemy w 2.5; w 2.3 sprawdzimy najpierw sam odczyt.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#pragma once
```

Przez `#pragma once` prosimy kompilator, żeby dołączył ten nagłówek tylko raz w danym pliku .cpp.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
#include "Dane.hpp"
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Krok 3.** Dopisujemy pod poprzednim fragmentem.

```cpp
#include <string>
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Krok 4.** Dopisujemy pod poprzednim fragmentem.

```cpp
class Pliki {
public:
    static Konfiguracja wczytajKonfiguracje(const std::string& nazwa);
    static std::vector<Transakcja> wczytajTransakcje(const std::string& nazwa);
```

Zaczynamy klasę. Łączymy w niej dane obiektu z metodami, przez które z niego korzystamy. Pod `public:` wpisujemy metody dostępne z innych części programu. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać.

**Krok 5.** Dopisujemy pod poprzednim fragmentem.

```cpp
    static void zapiszWyniki(const std::string& nazwa,
                            const std::vector<WynikTransakcji>& wyniki);
};
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

### 2.3. Wczytujemy, sprawdzamy i sortujemy dane

#### 2.3.1. Piszemy odczyt wierszy i rekordów

**Plik:** `Pliki.cpp`. **Co robimy:** nowy plik.

Zaczynamy od czterech pomocników. Blad zgłasza nazwę pliku, numer wiersza i przyczynę. WczytajWiersz daje nam tekst jednego wiersza jako strumień liczb. SprawdzKoniecWiersza wykrywa dodatkowe kolumny, a sprawdzKoniecPliku nadmiarowe rekordy. Potem definiujemy dwa odczyty. Sortujemy po ID, żeby sprawdzić powtórzenia; na końcu transakcje sortujemy po nadejściu i ID. Rekordy wejściowe muszą być kompletne, zanim zwrócimy listę.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#include "Pliki.hpp"
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
#include <algorithm>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Krok 3.** Dopisujemy pod poprzednim fragmentem.

```cpp
namespace {
void blad(const std::string& nazwa, int numer, const std::string& powod) {
    throw std::runtime_error(nazwa + " (wiersz " + std::to_string(numer)
                             + "): " + powod);
}
```

W tej przestrzeni nazw umieszczamy pomocnicze funkcje używane tylko w Pliki.cpp. Po ostatniej z nich zamykamy przestrzeń osobną klamrą. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Przez `throw` przerywamy tę operację i przekazujemy opis błędu do `catch` w main. Nie pracujemy dalej na niepoprawnych danych.

**Krok 4.** Dopisujemy pod poprzednim fragmentem.

```cpp
std::istringstream wczytajWiersz(std::ifstream& plik,
                               const std::string& nazwa, int numer) {
    std::string linia;
    if (!std::getline(plik, linia)) {
        blad(nazwa, numer, "brak wymaganego wiersza");
    }
```

Używamy `istringstream`, żeby czytać liczby z jednego wiersza tekstu. Dzięki temu wykrywamy brakujące i nadmiarowe kolumny w konkretnym wierszu. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Odczytujemy jeden cały wiersz tekstu. Brak wymaganego wiersza traktujemy jako błąd, a nie jako pusty rekord.

**Krok 5.** Dopisujemy pod poprzednim fragmentem.

```cpp
    return std::istringstream(linia);
}
```

Używamy `istringstream`, żeby czytać liczby z jednego wiersza tekstu. Dzięki temu wykrywamy brakujące i nadmiarowe kolumny w konkretnym wierszu.

**Krok 6.** Dopisujemy pod poprzednim fragmentem.

```cpp
void sprawdzKoniecWiersza(std::istringstream& wiersz,
                         const std::string& nazwa, int numer) {
    std::string nadmiar;
    if (wiersz >> nadmiar) {
```

Używamy `istringstream`, żeby czytać liczby z jednego wiersza tekstu. Dzięki temu wykrywamy brakujące i nadmiarowe kolumny w konkretnym wierszu. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Operatorem `>>` odczytujemy kolejne wartości. Warunek `if (!(...))` wykrywa, że którejś oczekiwanej liczby nie udało się odczytać.

**Krok 7.** Dopisujemy pod poprzednim fragmentem.

```cpp
        blad(nazwa, numer, "za duzo danych w wierszu");
    }
}
```

Dopisujemy tę część do rozpoczętej wcześniej definicji. Wcięcia pokazują, które instrukcje należą do tego samego bloku.

**Krok 8.** Dopisujemy pod poprzednim fragmentem.

```cpp
void sprawdzKoniecPliku(std::ifstream& plik,
                       const std::string& nazwa, int numer) {
    std::string linia;
    while (std::getline(plik, linia)) {
        std::istringstream wiersz(linia);
        std::string nadmiar;
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Odczytujemy jeden cały wiersz tekstu. Brak wymaganego wiersza traktujemy jako błąd, a nie jako pusty rekord. Używamy `istringstream`, żeby czytać liczby z jednego wiersza tekstu. Dzięki temu wykrywamy brakujące i nadmiarowe kolumny w konkretnym wierszu.

**Krok 9.** Dopisujemy pod poprzednim fragmentem.

```cpp
        if (wiersz >> nadmiar) {
            blad(nazwa, numer, "nadmiarowe dane po ostatnim rekordzie");
        }
        ++numer;
```

Operatorem `>>` odczytujemy kolejne wartości. Warunek `if (!(...))` wykrywa, że którejś oczekiwanej liczby nie udało się odczytać.

**Krok 10.** Dopisujemy pod poprzednim fragmentem.

```cpp
    }
}
}
```

Zamykamy otwarte wcześniej bloki. Po definicji klasy lub struktury stawiamy `};`, a po ciele funkcji samo `}`.

**Krok 11.** Dopisujemy pod poprzednim fragmentem.

```cpp
Konfiguracja Pliki::wczytajKonfiguracje(const std::string& nazwa) {
    std::ifstream plik(nazwa);
    if (!plik) {
        throw std::runtime_error("Nie mozna otworzyc " + nazwa);
    }
    auto naglowek = wczytajWiersz(plik, nazwa, 1);
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Otwieramy plik do odczytu. `if (!plik)` sprawdza, czy otwarcie się nie udało. Przez `throw` przerywamy tę operację i przekazujemy opis błędu do `catch` w main. Nie pracujemy dalej na niepoprawnych danych. Przez `auto` prosimy kompilator o ustalenie typu z wartości po znaku =. Tutaj otrzymujemy istringstream zwrócony przez wczytajWiersz.

**Krok 12.** Dopisujemy pod poprzednim fragmentem.

```cpp
    int liczba = 0;
    long long ziarno = 0;
    if (!(naglowek >> liczba >> ziarno)) {
        blad(nazwa, 1, "oczekujemy liczby walidatorow i ziarna");
    }
    sprawdzKoniecWiersza(naglowek, nazwa, 1);
```

Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki. Operatorem `>>` odczytujemy kolejne wartości. Warunek `if (!(...))` wykrywa, że którejś oczekiwanej liczby nie udało się odczytać.

**Krok 13.** Dopisujemy pod poprzednim fragmentem.

```cpp
    if (liczba < 1 || ziarno < 0
        || static_cast<unsigned long long>(ziarno)
           > std::numeric_limits<unsigned int>::max()) {
        blad(nazwa, 1, "nieprawidlowa liczba walidatorow lub ziarno");
    }
    Konfiguracja konfiguracja;
```

Zapis `||` czytamy jako „lub”: wystarczy jeden prawdziwy warunek. Przy sprawdzaniu błędów odrzucamy rekord, gdy choć jedno ograniczenie zostało złamane. Przez `static_cast<typ>` zapisujemy jawną konwersję liczby na podany typ. Przykładowo zamieniamy indeks `size_t` na zwracany `int`. Z biblioteki odczytujemy największą dozwoloną wartość typu. Sprawdzamy zakres liczby przed konwersją albo dodaniem czasu.

**Krok 14.** Dopisujemy pod poprzednim fragmentem.

```cpp
    konfiguracja.ziarno = static_cast<unsigned int>(ziarno);
    for (int i = 0; i < liczba; ++i) {
        auto wiersz = wczytajWiersz(plik, nazwa, i + 2);
        DaneWalidatora dane;
        if (!(wiersz >> dane.id >> dane.stake)) {
            blad(nazwa, i + 2, "oczekujemy ID i stake");
```

Przez `static_cast<typ>` zapisujemy jawną konwersję liczby na podany typ. Przykładowo zamieniamy indeks `size_t` na zwracany `int`. Powtarzamy odczyt tyle razy, ile rekordów zapowiedziano w pierwszym wierszu. Numer wiersza danych to `i + 2`. Przez `auto` prosimy kompilator o ustalenie typu z wartości po znaku =. Tutaj otrzymujemy istringstream zwrócony przez wczytajWiersz. Operatorem `>>` odczytujemy kolejne wartości. Warunek `if (!(...))` wykrywa, że którejś oczekiwanej liczby nie udało się odczytać.

**Krok 15.** Dopisujemy pod poprzednim fragmentem.

```cpp
        }
        sprawdzKoniecWiersza(wiersz, nazwa, i + 2);
        if (dane.id <= 0 || dane.stake <= 0) {
            blad(nazwa, i + 2, "ID i stake musza byc dodatnie");
        }
        konfiguracja.walidatorzy.push_back(dane);
```

Zapis `||` czytamy jako „lub”: wystarczy jeden prawdziwy warunek. Przy sprawdzaniu błędów odrzucamy rekord, gdy choć jedno ograniczenie zostało złamane. Przez `push_back` dopisujemy element na koniec listy.

**Krok 16.** Dopisujemy pod poprzednim fragmentem.

```cpp
    }
    sprawdzKoniecPliku(plik, nazwa, liczba + 2);
    std::sort(konfiguracja.walidatorzy.begin(), konfiguracja.walidatorzy.end(),
              [](const DaneWalidatora& a, const DaneWalidatora& b) {
                  return a.id < b.id;
              });
```

Sortujemy zakres od `begin()` do `end()`. W następnych liniach podajemy regułę, która mówi, kiedy element a ma być przed b. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Zapis `[](...) { ... }` to krótka funkcja, czyli lambda. Tu porównujemy dwa rekordy bez tworzenia osobnej nazwanej funkcji.

**Krok 17.** Dopisujemy pod poprzednim fragmentem.

```cpp
    for (std::size_t i = 1; i < konfiguracja.walidatorzy.size(); ++i) {
        if (konfiguracja.walidatorzy[i - 1].id == konfiguracja.walidatorzy[i].id) {
            throw std::runtime_error(nazwa + ": powtorzone ID walidatora");
        }
```

Zaczynamy od indeksu 1, żeby móc porównać rekord z poprzednim. Po sortowaniu jednakowe ID będą obok siebie. Przez `throw` przerywamy tę operację i przekazujemy opis błędu do `catch` w main. Nie pracujemy dalej na niepoprawnych danych.

**Krok 18.** Dopisujemy pod poprzednim fragmentem.

```cpp
    }
    return konfiguracja;
}
```

Dopisujemy tę część do rozpoczętej wcześniej definicji. Wcięcia pokazują, które instrukcje należą do tego samego bloku.

**Krok 19.** Dopisujemy pod poprzednim fragmentem.

```cpp
std::vector<Transakcja> Pliki::wczytajTransakcje(const std::string& nazwa) {
    std::ifstream plik(nazwa);
    if (!plik) {
        throw std::runtime_error("Nie mozna otworzyc " + nazwa);
    }
    auto naglowek = wczytajWiersz(plik, nazwa, 1);
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Otwieramy plik do odczytu. `if (!plik)` sprawdza, czy otwarcie się nie udało. Przez `throw` przerywamy tę operację i przekazujemy opis błędu do `catch` w main. Nie pracujemy dalej na niepoprawnych danych.

**Krok 20.** Dopisujemy pod poprzednim fragmentem.

```cpp
    int liczba = 0;
    if (!(naglowek >> liczba)) {
        blad(nazwa, 1, "oczekujemy liczby transakcji");
    }
    sprawdzKoniecWiersza(naglowek, nazwa, 1);
    if (liczba < 0) {
```

Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki. Operatorem `>>` odczytujemy kolejne wartości. Warunek `if (!(...))` wykrywa, że którejś oczekiwanej liczby nie udało się odczytać.

**Krok 21.** Dopisujemy pod poprzednim fragmentem.

```cpp
        blad(nazwa, 1, "liczba transakcji nie moze byc ujemna");
    }
    std::vector<Transakcja> transakcje;
    for (int i = 0; i < liczba; ++i) {
        auto wiersz = wczytajWiersz(plik, nazwa, i + 2);
        Transakcja transakcja;
```

W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Powtarzamy odczyt tyle razy, ile rekordów zapowiedziano w pierwszym wierszu. Numer wiersza danych to `i + 2`. Przez `auto` prosimy kompilator o ustalenie typu z wartości po znaku =. Tutaj otrzymujemy istringstream zwrócony przez wczytajWiersz.

**Krok 22.** Dopisujemy pod poprzednim fragmentem.

```cpp
        if (!(wiersz >> transakcja.id >> transakcja.nadejscie
              >> transakcja.czasWykonania >> transakcja.fee >> transakcja.deadline)) {
            blad(nazwa, i + 2, "oczekujemy pieciu liczb transakcji");
        }
        sprawdzKoniecWiersza(wiersz, nazwa, i + 2);
        if (transakcja.id <= 0 || transakcja.nadejscie < 0
```

Operatorem `>>` odczytujemy kolejne wartości. Warunek `if (!(...))` wykrywa, że którejś oczekiwanej liczby nie udało się odczytać. Zapis `||` czytamy jako „lub”: wystarczy jeden prawdziwy warunek. Przy sprawdzaniu błędów odrzucamy rekord, gdy choć jedno ograniczenie zostało złamane.

**Krok 23.** Dopisujemy pod poprzednim fragmentem.

```cpp
            || transakcja.czasWykonania <= 0 || transakcja.fee < 0
            || transakcja.deadline < transakcja.nadejscie) {
            blad(nazwa, i + 2, "nieprawidlowe wartosci transakcji");
        }
        transakcje.push_back(transakcja);
    }
```

Zapis `||` czytamy jako „lub”: wystarczy jeden prawdziwy warunek. Przy sprawdzaniu błędów odrzucamy rekord, gdy choć jedno ograniczenie zostało złamane. Przez `push_back` dopisujemy element na koniec listy.

**Krok 24.** Dopisujemy pod poprzednim fragmentem.

```cpp
    sprawdzKoniecPliku(plik, nazwa, liczba + 2);
    std::sort(transakcje.begin(), transakcje.end(),
              [](const Transakcja& a, const Transakcja& b) {
                  return a.id < b.id;
              });
    for (std::size_t i = 1; i < transakcje.size(); ++i) {
```

Sortujemy zakres od `begin()` do `end()`. W następnych liniach podajemy regułę, która mówi, kiedy element a ma być przed b. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Zapis `[](...) { ... }` to krótka funkcja, czyli lambda. Tu porównujemy dwa rekordy bez tworzenia osobnej nazwanej funkcji. Zaczynamy od indeksu 1, żeby móc porównać rekord z poprzednim. Po sortowaniu jednakowe ID będą obok siebie.

**Krok 25.** Dopisujemy pod poprzednim fragmentem.

```cpp
        if (transakcje[i - 1].id == transakcje[i].id) {
            throw std::runtime_error(nazwa + ": powtorzone ID transakcji");
        }
    }
    std::sort(transakcje.begin(), transakcje.end(),
              [](const Transakcja& a, const Transakcja& b) {
```

Przez `throw` przerywamy tę operację i przekazujemy opis błędu do `catch` w main. Nie pracujemy dalej na niepoprawnych danych. Sortujemy zakres od `begin()` do `end()`. W następnych liniach podajemy regułę, która mówi, kiedy element a ma być przed b. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Zapis `[](...) { ... }` to krótka funkcja, czyli lambda. Tu porównujemy dwa rekordy bez tworzenia osobnej nazwanej funkcji.

**Krok 26.** Dopisujemy pod poprzednim fragmentem.

```cpp
                  if (a.nadejscie != b.nadejscie) {
                      return a.nadejscie < b.nadejscie;
                  }
                  return a.id < b.id;
```

Dopisujemy tę część do rozpoczętej wcześniej definicji. Wcięcia pokazują, które instrukcje należą do tego samego bloku.

**Krok 27.** Dopisujemy pod poprzednim fragmentem.

```cpp
              });
    return transakcje;
}
```

Dopisujemy tę część do rozpoczętej wcześniej definicji. Wcięcia pokazują, które instrukcje należą do tego samego bloku.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.3.2. Sprawdzamy sam odczyt

**Plik:** `main.cpp`. **Co robimy:** zastępujemy cały main.cpp tym testem.

Zastępujemy test Walidator testem Pliki. Odczytujemy konfigurację i transakcje, a następnie wypisujemy pola. W razie błędu catch wypisuje jego przyczynę. Pliki wejściowe leżą w folderze, z którego uruchamiamy program.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#include "Pliki.hpp"
#include <exception>
#include <iostream>
int main() {
    try {
        Konfiguracja konfiguracja = Pliki::wczytajKonfiguracje("konfiguracja.txt");
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++. W bloku try wykonujemy czynności, które mogą zgłosić błąd. Pasujący catch wpiszemy po zamknięciu tego bloku. Wywołujemy metodę Pliki i zapamiętujemy zwrócone dane. Ścieżkę liczymy od folderu, z którego uruchomiliśmy program.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
        std::vector<Transakcja> transakcje = Pliki::wczytajTransakcje("zgloszenia.txt");
        std::cout << konfiguracja.walidatorzy.size() << ' '
                  << konfiguracja.ziarno << ' ' << transakcje.size() << '\n';
        for (const Transakcja& t : transakcje) {
            std::cout << t.id << ' ' << t.nadejscie << ' '
                      << t.czasWykonania << ' ' << t.fee << ' ' << t.deadline << '\n';
```

W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Wywołujemy metodę Pliki i zapamiętujemy zwrócone dane. Ścieżkę liczymy od folderu, z którego uruchomiliśmy program. Przez `cout <<` wypisujemy dane na ekran. Spacje rozdzielają wartości, a `\n` kończy wiersz; `cerr` służy do komunikatów o błędach. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania.

**Krok 3.** Dopisujemy pod poprzednim fragmentem.

```cpp
        }
        return 0;
    } catch (const std::exception& blad) {
        std::cerr << "Blad: " << blad.what() << '\n';
```

W main przez return 0 kończymy program i oznaczamy, że zakończył się poprawnie. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Przechwytujemy błąd. Przez what() odczytujemy jego opis, wypisujemy go na cerr i kończymy program kodem 1. Przez `cout <<` wypisujemy dane na ekran. Spacje rozdzielają wartości, a `\n` kończy wiersz; `cerr` służy do komunikatów o błędach.

**Krok 4.** Dopisujemy pod poprzednim fragmentem.

```cpp
        return 1;
    }
}
```

Dopisujemy tę część do rozpoczętej wcześniej definicji. Wcięcia pokazują, które instrukcje należą do tego samego bloku.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

**Przystanek: odczyt i sortowanie.** Kończymy podane definicje, zapisujemy pliki i kompilujemy w folderze z kodem.

```text
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Pliki.cpp -o symulator.exe
.\symulator.exe
```

**Oczekiwany wynik:** Dla naszego wejścia widzimy `1 12345 3`, potem trzy wiersze transakcji zgodne z plikiem. Gdy zamienimy kolejność rekordów na 3,1,2, program wypisze 1,2,3, bo nadejścia są jednakowe. Jeśli kompilator zgłasza błąd, poprawiamy pierwszy komunikat przed następnym krokiem.

Testujemy także powtórzone ID, czas wykonania 0, brak rekordu i dodatkową kolumnę. Za każdym razem oczekujemy komunikatu `Blad:` z przyczyną. Po próbie przywracamy poprawne wejście. W opisie zapisujemy wejście testu i komunikat.

### 2.4. Dodajemy FIFO i upływ czasu

#### 2.4.1. Deklarujemy stan przebiegu

**Plik:** `Symulator.hpp`. **Co robimy:** nowy plik.

Symulator przechowuje wejście, wykonawców, wyniki i kolejkę FIFO. Czas oraz nastepnaTransakcja zaczynają od 0. Prywatne metody dzielą przebieg na małe działania. Każde uruchomienie tworzymy w osobnym obiekcie. Na tym etapie konstruktor przyjmuje tylko konfigurację i wejście.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#pragma once
#include "Dane.hpp"
#include "Walidator.hpp"
#include <cstddef>
#include <queue>
#include <vector>
```

Przez `#pragma once` prosimy kompilator, żeby dołączył ten nagłówek tylko raz w danym pliku .cpp. Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
class Symulator {
public:
    Symulator(const Konfiguracja& konfiguracja,
              const std::vector<Transakcja>& wejscie);
    std::vector<WynikTransakcji> uruchom();
    long long pobierzCzasKonca() const;
```

Zaczynamy klasę. Łączymy w niej dane obiektu z metodami, przez które z niego korzystamy. Pod `public:` wpisujemy metody dostępne z innych części programu. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać.

**Krok 3.** Dopisujemy pod poprzednim fragmentem.

```cpp
private:
    std::vector<Transakcja> transakcje;
    std::vector<Walidator> walidatorzy;
    std::vector<WynikTransakcji> wyniki;
    std::queue<Transakcja> fifo;
    long long czas = 0;
```

Pod `private:` trzymamy stan obiektu. Zmieniamy go przez metody, żeby nie nadpisać go przypadkiem w main. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Tworzymy kolejkę FIFO. Dodajemy na koniec, a pobieramy z początku. Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki.

**Krok 4.** Dopisujemy pod poprzednim fragmentem.

```cpp
    std::size_t nastepnaTransakcja = 0;
    void dodajNadejscia();
    bool kolejkaPusta() const;
    Transakcja pobierzTransakcje();
    int wybierzWalidatora() const;
    void przydzielZadania();
```

Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki.

**Krok 5.** Dopisujemy pod poprzednim fragmentem.

```cpp
    void sprawdzZakonczenia();
    bool czyPozostalaPraca() const;
};
```

Dopisujemy tę część do rozpoczętej wcześniej definicji. Wcięcia pokazują, które instrukcje należą do tego samego bloku.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.4.2. Dołączamy nagłówek

**Plik:** `Symulator.cpp`. **Co robimy:** nowy plik.

Tworzymy plik definicji metod. Kolejne metody dopisujemy w tym samym pliku, pod poprzednimi.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#include "Symulator.hpp"
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.4.3. Tworzymy walidatorów

**Plik:** `Symulator.cpp`. **Co robimy:** dopisujemy po poprzedniej zakończonej metodzie.

Kopiujemy transakcje i tworzymy osobny Walidator dla każdego rekordu konfiguracji. Każdy zaczyna wolny.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
Symulator::Symulator(const Konfiguracja& konfiguracja,
                     const std::vector<Transakcja>& wejscie)
    : transakcje(wejscie) {
    for (const DaneWalidatora& dane : konfiguracja.walidatorzy) {
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Na liście inicjalizacyjnej kopiujemy dane wejściowe do nowego przebiegu. Pola czasu i indeksów zaczynają od wartości 0 podanych w nagłówku. W tej pętli bierzemy po kolei elementy listy. Znak `&` pozwala odwołać się do właściwego obiektu; z `const` tylko go odczytujemy.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
        walidatorzy.push_back(Walidator(dane));
    }
}
```

Przez `push_back` dopisujemy element na koniec listy.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.4.4. Wpuszczamy nadejścia

**Plik:** `Symulator.cpp`. **Co robimy:** dopisujemy po poprzedniej zakończonej metodzie.

Wejście jest posortowane przez Pliki. Dopóki kolejny rekord ma nadejście równe czas, dopisujemy go do kolejki. Nie wpuszczamy jeszcze przyszłych zadań.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
void Symulator::dodajNadejscia() {
    while (nastepnaTransakcja < transakcje.size()
           && transakcje[nastepnaTransakcja].nadejscie == czas) {
        fifo.push(transakcje[nastepnaTransakcja]);
```

Najpierw sprawdzamy, czy istnieje kolejny rekord. Dzięki kolejności warunków z `&&` nie odczytujemy elementu poza listą. Wpuszczamy do kolejki tylko transakcje przychodzące w bieżącym czasie. Wcześniejsze wczytanie pliku nie oznacza ich nadejścia. Przez `push` dodajemy transakcję do wybranej kolejki.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
        ++nastepnaTransakcja;
    }
}
```

Przesuwamy indeks na kolejny rekord, żeby nie dodać tej samej transakcji drugi raz.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.4.5. Sprawdzamy kolejkę

**Plik:** `Symulator.cpp`. **Co robimy:** dopisujemy po poprzedniej zakończonej metodzie.

Ta metoda ukrywa sposób sprawdzania kolejki. W etapie 3 zmienimy jej wnętrze, ale pętla czasu nadal będzie ją wywoływać.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
bool Symulator::kolejkaPusta() const {
    return fifo.empty();
}
```

Dopisujemy tę część do rozpoczętej wcześniej definicji. Wcięcia pokazują, które instrukcje należą do tego samego bloku.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.4.6. Pobieramy pierwszą transakcję

**Plik:** `Symulator.cpp`. **Co robimy:** dopisujemy po poprzedniej zakończonej metodzie.

Front pokazuje pierwszą transakcję, pop usuwa ją z kolejki. Najpierw kopiujemy rekord, żeby nie stracić go po usunięciu.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
Transakcja Symulator::pobierzTransakcje() {
    Transakcja transakcja = fifo.front();
    fifo.pop();
    return transakcja;
}
```

Przez `front()` kopiujemy pierwszą transakcję FIFO. Dopiero `pop()` usuwa ją z kolejki.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.4.7. Wybieramy wolnego wykonawcę

**Plik:** `Symulator.cpp`. **Co robimy:** dopisujemy po poprzedniej zakończonej metodzie.

Sprawdzamy wykonawców w kolejności ID. Zwracamy indeks pierwszego wolnego, a nie jego ID. Gdy nie ma wolnych, zwracamy -1.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
int Symulator::wybierzWalidatora() const {
    for (std::size_t i = 0; i < walidatorzy.size(); ++i) {
        if (walidatorzy[i].czyWolny()) {
            return static_cast<int>(i);
        }
    }
```

Przechodzimy przez listę według indeksów: zaczynamy od 0, kończymy przed `size()`, a po każdym obrocie zwiększamy i o 1. Przez `static_cast<typ>` zapisujemy jawną konwersję liczby na podany typ. Przykładowo zamieniamy indeks `size_t` na zwracany `int`.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
    return -1;
}
```

Wartość -1 oznacza, że nie znaleźliśmy wolnego walidatora. Wtedy zostawiamy zadanie w kolejce.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.4.8. Przydzielamy zadania

**Plik:** `Symulator.cpp`. **Co robimy:** dopisujemy po poprzedniej zakończonej metodzie.

Najpierw szukamy wykonawcy, dopiero potem zdejmujemy transakcję z kolejki. Dzięki temu zadanie nie ginie, gdy wszyscy są zajęci.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
void Symulator::przydzielZadania() {
    while (!kolejkaPusta()) {
        int indeks = wybierzWalidatora();
        if (indeks == -1) {
            break;
        }
```

Przydzielamy zadania, dopóki w kolejce coś czeka. Jeśli nie znajdziemy wolnego wykonawcy, przerwiemy tę pętlę przez `break`. Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki. Wartość -1 oznacza, że nie znaleźliśmy wolnego walidatora. Wtedy zostawiamy zadanie w kolejce.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
        Transakcja transakcja = pobierzTransakcje();
        walidatorzy[indeks].rozpocznij(transakcja, czas);
    }
}
```

Przekazujemy wybraną transakcję wolnemu walidatorowi. Zaczyna ją w bieżącym czasie symulacji.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.4.9. Kończymy zadania

**Plik:** `Symulator.cpp`. **Co robimy:** dopisujemy po poprzedniej zakończonej metodzie.

Sprawdzamy wykonawców po przesunięciu czasu. Zakoncz oddaje wynik i oznacza obiekt jako wolny.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
void Symulator::sprawdzZakonczenia() {
    for (Walidator& walidator : walidatorzy) {
        if (walidator.czyKonczy(czas)) {
            wyniki.push_back(walidator.zakoncz());
```

W tej pętli bierzemy po kolei elementy listy. Znak `&` pozwala odwołać się do właściwego obiektu; z `const` tylko go odczytujemy. Sprawdzamy, czy zadanie kończy się dokładnie teraz. Przez `zakoncz()` otrzymujemy wynik i zwalniamy walidatora. Przez `push_back` dopisujemy element na koniec listy.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
        }
    }
}
```

Zamykamy otwarte wcześniej bloki. Po definicji klasy lub struktury stawiamy `};`, a po ciele funkcji samo `}`.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.4.10. Sprawdzamy, czy jest jeszcze praca

**Plik:** `Symulator.cpp`. **Co robimy:** dopisujemy po poprzedniej zakończonej metodzie.

Kontrolujemy trzy możliwości: przyszłe nadejścia, oczekujących w kolejce i pracujących walidatorów. Samo opróżnienie kolejki nie kończy symulacji.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
bool Symulator::czyPozostalaPraca() const {
    if (nastepnaTransakcja < transakcje.size() || !kolejkaPusta()) {
        return true;
    }
    for (const Walidator& walidator : walidatorzy) {
        if (!walidator.czyWolny()) {
```

Zapis `||` czytamy jako „lub”: wystarczy jeden prawdziwy warunek. Przy sprawdzaniu błędów odrzucamy rekord, gdy choć jedno ograniczenie zostało złamane. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W tej pętli bierzemy po kolei elementy listy. Znak `&` pozwala odwołać się do właściwego obiektu; z `const` tylko go odczytujemy.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
            return true;
        }
    }
    return false;
}
```

Dopisujemy tę część do rozpoczętej wcześniej definicji. Wcięcia pokazują, które instrukcje należą do tego samego bloku.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.4.11. Łączymy pętlę czasu

**Plik:** `Symulator.cpp`. **Co robimy:** dopisujemy po poprzedniej zakończonej metodzie.

Zaczynamy obsługę w 0. Jeśli praca została, zwiększamy czas o 1 i obsługujemy zakończenia. Dopiero potem kolejny obrót wpuszcza nadejścia i przydziela nowe zadania.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
std::vector<WynikTransakcji> Symulator::uruchom() {
    while (true) {
        dodajNadejscia();
        przydzielZadania();
        if (!czyPozostalaPraca()) {
            break;
```

W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
        }
        ++czas;
        sprawdzZakonczenia();
    }
    return wyniki;
}
```

Zwiększamy czas symulacji o jedną umowną jednostkę. Nie dodajemy rzeczywistego czekania ani sleep.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.4.12. Udostępniamy czas końca

**Plik:** `Symulator.cpp`. **Co robimy:** dopisujemy po poprzedniej zakończonej metodzie.

Zwracamy czas zakończenia całego przebiegu. W etapie 5 użyjemy go jako mianownika wykorzystania walidatorów.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
long long Symulator::pobierzCzasKonca() const {
    return czas;
}
```

Przez return oddajemy wartość wskazanego pola osobie wywołującej metodę. Metoda z const na końcu nagłówka odczytuje stan, bez zmieniania go.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.4.13. Sprawdzamy przebieg na ekranie

**Plik:** `main.cpp`. **Co robimy:** zastępujemy test odczytu tym main.cpp.

Odbieramy listę wyników i wypisujemy cztery pola. Nie zapisujemy jeszcze pliku.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#include "Pliki.hpp"
#include "Symulator.hpp"
#include <exception>
#include <iostream>
int main() {
    try {
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++. W bloku try wykonujemy czynności, które mogą zgłosić błąd. Pasujący catch wpiszemy po zamknięciu tego bloku.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
        Konfiguracja konfiguracja = Pliki::wczytajKonfiguracje("konfiguracja.txt");
        std::vector<Transakcja> transakcje = Pliki::wczytajTransakcje("zgloszenia.txt");
        Symulator symulator(konfiguracja, transakcje);
        std::vector<WynikTransakcji> wyniki = symulator.uruchom();
        for (const WynikTransakcji& w : wyniki) {
            std::cout << w.idTransakcji << ' ' << w.start << ' '
```

Wywołujemy metodę Pliki i zapamiętujemy zwrócone dane. Ścieżkę liczymy od folderu, z którego uruchomiliśmy program. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Uruchamiamy cały przebieg i odbieramy listę zakończonych transakcji. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania.

**Krok 3.** Dopisujemy pod poprzednim fragmentem.

```cpp
                      << w.koniec << ' ' << w.oczekiwanie << '\n';
        }
        return 0;
    } catch (const std::exception& blad) {
```

Przez `cout <<` wypisujemy dane na ekran. Spacje rozdzielają wartości, a `\n` kończy wiersz; `cerr` służy do komunikatów o błędach. W main przez return 0 kończymy program i oznaczamy, że zakończył się poprawnie. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Przechwytujemy błąd. Przez what() odczytujemy jego opis, wypisujemy go na cerr i kończymy program kodem 1.

**Krok 4.** Dopisujemy pod poprzednim fragmentem.

```cpp
        std::cerr << "Blad: " << blad.what() << '\n';
        return 1;
    }
}
```

Przez `cout <<` wypisujemy dane na ekran. Spacje rozdzielają wartości, a `\n` kończy wiersz; `cerr` służy do komunikatów o błędach.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

**Przystanek: FIFO na ekranie.** Kończymy podane definicje, zapisujemy pliki i kompilujemy w folderze z kodem.

```text
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Walidator.cpp Symulator.cpp Pliki.cpp -o symulator.exe
.\symulator.exe
```

**Oczekiwany wynik:** Otrzymujemy `1 0 2 0`, `2 2 4 2`, `3 4 6 4`. Dla jednej transakcji `1 0 3 1 10` oczekujemy startu 0 i końca 3. Dla samego `0` nie ma rekordów wyników. Jeśli kompilator zgłasza błąd, poprawiamy pierwszy komunikat przed następnym krokiem.

### 2.5. Zapisujemy wyniki FIFO

#### 2.5.1. Definiujemy zapis wyników

**Plik:** `Pliki.cpp`. **Co robimy:** dopisujemy na końcu Pliki.cpp, po odczycie transakcji.

Dopisujemy zapiszWyniki na końcu Pliki.cpp. Nagłówek kolumn i jeden wiersz na zakończoną transakcję pozwalają porównać plik z przykładem ręcznym. Na razie nie dopisujemy średniej ani podsumowania.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
void Pliki::zapiszWyniki(const std::string& nazwa,
                        const std::vector<WynikTransakcji>& wyniki) {
    std::ofstream plik(nazwa);
    if (!plik) {
        throw std::runtime_error("Nie mozna zapisac " + nazwa);
    }
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Otwieramy plik do zapisu. Istniejącą zawartość tego pliku nadpisujemy przy każdym udanym otwarciu. Przez `throw` przerywamy tę operację i przekazujemy opis błędu do `catch` w main. Nie pracujemy dalej na niepoprawnych danych.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
    plik << "id_transakcji id_walidatora nadejscie start koniec "
         << "oczekiwanie deadline opoznienie terminowo\n";
    for (const WynikTransakcji& wynik : wyniki) {
        plik << wynik.idTransakcji << ' ' << wynik.idWalidatora << ' '
             << wynik.nadejscie << ' ' << wynik.start << ' ' << wynik.koniec << ' '
             << wynik.oczekiwanie << ' ' << wynik.deadline << ' '
```

Operatorem `<<` dopisujemy dane do pliku. Spacje rozdzielają kolumny, a `\n` kończy wiersz. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W tej pętli bierzemy po kolei elementy listy. Znak `&` pozwala odwołać się do właściwego obiektu; z `const` tylko go odczytujemy.

**Krok 3.** Dopisujemy pod poprzednim fragmentem.

```cpp
             << wynik.opoznienie << ' ' << wynik.terminowo << '\n';
    }
    plik.flush();
    if (!plik) {
```

Operatorem `<<` dopisujemy dane do pliku. Spacje rozdzielają kolumny, a `\n` kończy wiersz. Przekazujemy bufor zapisu do pliku i sprawdzamy stan strumienia. Błąd zapisu zgłaszamy tak samo jak błąd otwarcia.

**Krok 4.** Dopisujemy pod poprzednim fragmentem.

```cpp
        throw std::runtime_error("Blad zapisu " + nazwa);
    }
}
```

Przez `throw` przerywamy tę operację i przekazujemy opis błędu do `catch` w main. Nie pracujemy dalej na niepoprawnych danych.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 2.5.2. Łączymy odczyt, FIFO i zapis

**Plik:** `main.cpp`. **Co robimy:** zastępujemy cały dotychczasowy main.cpp.

Zastępujemy main testowy programem kończącym etap 2. Wczytujemy dane, uruchamiamy jeden przebieg i zapisujemy wynik. Zachowujemy try/catch.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#include "Pliki.hpp"
#include "Symulator.hpp"
#include <exception>
#include <iostream>
int main() {
    try {
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++. W bloku try wykonujemy czynności, które mogą zgłosić błąd. Pasujący catch wpiszemy po zamknięciu tego bloku.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
        Konfiguracja konfiguracja = Pliki::wczytajKonfiguracje("konfiguracja.txt");
        std::vector<Transakcja> transakcje = Pliki::wczytajTransakcje("zgloszenia.txt");
        Symulator fifo(konfiguracja, transakcje);
        std::vector<WynikTransakcji> wynikiFifo = fifo.uruchom();
        Pliki::zapiszWyniki("wyniki_fifo.txt", wynikiFifo);
        std::cout << "Zapisano wyniki_fifo.txt\n";
```

Wywołujemy metodę Pliki i zapamiętujemy zwrócone dane. Ścieżkę liczymy od folderu, z którego uruchomiliśmy program. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Tworzymy nowy obiekt na ten przebieg. Zaczynamy od czasu 0 i wolnych walidatorów, niezależnie od poprzedniej strategii. Uruchamiamy cały przebieg i odbieramy listę zakończonych transakcji.

**Krok 3.** Dopisujemy pod poprzednim fragmentem.

```cpp
        return 0;
    } catch (const std::exception& blad) {
        std::cerr << "Blad: " << blad.what() << '\n';
        return 1;
    }
}
```

W main przez return 0 kończymy program i oznaczamy, że zakończył się poprawnie. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Przechwytujemy błąd. Przez what() odczytujemy jego opis, wypisujemy go na cerr i kończymy program kodem 1. Przez `cout <<` wypisujemy dane na ekran. Spacje rozdzielają wartości, a `\n` kończy wiersz; `cerr` służy do komunikatów o błędach.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

**Przystanek: koniec etapu 2.** Kończymy podane definicje, zapisujemy pliki i kompilujemy w folderze z kodem.

```text
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Walidator.cpp Symulator.cpp Pliki.cpp -o symulator.exe
.\symulator.exe
```

**Oczekiwany wynik:** Widzimy `Zapisano wyniki_fifo.txt`. W pliku mamy nagłówek i trzy rekordy: starty 0,2,4; końce 2,4,6; oczekiwania 0,2,4. Nie ma jeszcze PODSUMOWANIE. Jeśli kompilator zgłasza błąd, poprawiamy pierwszy komunikat przed następnym krokiem.

Na koniec mamy osiem plików kodu, README.md z poleceniem kompilacji oraz wyniki_fifo.txt. Sprawdzamy, że liczba rekordów zgadza się z liczbą transakcji i że jeden walidator nie wykonuje dwóch zadań naraz. Wynik próby zapisujemy w „Stan pracy” w opisie.

## Etap 3. Dodajemy FEE i losowanie walidatora

Rozbudowujemy działające FIFO. Zachowujemy Walidator i odczyt plików. Poniżej wskazujemy konkretne miejsca dopisania lub zastąpienia kodu. Po ostatnim kroku tego etapu oba przebiegi zapisują wyniki, nadal bez zbiorczych statystyk.

### 3.1. Dodajemy wybór strategii

#### 3.1.1. Nazywamy strategie

**Plik:** `Dane.hpp`. **Co robimy:** dopisujemy na końcu nagłówka.

Dopisujemy na końcu Dane.hpp, po zamknięciu WynikTransakcji. Ten typ przechowuje wybór kolejki.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
enum class Strategia { FIFO, FEE };
```

Definiujemy własny typ `Strategia`. W kodzie wybieramy jedną z nazw: `Strategia::FIFO` albo `Strategia::FEE`.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.1.2. Zapamiętujemy wybór

**Plik:** `Symulator.hpp`. **Co robimy:** dopisujemy jedno pole pod private:.

Dopisujemy bezpośrednio pod private:.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
    Strategia strategia;
```

Dopisujemy tę część do rozpoczętej wcześniej definicji. Wcięcia pokazują, które instrukcje należą do tego samego bloku.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.1.3. Rozszerzamy deklarację konstruktora

**Plik:** `Symulator.hpp`. **Co robimy:** zastępujemy tylko deklarację konstruktora w public:.

Usuwamy dotychczasową deklarację konstruktora z dwoma parametrami. Metod uruchom i pobierzCzasKonca nie zmieniamy.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
    Symulator(const Konfiguracja& konfiguracja,
              const std::vector<Transakcja>& wejscie,
              Strategia wybranaStrategia);
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.1.4. Zmieniamy definicję konstruktora

**Plik:** `Symulator.cpp`. **Co robimy:** zastępujemy starą definicję konstruktora.

Zastępujemy tylko Symulator::Symulator. Parametr wybranaStrategia kopiujemy do pola strategia. Pozostałe metody na razie obsługują FIFO.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
Symulator::Symulator(const Konfiguracja& konfiguracja,
                     const std::vector<Transakcja>& wejscie,
                     Strategia wybranaStrategia)
    : strategia(wybranaStrategia), transakcje(wejscie) {
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Na liście inicjalizacyjnej kopiujemy dane wejściowe do nowego przebiegu. Pola czasu i indeksów zaczynają od wartości 0 podanych w nagłówku.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
    for (const DaneWalidatora& dane : konfiguracja.walidatorzy) {
        walidatorzy.push_back(Walidator(dane));
    }
}
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W tej pętli bierzemy po kolei elementy listy. Znak `&` pozwala odwołać się do właściwego obiektu; z `const` tylko go odczytujemy. Przez `push_back` dopisujemy element na koniec listy.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.1.5. Podajemy wybór przy tworzeniu obiektu

**Plik:** `main.cpp`. **Co robimy:** zastępujemy jeden wiersz w main.

Zastępujemy dotychczasowy wiersz tworzący fifo. Na tym przystanku sprawdzamy tylko FIFO; obsługę wyboru FEE dopiszemy w 3.2.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
        Symulator fifo(konfiguracja, transakcje, Strategia::FIFO);
```

Tworzymy nowy obiekt na ten przebieg. Zaczynamy od czasu 0 i wolnych walidatorów, niezależnie od poprzedniej strategii.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

**Przystanek: wybór zapisany w obiekcie.** Kończymy podane definicje, zapisujemy pliki i kompilujemy w folderze z kodem.

```text
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Walidator.cpp Symulator.cpp Pliki.cpp -o symulator.exe
.\symulator.exe
```

**Oczekiwany wynik:** FIFO daje te same trzy rekordy co na końcu etapu 2. Jeśli kompilator zgłasza błąd, poprawiamy pierwszy komunikat przed następnym krokiem.

### 3.2. Piszemy kolejkę FEE

#### 3.2.1. Deklarujemy regułę porównywania

**Plik:** `Symulator.hpp`. **Co robimy:** dopisujemy przed definicją klasy Symulator.

Wpisujemy przed class Symulator, po include. Funktor to obiekt wywoływany jak funkcja: kolejka użyje jego operator(). Porównujemy a z b. True oznacza, że a ma niższy priorytet.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
struct PorownajFee {
    bool operator()(const Transakcja& a, const Transakcja& b) const;
};
```

Zaczynamy rekord `PorownajFee`. Między jego klamrami wpisujemy pola, które przechowujemy razem. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.2.2. Dodajemy drugą kolejkę

**Plik:** `Symulator.hpp`. **Co robimy:** dopisujemy pod std::queue<Transakcja> fifo;.

Dopisujemy pod polem fifo. Trzy parametry oznaczają typ elementu, listę przechowującą elementy i regułę porównania.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
    std::priority_queue<Transakcja, std::vector<Transakcja>, PorownajFee> fee;
```

W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Tworzymy kolejkę FEE. `PorownajFee` wyznacza element, który odczytamy przez `top()`.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.2.3. Definiujemy komparator

**Plik:** `Symulator.cpp`. **Co robimy:** dopisujemy przed konstruktorem.

Dopisujemy po include, przed konstruktorem. Najpierw porównujemy fee, potem nadejście, potem ID. Funkcja nie zmienia a ani b.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
bool PorownajFee::operator()(const Transakcja& a, const Transakcja& b) const {
    if (a.fee != b.fee) {
        return a.fee < b.fee;
    }
    if (a.nadejscie != b.nadejscie) {
        return a.nadejscie > b.nadejscie;
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W komparatorze priority_queue true oznacza, że a ma niższy priorytet niż b. Dlatego niższe fee porównujemy znakiem `<`. Przy równym fee późniejsze nadejście ma niższy priorytet. Zwracamy wtedy true dla a.nadejscie większego od b.nadejscie.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
    }
    return a.id > b.id;
}
```

Gdy fee i nadejście są równe, niższe ID wygrywa. Dlatego większe ID uznajemy za niższy priorytet.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.2.4. Dodajemy do wybranej kolejki

**Plik:** `Symulator.cpp`. **Co robimy:** zastępujemy poprzednią definicję wskazanej metody.

Zastępujemy tylko tę metodę. Deklaracja w nagłówku pozostaje bez zmian. Pętla uruchom także pozostaje taka sama.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
void Symulator::dodajNadejscia() {
    while (nastepnaTransakcja < transakcje.size()
           && transakcje[nastepnaTransakcja].nadejscie == czas) {
        const Transakcja& transakcja = transakcje[nastepnaTransakcja];
        if (strategia == Strategia::FIFO) {
            fifo.push(transakcja);
```

Najpierw sprawdzamy, czy istnieje kolejny rekord. Dzięki kolejności warunków z `&&` nie odczytujemy elementu poza listą. Wpuszczamy do kolejki tylko transakcje przychodzące w bieżącym czasie. Wcześniejsze wczytanie pliku nie oznacza ich nadejścia. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Przez `push` dodajemy transakcję do wybranej kolejki.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
        } else {
            fee.push(transakcja);
        }
        ++nastepnaTransakcja;
    }
}
```

Przez `push` dodajemy transakcję do wybranej kolejki. Przesuwamy indeks na kolejny rekord, żeby nie dodać tej samej transakcji drugi raz.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.2.5. Sprawdzamy wybraną kolejkę

**Plik:** `Symulator.cpp`. **Co robimy:** zastępujemy poprzednią definicję wskazanej metody.

Zastępujemy tylko tę metodę. Deklaracja w nagłówku pozostaje bez zmian. Pętla uruchom także pozostaje taka sama.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
bool Symulator::kolejkaPusta() const {
    if (strategia == Strategia::FIFO) {
        return fifo.empty();
    }
    return fee.empty();
}
```

Dopisujemy tę część do rozpoczętej wcześniej definicji. Wcięcia pokazują, które instrukcje należą do tego samego bloku.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.2.6. Pobieramy według strategii

**Plik:** `Symulator.cpp`. **Co robimy:** zastępujemy poprzednią definicję wskazanej metody.

Zastępujemy tylko tę metodę. Deklaracja w nagłówku pozostaje bez zmian. Pętla uruchom także pozostaje taka sama.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
Transakcja Symulator::pobierzTransakcje() {
    if (strategia == Strategia::FIFO) {
        Transakcja transakcja = fifo.front();
        fifo.pop();
        return transakcja;
    }
```

Przez `front()` kopiujemy pierwszą transakcję FIFO. Dopiero `pop()` usuwa ją z kolejki.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
    Transakcja transakcja = fee.top();
    fee.pop();
    return transakcja;
}
```

Przez `top()` kopiujemy transakcję o najwyższym priorytecie FEE. Dopiero `pop()` usuwa ją z kolejki.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

Na próbę w main zmieniamy tylko Strategia::FIFO na Strategia::FEE. Nazwa obiektu fifo i pliku wyniki_fifo.txt pozostaje robocza podczas tej próby; rozdzielimy je w 3.5.

**Przystanek: kolejka priorytetowa.** Kończymy podane definicje, zapisujemy pliki i kompilujemy w folderze z kodem.

```text
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Walidator.cpp Symulator.cpp Pliki.cpp -o symulator.exe
.\symulator.exe
```

**Oczekiwany wynik:** Po wybraniu FEE kolejność ID wynosi 2,3,1, a starty i końce nadal 0/2, 2/4, 4/6. Przy równych fee i nadejściach kolejność wynosi 1,2,3. Po teście przywracamy Strategia::FIFO w main. Jeśli kompilator zgłasza błąd, poprawiamy pierwszy komunikat przed następnym krokiem.

### 3.3. Dodajemy wybór według stake

Najpierw używamy ręcznie przygotowanej listy ułamków 0.5. W ten sposób sprawdzamy przedziały bez losowania. Generator dopiszemy w 3.4. Wszystkie zmiany w 3.3 kończymy przed kompilacją, bo zmieniamy jednocześnie deklarację, definicję i wywołanie konstruktora.

#### 3.3.1. Zapamiętujemy listę i jej indeks

**Plik:** `Symulator.hpp`. **Co robimy:** dopisujemy pod std::vector<Walidator> walidatorzy;.

Dopisujemy dwa pola w private:, bezpośrednio pod listą walidatorzy. Na jeden udany przydział użyjemy jednej pozycji listy.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
    std::vector<double> losowania;
    std::size_t nastepneLosowanie = 0;
```

W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.3.2. Przekazujemy losowania do konstruktora

**Plik:** `Symulator.hpp`. **Co robimy:** zastępujemy deklarację konstruktora.

Zastępujemy deklarację z 3.1. Nowy parametr wspolneLosowania przekazujemy przed wyborem strategii.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
    Symulator(const Konfiguracja& konfiguracja,
              const std::vector<Transakcja>& wejscie,
              const std::vector<double>& wspolneLosowania,
              Strategia wybranaStrategia);
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.3.3. Kopiujemy losowania do przebiegu

**Plik:** `Symulator.cpp`. **Co robimy:** zastępujemy definicję konstruktora.

Zastępujemy konstruktor z 3.1. Dzięki kopii listy i indeksowi 0 każdy nowy przebieg używa tych samych losów od początku.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
Symulator::Symulator(const Konfiguracja& konfiguracja,
                     const std::vector<Transakcja>& wejscie,
                     const std::vector<double>& wspolneLosowania,
                     Strategia wybranaStrategia)
    : strategia(wybranaStrategia), transakcje(wejscie), losowania(wspolneLosowania) {
    for (const DaneWalidatora& dane : konfiguracja.walidatorzy) {
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Na liście inicjalizacyjnej kopiujemy dane wejściowe do nowego przebiegu. Pola czasu i indeksów zaczynają od wartości 0 podanych w nagłówku. W tej pętli bierzemy po kolei elementy listy. Znak `&` pozwala odwołać się do właściwego obiektu; z `const` tylko go odczytujemy.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
        walidatorzy.push_back(Walidator(dane));
    }
}
```

Przez `push_back` dopisujemy element na koniec listy.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.3.4. Przyjmujemy jeden ułamek

**Plik:** `Symulator.hpp`. **Co robimy:** zastępujemy deklarację wybierzWalidatora.

Zastępujemy deklarację bez parametrów. Double losowanie ma należeć do [0,1). Zwracamy indeks wykonawcy albo -1.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
    int wybierzWalidatora(double losowanie) const;
```

Dopisujemy tę część do rozpoczętej wcześniej definicji. Wcięcia pokazują, które instrukcje należą do tego samego bloku.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.3.5. Piszemy wybór po przedziałach

**Plik:** `Symulator.cpp`. **Co robimy:** zastępujemy całą metodę wybierzWalidatora.

Zastępujemy wybór pierwszego wolnego wykonawcy. Dla wolnych stake 10 i 30 suma wynosi 40. Pierwszy przedział to [0,10), drugi [10,40). Ułamek 0.2 daje pozycję 8, więc pierwszego; 0.5 daje pozycję 20, więc drugiego. Zajętych pomijamy w obu pętlach.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
int Symulator::wybierzWalidatora(double losowanie) const {
    double sumaStake = 0;
    int ostatniWolny = -1;
    for (std::size_t i = 0; i < walidatorzy.size(); ++i) {
        if (walidatorzy[i].czyWolny()) {
            sumaStake += walidatorzy[i].pobierzStake();
```

Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki. Przechodzimy przez listę według indeksów: zaczynamy od 0, kończymy przed `size()`, a po każdym obrocie zwiększamy i o 1. Dodajemy stake tylko wolnego walidatora. Suma wyznacza długość całego przedziału losowania.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
            ostatniWolny = static_cast<int>(i);
        }
    }
    if (ostatniWolny == -1) {
        return -1;
    }
```

Przez `static_cast<typ>` zapisujemy jawną konwersję liczby na podany typ. Przykładowo zamieniamy indeks `size_t` na zwracany `int`. Wartość -1 oznacza, że nie znaleźliśmy wolnego walidatora. Wtedy zostawiamy zadanie w kolejce.

**Krok 3.** Dopisujemy pod poprzednim fragmentem.

```cpp
    double pozycja = losowanie * sumaStake;
    double granica = 0;
    for (std::size_t i = 0; i < walidatorzy.size(); ++i) {
        if (walidatorzy[i].czyWolny()) {
            granica += walidatorzy[i].pobierzStake();
            if (pozycja < granica) {
```

Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki. Przeliczamy ułamek z zakresu [0, 1) na pozycję w przedziale sumy stake. Dla ułamka 0.5 i sumy 40 pozycja wynosi 20. Przechodzimy przez listę według indeksów: zaczynamy od 0, kończymy przed `size()`, a po każdym obrocie zwiększamy i o 1. Dokładamy kolejny przedział stake. Pierwszy przedział z końcem większym od pozycji wskazuje wybranego wykonawcę.

**Krok 4.** Dopisujemy pod poprzednim fragmentem.

```cpp
                return static_cast<int>(i);
            }
        }
    }
    return ostatniWolny;
}
```

Przez `static_cast<typ>` zapisujemy jawną konwersję liczby na podany typ. Przykładowo zamieniamy indeks `size_t` na zwracany `int`. Na wypadek zaokrągleń liczb double zwracamy ostatniego wolnego walidatora. Do tego miejsca nie wybieramy zajętego wykonawcy.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.3.6. Zużywamy los po udanym przydziale

**Plik:** `Symulator.cpp`. **Co robimy:** zastępujemy metodę przydzielZadania.

Zastępujemy metodę przydzielZadania. Używamy kolejnej wartości dopiero wtedy, gdy mamy transakcję w kolejce. Lista ma N wartości; każdy udany przydział usuwa jedno z N zadań. Przy braku wolnych wykonawców nie zdejmujemy zadania i nie zwiększamy indeksu.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
void Symulator::przydzielZadania() {
    while (!kolejkaPusta()) {
        int indeks = wybierzWalidatora(losowania[nastepneLosowanie]);
        if (indeks == -1) {
            break;
        }
```

Przydzielamy zadania, dopóki w kolejce coś czeka. Jeśli nie znajdziemy wolnego wykonawcy, przerwiemy tę pętlę przez `break`. Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki. Wartość -1 oznacza, że nie znaleźliśmy wolnego walidatora. Wtedy zostawiamy zadanie w kolejce.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
        Transakcja transakcja = pobierzTransakcje();
        walidatorzy[indeks].rozpocznij(transakcja, czas);
        ++nastepneLosowanie;
    }
}
```

Przekazujemy wybraną transakcję wolnemu walidatorowi. Zaczyna ją w bieżącym czasie symulacji. Zużywamy los dopiero po udanym przydziale. Nie przesuwamy indeksu, gdy wszyscy walidatorzy są zajęci.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.3.7. Podajemy ręczne wartości testowe

**Plik:** `main.cpp`. **Co robimy:** zastępujemy wiersz tworzący fifo.

Zastępujemy jeden wiersz tworzący fifo tymi dwoma wierszami. Konstruktor vector z rozmiarem i wartością tworzy N jednakowych ułamków. Nie losujemy ich jeszcze.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
        std::vector<double> losowania(transakcje.size(), 0.5);
        Symulator fifo(konfiguracja, transakcje, losowania, Strategia::FIFO);
```

W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Tworzymy nowy obiekt na ten przebieg. Zaczynamy od czasu 0 i wolnych walidatorów, niezależnie od poprzedniej strategii.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

**Przystanek: przedziały stake.** Kończymy podane definicje, zapisujemy pliki i kompilujemy w folderze z kodem.

```text
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Walidator.cpp Symulator.cpp Pliki.cpp -o symulator.exe
.\symulator.exe
```

**Oczekiwany wynik:** Dla jednego walidatora wyniki są takie same jak wcześniej. Dla konfiguracji `2 12345`, `1 10`, `2 30` i jednej transakcji wybieramy ID 2 przy ułamku 0.5, a ID 1 po zmianie na 0.2. Przywracamy konfigurację i 0.5 po próbie. Jeśli kompilator zgłasza błąd, poprawiamy pierwszy komunikat przed następnym krokiem.

### 3.4. Generujemy wspólne losowania z ziarna

#### 3.4.1. Dołączamy bibliotekę losowania

**Plik:** `main.cpp`. **Co robimy:** dopisujemy jeden include.

Dopisujemy obok pozostałych include na początku main.cpp.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#include <random>
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.4.2. Przygotowujemy N losów

**Plik:** `main.cpp`. **Co robimy:** dopisujemy funkcję przed main.

Dopisujemy przed int main(). Liczba to liczba transakcji, ziarno odczytujemy z konfiguracji. Funkcja zwraca vector ułamków. Raz przygotowaną listę przekażemy obu strategiom.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
std::vector<double> przygotujLosowania(std::size_t liczba, unsigned int ziarno) {
    std::mt19937 generator(ziarno);
    std::uniform_real_distribution<double> losuj(0.0, 1.0);
    std::vector<double> losowania;
    for (std::size_t i = 0; i < liczba; ++i) {
        losowania.push_back(losuj(generator));
```

W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Tworzymy generator z wybranym ziarnem, tak jak seed ustala powtarzalny świat w Minecraft. Tutaj ustala kolejność losowanych liczb. Wybieramy równomierne losowanie ułamka od 0 włącznie do 1 wyłącznie. To jeszcze nie wybór walidatora: wykorzystamy ułamek w przedziałach stake. Przechodzimy przez listę według indeksów: zaczynamy od 0, kończymy przed `size()`, a po każdym obrocie zwiększamy i o 1.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
    }
    return losowania;
}
```

Dopisujemy tę część do rozpoczętej wcześniej definicji. Wcięcia pokazują, które instrukcje należą do tego samego bloku.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 3.4.3. Zastępujemy ręczne ułamki

**Plik:** `main.cpp`. **Co robimy:** zastępujemy wiersz tworzący listę ręcznych losów.

Zastępujemy tylko wiersz vector z 0.5. Wywołania konstruktora fifo poniżej nie zmieniamy.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
        std::vector<double> losowania = przygotujLosowania(
            transakcje.size(), konfiguracja.ziarno);
```

W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

**Przystanek: ziarno.** Kończymy podane definicje, zapisujemy pliki i kompilujemy w folderze z kodem.

```text
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Walidator.cpp Symulator.cpp Pliki.cpp -o symulator.exe
.\symulator.exe
```

**Oczekiwany wynik:** Dwa uruchomienia z tym samym wejściem i ziarnem zapisują identyczną zawartość wyniki_fifo.txt. Sprawdzamy w tym samym środowisku kompilacji. Przy jednym walidatorze zmiana ziarna nie zmieni przydziału. Jeśli kompilator zgłasza błąd, poprawiamy pierwszy komunikat przed następnym krokiem.

### 3.5. Uruchamiamy oba warianty

#### 3.5.1. Dodajemy drugi przebieg

**Plik:** `main.cpp`. **Co robimy:** zastępujemy funkcję main od int main() do jej ostatniej klamry.

Zastępujemy tylko funkcję main. Include oraz przygotujLosowania z 3.4 zostawiamy nad nią. Wczytujemy dane i generujemy listę raz. Tworzymy osobny obiekt FIFO i osobny FEE. Wynik drugiego przebiegu nie korzysta ze stanu pierwszego.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
int main() {
    try {
        Konfiguracja konfiguracja = Pliki::wczytajKonfiguracje("konfiguracja.txt");
        std::vector<Transakcja> transakcje = Pliki::wczytajTransakcje("zgloszenia.txt");
        std::vector<double> losowania = przygotujLosowania(
            transakcje.size(), konfiguracja.ziarno);
```

W bloku try wykonujemy czynności, które mogą zgłosić błąd. Pasujący catch wpiszemy po zamknięciu tego bloku. Wywołujemy metodę Pliki i zapamiętujemy zwrócone dane. Ścieżkę liczymy od folderu, z którego uruchomiliśmy program. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
        Symulator fifo(konfiguracja, transakcje, losowania, Strategia::FIFO);
        std::vector<WynikTransakcji> wynikiFifo = fifo.uruchom();
        Pliki::zapiszWyniki("wyniki_fifo.txt", wynikiFifo);
```

Tworzymy nowy obiekt na ten przebieg. Zaczynamy od czasu 0 i wolnych walidatorów, niezależnie od poprzedniej strategii. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Uruchamiamy cały przebieg i odbieramy listę zakończonych transakcji. Zapisujemy wyniki pod wskazaną nazwą. FIFO i FEE mają osobne pliki.

**Krok 3.** Dopisujemy pod poprzednim fragmentem.

```cpp
        Symulator fee(konfiguracja, transakcje, losowania, Strategia::FEE);
        std::vector<WynikTransakcji> wynikiFee = fee.uruchom();
        Pliki::zapiszWyniki("wyniki_fee.txt", wynikiFee);
```

Tworzymy nowy obiekt na ten przebieg. Zaczynamy od czasu 0 i wolnych walidatorów, niezależnie od poprzedniej strategii. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Uruchamiamy cały przebieg i odbieramy listę zakończonych transakcji. Zapisujemy wyniki pod wskazaną nazwą. FIFO i FEE mają osobne pliki.

**Krok 4.** Dopisujemy pod poprzednim fragmentem.

```cpp
        std::cout << "Zapisano wyniki_fifo.txt i wyniki_fee.txt\n";
        return 0;
    } catch (const std::exception& blad) {
        std::cerr << "Blad: " << blad.what() << '\n';
```

Przez `cout <<` wypisujemy dane na ekran. Spacje rozdzielają wartości, a `\n` kończy wiersz; `cerr` służy do komunikatów o błędach. W main przez return 0 kończymy program i oznaczamy, że zakończył się poprawnie. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Przechwytujemy błąd. Przez what() odczytujemy jego opis, wypisujemy go na cerr i kończymy program kodem 1.

**Krok 5.** Dopisujemy pod poprzednim fragmentem.

```cpp
        return 1;
    }
}
```

Dopisujemy tę część do rozpoczętej wcześniej definicji. Wcięcia pokazują, które instrukcje należą do tego samego bloku.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

**Przystanek: koniec etapu 3.** Kończymy podane definicje, zapisujemy pliki i kompilujemy w folderze z kodem.

```text
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Walidator.cpp Symulator.cpp Pliki.cpp -o symulator.exe
.\symulator.exe
```

**Oczekiwany wynik:** Powstają dwa pliki. FIFO ma kolejność 1,2,3; FEE 2,3,1. Oba zawierają trzy wyniki. Zbiorcze pomiary dopiszemy w etapie 5. Jeśli kompilator zgłasza błąd, poprawiamy pierwszy komunikat przed następnym krokiem.

Sprawdzamy także dwa walidatory i więcej zadań niż wykonawców: każda transakcja kończy się raz, a zadania jednego wykonawcy nie nakładają się. Porównujemy powtórzone uruchomienia obu strategii. Zajęty walidator nie bierze udziału w losowaniu. Wyniki tych prób wpisujemy do opisu.

## Etap 4. Opisujemy koszt działania programu

Zadanie 4. Przewidujemy 3–4 godziny.

### 4.1. Zapisujemy znaczenie symboli

- [ ] Tworzymy `zlozonosc.md`.
- [ ] Zapisujemy: N to liczba transakcji, K to liczba walidatorów, T to liczba wykonanych kroków czasu.

**Wynik:** początek opisu złożoności. T oznacza kroki symulacji, a nie sekundy działania komputera.

### 4.2. Opisujemy kolejki

- [ ] FIFO: dodanie i usunięcie elementu kosztują O(1).
- [ ] FEE: dodanie i usunięcie kosztują O(log N), a podejrzenie pierwszego elementu O(1).
- [ ] Dopisujemy jedno zdanie wyjaśnienia: O(1) oznacza stały koszt operacji; O(log N) rośnie wraz z rozmiarem kolejki.

**Wynik:** tabela operacji obu kolejek w `zlozonosc.md`.

### 4.3. Liczymy pętle

- [ ] Zaznaczamy w kodzie sprawdzanie K walidatorów w każdym kroku: O(TK).
- [ ] Zaznaczamy wybór wykonawcy przez przegląd walidatorów dla N przydziałów: O(NK). Po dopisaniu Statystyki w etapie 5 uzupełniamy analizę o liczenie wykorzystania: O(NK), bo dla każdego walidatora przeglądamy wyniki.
- [ ] Dodajemy sortowanie transakcji: O(N log N) i danych walidatorów: O(K log K).

**Wynik:** opis, który wskazuje konkretne pętle i operacje w naszym kodzie.

### 4.4. Zapisujemy koszt całości i pamięci

- [ ] Dla tego modelu zapisujemy O(N log(N + 1) + K log(K + 1) + TK + NK). Dodanie 1 w logarytmach pozwala objąć też puste wejście i jednego walidatora.
- [ ] Wyjaśniamy, że koszt kolejek różni się, nawet gdy po uwzględnieniu sortowania otrzymujemy tę samą ogólną granicę.
- [ ] Szacujemy pamięć jako O(N + K), jeśli przechowujemy transakcje, losowania, wyniki i walidatorów.

**Wynik:** opis czasu i pamięci. Jeśli dodamy inne pętle lub zapisy, uwzględniamy je w obliczeniach.

### 4.5. Opisujemy ograniczenia

- [ ] Wyjaśniamy, że długa przerwa między transakcjami zwiększa liczbę kroków T.
- [ ] Opisujemy długie oczekiwanie niskiego fee przy napływie transakcji z wysokim fee.
- [ ] Czytamy opis we dwoje i pokazujemy sobie odpowiadające mu fragmenty kodu.

**Na koniec etapu:** `zlozonosc.md` i potrafimy uzasadnić podane koszty.

## Etap 5. Zbieramy wyniki i piszemy sprawozdanie

Teraz dokładamy zbiorcze statystyki do działających przebiegów. Oczekiwanie, deadline i opóźnienie pojedynczej transakcji mamy już w wynikach z etapu 2. Tu obliczamy średnią, maksimum, liczby terminowych i spóźnionych oraz wykorzystanie. Tworzymy dwa nowe pliki Statystyki.

### 5.1. Dodajemy podsumowanie i obliczenia

#### 5.1.1. Dodajemy rekord Podsumowanie

**Plik:** `Dane.hpp`. **Co robimy:** dopisujemy po istniejących definicjach.

Dopisujemy na końcu nagłówka. Pola zaczynają od 0, a wykorzystanie od pustej listy. Wpiszemy do niej jeden procent na walidatora, w kolejności z konfiguracji.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
struct Podsumowanie {
    long long liczbaObsluzonych = 0;
    double srednieOczekiwanie = 0;
    long long maksymalneOczekiwanie = 0;
    long long terminowe = 0;
    long long spoznione = 0;
```

Zaczynamy rekord `Podsumowanie`. Między jego klamrami wpisujemy pola, które przechowujemy razem. Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
    long long czasKonca = 0;
    std::vector<double> wykorzystanie;
};
```

Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 5.1.2. Deklarujemy obliczenia

**Plik:** `Statystyki.hpp`. **Co robimy:** nowy plik.

Metoda statyczna oblicz przyjmuje wyniki, początkowych walidatorów i czas końca. Oddaje Podsumowanie. Nie przechowujemy stanu w obiekcie Statystyki.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#pragma once
```

Przez `#pragma once` prosimy kompilator, żeby dołączył ten nagłówek tylko raz w danym pliku .cpp.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
#include "Dane.hpp"
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Krok 3.** Dopisujemy pod poprzednim fragmentem.

```cpp
class Statystyki {
public:
    static Podsumowanie oblicz(const std::vector<WynikTransakcji>& wyniki,
                              const std::vector<DaneWalidatora>& walidatorzy,
                              long long czasKonca);
};
```

Zaczynamy klasę. Łączymy w niej dane obiektu z metodami, przez które z niego korzystamy. Pod `public:` wpisujemy metody dostępne z innych części programu. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 5.1.3. Definiujemy pomiary

**Plik:** `Statystyki.cpp`. **Co robimy:** nowy plik.

Najpierw przechodzimy przez wyniki: sumujemy oczekiwanie, wybieramy maksimum i zliczamy terminy. Następnie dla każdego walidatora sumujemy czasy jego zadań. Wykorzystanie = 100 razy czas pracy / czas końca przebiegu od 0. Przy pustym wejściu zostawiamy zera, żeby nie dzielić przez 0. Dwie zagnieżdżone pętle wykorzystania dają O(NK); dopisujemy to w zlozonosc.md z etapu 4.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#include "Statystyki.hpp"
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
#include <algorithm>
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Krok 3.** Dopisujemy pod poprzednim fragmentem.

```cpp
Podsumowanie Statystyki::oblicz(const std::vector<WynikTransakcji>& wyniki,
                              const std::vector<DaneWalidatora>& walidatorzy,
                              long long czasKonca) {
    Podsumowanie podsumowanie;
    podsumowanie.liczbaObsluzonych = static_cast<long long>(wyniki.size());
    podsumowanie.czasKonca = czasKonca;
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Przez `static_cast<typ>` zapisujemy jawną konwersję liczby na podany typ. Przykładowo zamieniamy indeks `size_t` na zwracany `int`.

**Krok 4.** Dopisujemy pod poprzednim fragmentem.

```cpp
    double sumaOczekiwania = 0;
    for (const WynikTransakcji& wynik : wyniki) {
        sumaOczekiwania += wynik.oczekiwanie;
        podsumowanie.maksymalneOczekiwanie = std::max(
            podsumowanie.maksymalneOczekiwanie, wynik.oczekiwanie);
        if (wynik.terminowo) {
```

Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W tej pętli bierzemy po kolei elementy listy. Znak `&` pozwala odwołać się do właściwego obiektu; z `const` tylko go odczytujemy. Dodajemy oczekiwanie do sumy. W `std::max` wybieramy największe dotychczasowe oczekiwanie.

**Krok 5.** Dopisujemy pod poprzednim fragmentem.

```cpp
            ++podsumowanie.terminowe;
        } else {
            ++podsumowanie.spoznione;
        }
    }
    if (!wyniki.empty()) {
```

Średnią liczymy tylko dla niepustej listy, żeby nie dzielić przez zero. Przy pustej zostawiamy początkową wartość 0.

**Krok 6.** Dopisujemy pod poprzednim fragmentem.

```cpp
        podsumowanie.srednieOczekiwanie = sumaOczekiwania / wyniki.size();
    }
    for (const DaneWalidatora& walidator : walidatorzy) {
        long long czasPracy = 0;
        for (const WynikTransakcji& wynik : wyniki) {
            if (wynik.idWalidatora == walidator.id) {
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W tej pętli bierzemy po kolei elementy listy. Znak `&` pozwala odwołać się do właściwego obiektu; z `const` tylko go odczytujemy. Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki.

**Krok 7.** Dopisujemy pod poprzednim fragmentem.

```cpp
                czasPracy += wynik.koniec - wynik.start;
            }
        }
        double procent = 0;
        if (czasKonca > 0) {
            procent = 100.0 * czasPracy / czasKonca;
```

Sumujemy czas pracy danego walidatora. Bierzemy tylko wyniki, w których idWalidatora zgadza się z jego ID. Tworzymy zmienną albo pole. Wartość po `=` jest wartością początkową; `long long` mieści większe liczby całkowite niż `int`, a `double` także ułamki. Przeliczamy zajętość na procent całego przebiegu od czasu 0. Dla pracy 2 i końca przebiegu 5 otrzymujemy 40%, a nie 100%.

**Krok 8.** Dopisujemy pod poprzednim fragmentem.

```cpp
        }
        podsumowanie.wykorzystanie.push_back(procent);
    }
    return podsumowanie;
}
```

Przez `push_back` dopisujemy element na koniec listy.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 5.1.4. Rozszerzamy deklarację zapisu

**Plik:** `Pliki.hpp`. **Co robimy:** zastępujemy tylko deklarację zapiszWyniki.

Zastępujemy starą deklarację zapiszWyniki. Nowe parametry to Podsumowanie i lista danych wykonawców. Nazwy w deklaracji i definicji muszą się zgadzać co do typów i kolejności.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
    static void zapiszWyniki(const std::string& nazwa,
                            const std::vector<WynikTransakcji>& wyniki,
                            const Podsumowanie& podsumowanie,
                            const std::vector<DaneWalidatora>& walidatorzy);
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 5.1.5. Dodajemy formatowanie ułamków

**Plik:** `Pliki.cpp`. **Co robimy:** dopisujemy include na początku pliku.

Dopisujemy obok innych include. Będziemy zapisywać średnią i procenty z dwoma cyframi po kropce.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#include <iomanip>
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 5.1.6. Rozszerzamy zapis wyników

**Plik:** `Pliki.cpp`. **Co robimy:** zastępujemy całą definicję zapiszWyniki.

Zastępujemy metodę zapisu z etapu 2. Zachowujemy wiersze transakcji i dopisujemy PODSUMOWANIE. Nie zmieniamy odczytu konfiguracji ani transakcji.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
void Pliki::zapiszWyniki(const std::string& nazwa,
                        const std::vector<WynikTransakcji>& wyniki,
                        const Podsumowanie& podsumowanie,
                        const std::vector<DaneWalidatora>& walidatorzy) {
    std::ofstream plik(nazwa);
    if (!plik) {
```

Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Otwieramy plik do zapisu. Istniejącą zawartość tego pliku nadpisujemy przy każdym udanym otwarciu.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
        throw std::runtime_error("Nie mozna zapisac " + nazwa);
    }
    plik << "id_transakcji id_walidatora nadejscie start koniec "
         << "oczekiwanie deadline opoznienie terminowo\n";
    for (const WynikTransakcji& wynik : wyniki) {
        plik << wynik.idTransakcji << ' ' << wynik.idWalidatora << ' '
```

Przez `throw` przerywamy tę operację i przekazujemy opis błędu do `catch` w main. Nie pracujemy dalej na niepoprawnych danych. Operatorem `<<` dopisujemy dane do pliku. Spacje rozdzielają kolumny, a `\n` kończy wiersz. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. W tej pętli bierzemy po kolei elementy listy. Znak `&` pozwala odwołać się do właściwego obiektu; z `const` tylko go odczytujemy.

**Krok 3.** Dopisujemy pod poprzednim fragmentem.

```cpp
             << wynik.nadejscie << ' ' << wynik.start << ' ' << wynik.koniec << ' '
             << wynik.oczekiwanie << ' ' << wynik.deadline << ' '
             << wynik.opoznienie << ' ' << wynik.terminowo << '\n';
    }
    plik << "\nPODSUMOWANIE\n";
    plik << "liczba_obsluzonych " << podsumowanie.liczbaObsluzonych << '\n';
```

Operatorem `<<` dopisujemy dane do pliku. Spacje rozdzielają kolumny, a `\n` kończy wiersz.

**Krok 4.** Dopisujemy pod poprzednim fragmentem.

```cpp
    plik << std::fixed << std::setprecision(2);
    plik << "srednie_oczekiwanie " << podsumowanie.srednieOczekiwanie << '\n';
    plik << "maksymalne_oczekiwanie " << podsumowanie.maksymalneOczekiwanie << '\n';
    plik << "terminowe " << podsumowanie.terminowe << '\n';
    plik << "spoznione " << podsumowanie.spoznione << '\n';
    plik << "czas_konca " << podsumowanie.czasKonca << '\n';
```

Operatorem `<<` dopisujemy dane do pliku. Spacje rozdzielają kolumny, a `\n` kończy wiersz. Przez fixed i setprecision(2) zapisujemy średnią i procent z dwoma cyframi po kropce.

**Krok 5.** Dopisujemy pod poprzednim fragmentem.

```cpp
    for (std::size_t i = 0; i < walidatorzy.size(); ++i) {
        plik << "wykorzystanie " << walidatorzy[i].id << ' '
             << podsumowanie.wykorzystanie[i] << '\n';
    }
    plik.flush();
    if (!plik) {
```

Przechodzimy przez listę według indeksów: zaczynamy od 0, kończymy przed `size()`, a po każdym obrocie zwiększamy i o 1. Operatorem `<<` dopisujemy dane do pliku. Spacje rozdzielają kolumny, a `\n` kończy wiersz. Przekazujemy bufor zapisu do pliku i sprawdzamy stan strumienia. Błąd zapisu zgłaszamy tak samo jak błąd otwarcia.

**Krok 6.** Dopisujemy pod poprzednim fragmentem.

```cpp
        throw std::runtime_error("Blad zapisu " + nazwa);
    }
}
```

Przez `throw` przerywamy tę operację i przekazujemy opis błędu do `catch` w main. Nie pracujemy dalej na niepoprawnych danych.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 5.1.7. Dołączamy Statystyki do main

**Plik:** `main.cpp`. **Co robimy:** dopisujemy include na początku pliku.

Dopisujemy obok naszych dwóch include. Generator i jego funkcję zostawiamy.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
#include "Statystyki.hpp"
```

Przez `#include` dołączamy deklaracje, których używamy niżej. Cudzysłowów używamy przy naszych plikach, a znaków `< >` przy bibliotece C++.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

#### 5.1.8. Wywołujemy pomiary dla obu strategii

**Plik:** `main.cpp`. **Co robimy:** zastępujemy tylko funkcję main.

Zastępujemy funkcję main z etapu 3. Po każdym uruchomieniu liczymy podsumowanie dla jego wyników i czasu końca. Wywołujemy rozszerzone zapiszWyniki z czterema argumentami. Funkcję przygotujLosowania zostawiamy nad main.

Fragmenty wpisujemy w pokazanej kolejności. Drugi fragment dopisujemy pod pierwszym, następny pod poprzednim. Nie zamykamy wcześniej funkcji dodatkową klamrą.

**Krok 1.** Wpisujemy początek wskazanego kodu.

```cpp
int main() {
    try {
        Konfiguracja konfiguracja = Pliki::wczytajKonfiguracje("konfiguracja.txt");
        std::vector<Transakcja> transakcje = Pliki::wczytajTransakcje("zgloszenia.txt");
        std::vector<double> losowania = przygotujLosowania(
            transakcje.size(), konfiguracja.ziarno);
```

W bloku try wykonujemy czynności, które mogą zgłosić błąd. Pasujący catch wpiszemy po zamknięciu tego bloku. Wywołujemy metodę Pliki i zapamiętujemy zwrócone dane. Ścieżkę liczymy od folderu, z którego uruchomiliśmy program. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać.

**Krok 2.** Dopisujemy pod poprzednim fragmentem.

```cpp
        Symulator fifo(konfiguracja, transakcje, losowania, Strategia::FIFO);
        std::vector<WynikTransakcji> wynikiFifo = fifo.uruchom();
        Podsumowanie statystykiFifo = Statystyki::oblicz(
            wynikiFifo, konfiguracja.walidatorzy, fifo.pobierzCzasKonca());
        Pliki::zapiszWyniki("wyniki_fifo.txt", wynikiFifo,
                            statystykiFifo, konfiguracja.walidatorzy);
```

Tworzymy nowy obiekt na ten przebieg. Zaczynamy od czasu 0 i wolnych walidatorów, niezależnie od poprzedniej strategii. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Uruchamiamy cały przebieg i odbieramy listę zakończonych transakcji. Przekazujemy listę wyników, dane walidatorów i czas końca. Odbieramy rekord podsumowania.

**Krok 3.** Dopisujemy pod poprzednim fragmentem.

```cpp
        Symulator fee(konfiguracja, transakcje, losowania, Strategia::FEE);
        std::vector<WynikTransakcji> wynikiFee = fee.uruchom();
        Podsumowanie statystykiFee = Statystyki::oblicz(
            wynikiFee, konfiguracja.walidatorzy, fee.pobierzCzasKonca());
        Pliki::zapiszWyniki("wyniki_fee.txt", wynikiFee,
                            statystykiFee, konfiguracja.walidatorzy);
```

Tworzymy nowy obiekt na ten przebieg. Zaczynamy od czasu 0 i wolnych walidatorów, niezależnie od poprzedniej strategii. W `std::vector` przechowujemy listę elementów. Typ wewnątrz `< >` mówi, co możemy na tę listę dodawać. Uruchamiamy cały przebieg i odbieramy listę zakończonych transakcji. Przekazujemy listę wyników, dane walidatorów i czas końca. Odbieramy rekord podsumowania.

**Krok 4.** Dopisujemy pod poprzednim fragmentem.

```cpp
        std::cout << "Zapisano wyniki_fifo.txt i wyniki_fee.txt\n";
        return 0;
    } catch (const std::exception& blad) {
        std::cerr << "Blad: " << blad.what() << '\n';
```

Przez `cout <<` wypisujemy dane na ekran. Spacje rozdzielają wartości, a `\n` kończy wiersz; `cerr` służy do komunikatów o błędach. W main przez return 0 kończymy program i oznaczamy, że zakończył się poprawnie. Przez `const ...&` korzystamy z istniejących danych bez kopiowania i bez ich zmieniania. Przechwytujemy błąd. Przez what() odczytujemy jego opis, wypisujemy go na cerr i kończymy program kodem 1.

**Krok 5.** Dopisujemy pod poprzednim fragmentem.

```cpp
        return 1;
    }
}
```

Dopisujemy tę część do rozpoczętej wcześniej definicji. Wcięcia pokazują, które instrukcje należą do tego samego bloku.

**Sprawdzamy przed dalszą pracą:** liczymy otwarcia i zamknięcia klamer oraz porównujemy nazwy z instrukcją. Kompilujemy na najbliższym oznaczonym przystanku, po ukończeniu potrzebnych definicji.

**Przystanek: statystyki.** Kończymy podane definicje, zapisujemy pliki i kompilujemy w folderze z kodem.

```text
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp Walidator.cpp Symulator.cpp Statystyki.cpp Pliki.cpp -o symulator.exe
.\symulator.exe
```

**Oczekiwany wynik:** W obu plikach mamy średnią 2.00, maksimum 4, trzy terminowe, zero spóźnionych, czas końca 6 i wykorzystanie walidatora 1 równe 100.00. W README aktualizujemy kompilację: dochodzi Statystyki.cpp. Jeśli kompilator zgłasza błąd, poprawiamy pierwszy komunikat przed następnym krokiem.

### 5.2. Sprawdzamy wzory na małych danych

Dla jednej transakcji `1 3 2 1 5` oczekujemy startu 3, końca 5, oczekiwania 0 i wykorzystania 40%. Dla deadline 4 ta sama transakcja ma opóźnienie 1. Przy samym `0` w zgloszenia.txt oczekujemy pustych rekordów i zerowych statystyk.

- [ ] Dla przykładu trzech transakcji sprawdzamy średnie oczekiwanie 2 i maksymalne 4.
- [ ] Dodajemy transakcję kończącą się w deadline i drugą kończącą się po nim.
- [ ] Przy pustym wejściu przyjmujemy zerowe statystyki. Unikamy dzielenia przez zero.

**Sprawdzamy:** wyniki programu zgadzają się z rachunkami zapisanymi w `oczekiwane_wyniki.txt`.

### 5.3. Przygotowujemy trzy scenariusze

- [ ] `male_obciazenie.txt`: transakcje przychodzą rzadko, np. co 5 kroków, i trwają 2.
- [ ] `przeciazenie.txt`: transakcje przychodzą szybciej, niż walidatorzy zdążą je kończyć, np. 3 nowe na krok przy 2 walidatorach i czasie wykonania 4.
- [ ] `nagly_naplyw.txt`: np. 20 transakcji przychodzi w jednym kroku. W scenariuszach zmieniamy fee i terminy.

**Wynik:** trzy pliki testowe. W opisie podajemy liczbę transakcji i parametry, które wybraliśmy.

### 5.4. Robimy porównanie

- [ ] Dla każdego scenariusza uruchamiamy FIFO i FEE z ziarnami 11, 22 i 33. To 18 przebiegów.
- [ ] Wpisujemy wyniki do `porownanie.csv`. Zapisujemy scenariusz, strategię, ziarno i mierzone wartości.
- [ ] Robimy wykres średniego oczekiwania i wykres odsetka terminowych transakcji. Porównujemy też wykorzystanie walidatorów w tabeli.

**Wynik:** tabela i dwa wykresy. Jeśli wyniki między ziarnami się różnią, pokazujemy różnice.

### 5.5. Piszemy wnioski i sprawozdanie

- [ ] Opisujemy cel, zasady, pliki, klasy i obie kolejki. Dodajemy diagramy z FigJam.
- [ ] Dodajemy analizę złożoności, tabele i wykresy.
- [ ] Piszemy, kto czeka krócej w FEE, kto czeka dłużej i w których danych strategie dają podobne wyniki.

**Na koniec etapu:** `sprawozdanie.pdf`, pliki eksperymentów i wyniki, na których opieramy wnioski.

Przy jednakowej szybkości wykonawców większy stake zmienia podział pracy. Nie zakładamy, że przyspieszy cały system. Dla skończonego wejścia obsługiwanego do końca mierzymy długie oczekiwanie; trwałe głodzenie rozważamy jako ryzyko przy ciągłym napływie.

## Etap 6. Przygotowujemy pokaz i oddanie projektu

Zadanie 6. Przewidujemy 3–5 godzin.

### 6.1. Przygotowujemy folder do oddania

- [ ] Umieszczamy w nim kod, dane testowe, `README.md` i `sprawozdanie.pdf`.
- [ ] Dopisujemy w README sposób kompilacji i uruchomienia.
- [ ] Uruchamiamy program z tego folderu.

**Wynik:** komplet plików, z którego prowadzący może uruchomić projekt.

### 6.2. Układamy pokaz

- [ ] Zaczynamy od trzech transakcji pokazujących różną kolejność FIFO i FEE.
- [ ] Pokazujemy jeden większy scenariusz oraz jego wyniki.
- [ ] Zapisujemy kolejność pokazu w `pokaz.md`.

**Wynik:** plan prezentacji, który możemy przećwiczyć.

### 6.3. Ćwiczymy wyjaśnianie kodu

- [ ] Jedna osoba wyjaśnia kolejkę, druga czas i przydział.
- [ ] Zamieniamy się rolami.
- [ ] Ćwiczymy pytania o stake, deadline, powtarzalność i złożoność.

**Sprawdzamy:** oboje umiemy wyjaśnić części napisane przez drugą osobę.

### 6.4. Sprawdzamy wymagania prowadzącego

- [ ] Używamy tabeli pokrycia efektów poniżej.
- [ ] Przy każdej pozycji wskazujemy plik, wynik albo fragment pokazu.
- [ ] Uzupełniamy brakujące elementy.

**Wynik:** sprawdzona lista wymagań w `opis_projektu.md`.

### 6.5. Robimy próbę prezentacji

- [ ] Przeprowadzamy pokaz od uruchomienia do wniosków.
- [ ] Mierzymy czas. Jeśli prowadzący nie określił długości, przygotowujemy wersję 5–7 minut.
- [ ] Zapisujemy kopię folderu przed oddaniem.

**Na koniec etapu:** komplet projektu i przećwiczony pokaz.

## Pliki wejściowe i przykład kontrolny

W plikach wejściowych używamy liczb oddzielonych spacjami. Opis kolumn trzymamy w dokumentacji.

### konfiguracja.txt

Pierwszy wiersz: liczba walidatorów oraz ziarno losowania. Kolejne wiersze: ID i stake.

```text
1 12345
1 10
```

### zgloszenia.txt

Pierwszy wiersz: liczba transakcji. Kolejne: ID, nadejście, czas wykonania, fee, deadline.

```text
3
1 0 2 1 10
2 0 2 9 10
3 0 2 4 10
```

### Wynik, z którym porównujemy program

| Strategia | Transakcja | Start | Koniec | Oczekiwanie |
|---|---|---:|---:|---:|
| FIFO | T1 | 0 | 2 | 0 |
| FIFO | T2 | 2 | 4 | 2 |
| FIFO | T3 | 4 | 6 | 4 |
| FEE | T2 | 0 | 2 | 0 |
| FEE | T3 | 2 | 4 | 2 |
| FEE | T1 | 4 | 6 | 4 |

W obu wariantach średnie oczekiwanie wynosi 2, maksymalne 4, a wykorzystanie jedynego walidatora 100%. Te dane sprawdzają kolejność. Do oceny różnic potrzebujemy także scenariuszy z etapu 5.

### Dane, które odrzucamy

Odrzucamy brak pliku, błędny zapis liczby i liczbę rekordów niezgodną z pierwszym wierszem. Sprawdzamy też:

1. Liczba walidatorów co najmniej 1; liczba transakcji co najmniej 0.
2. ID są dodatnie i nie powtarzają się w obrębie transakcji ani w obrębie walidatorów.
3. Stake i czas wykonania są większe od 0.
4. Nadejście i fee są co najmniej 0.
5. Deadline nie jest wcześniejszy niż nadejście. Może być wcześniejszy niż możliwe zakończenie zadania.

## Wzory do statystyk

| Liczona wartość | Wzór |
|---|---|
| Oczekiwanie | start − nadejście |
| Opóźnienie | max(0, koniec − deadline) |
| Terminowo | koniec <= deadline |
| Średnie oczekiwanie | suma oczekiwań / liczba transakcji |
| Wykorzystanie walidatora | czas jego zajętości / czas od 0 do końca przebiegu × 100% |

W pliku wyników zapisujemy: ID transakcji, ID walidatora, nadejście, start, koniec, oczekiwanie, deadline, opóźnienie i terminowość. Dodajemy podsumowanie przebiegu. Dla pustego wejścia przyjmujemy wartości 0.

## Pokrycie efektów uczenia się

Sprawdzamy tę tabelę przed oddaniem. Przypisanie opiera się na macierzy zadań, którą podaliśmy.

| Efekt | Co przygotowujemy |
|---|---|
| K_W05 | Opis modelu, klasy C++, FIFO i FEE |
| K_U05 | Wyniki eksperymentów i ich omówienie |
| K_U08 | Uzasadnienie modelu, algorytmów i struktur danych |
| K_U09 | Opis czasu działania i pamięci w O() |
| K_U15 | Działający symulator i obie kolejki |
| K_K09 | Wyjaśnienie decyzji w opisie i na obronie |
| K_K10 | Sprawozdanie i prezentację wyników |
