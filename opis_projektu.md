# Symulator PoS — obsługa transakcji

Tworzymy projekt w parze na przedmiot Podstawy informatyki. Program piszemy w C++. Ten dokument opisuje zasady, których trzymamy się podczas pracy.

## 1.1. Co budujemy i jak działa symulacja

Budujemy symulator obsługi transakcji w czasie dyskretnym. Czas liczymy jako kolejne kroki: 0, 1, 2, 3… Jeden krok jest umowną jednostką czasu. Program nie musi czekać rzeczywistej sekundy między krokami.

**Transakcja** jest zgłoszeniem do wykonania. Ma czas nadejścia, czas wykonania, opłatę i termin zakończenia. **Kolejka** przechowuje transakcje, które już nadeszły i czekają na obsługę. **Walidator** jest wykonawcą: obsługuje jedną transakcję naraz, a po jej zakończeniu może dostać kolejną.

Korzystamy z uproszczonego pomysłu Proof of Stake (PoS): liczba monet walidatora wpływa na jego szansę wyboru. Nasz program symuluje kolejkę i przydział pracy. Nie implementujemy pełnego blockchaina, bloków, kryptografii ani uzgadniania wspólnego stanu przez sieć.

### Wybór transakcji

Porównujemy dwie strategie na tych samych danych:

| Strategia | Którą oczekującą transakcję wybieramy |
|---|---|
| FIFO | Tę, która najwcześniej nadeszła. Przy tym samym czasie nadejścia wybieramy niższe ID. |
| FEE | Tę, która ma największą opłatę. Przy równych opłatach wybieramy wcześniejsze nadejście, a potem niższe ID. |

Opłata (`fee`) jest naszym priorytetem finansowym. Deadline służy do oceny terminowości, a nie do ustalania kolejności obsługi.

### Wybór walidatora

Po wybraniu transakcji losujemy wykonawcę spośród wolnych walidatorów. Szansa danego walidatora to jego stake podzielony przez sumę stake wszystkich aktualnie wolnych walidatorów.

Przykład: jeśli wolni walidatorzy mają 10 i 30 monet, ich szanse wynoszą odpowiednio 25% i 75%. Zajętych walidatorów pomijamy. Jeśli żaden nie jest wolny, transakcja zostaje w kolejce.

**Stake jest stały podczas przebiegu.** Walidatorzy mogą mieć różne wartości, np. 10, 30 i 60 monet. Nie doliczamy nagród i nie odejmujemy monet. Wszyscy wykonują zadania z taką samą szybkością, więc większy stake nie skraca czasu wykonania transakcji. Nie dzielimy walidatorów na małe, średnie i duże serwery.

### Kolejność działań w jednym przebiegu

1. Ustawiamy czas `t = 0`, pustą kolejkę i wszystkich walidatorów jako wolnych.
2. Dodajemy do kolejki transakcje, których czas nadejścia wynosi `t`.
3. Przydzielamy transakcje według wybranej strategii, dopóki kolejka nie jest pusta i mamy wolnych walidatorów. Czas zakończenia zadania obliczamy jako `start + czas wykonania`.
4. Sprawdzamy, czy zostały przyszłe nadejścia, transakcje w kolejce lub zajęci walidatorzy. Jeśli nic nie zostało, kończymy przebieg.
5. Jeśli praca została, zwiększamy `t` o 1. Kończymy zadania, których czas zakończenia jest równy nowemu `t`: zapisujemy wyniki i zwalniamy walidatorów. Wracamy do kroku 2.

Przykład: transakcja zaczyna się w czasie 0 i trwa 3 kroki. Kończy się w czasie 3. W tym samym kroku zwolniony walidator może rozpocząć kolejną transakcję.

### Pozostałe zasady

- Wczytanie transakcji z pliku nie oznacza jej nadejścia. Dodajemy ją do kolejki dopiero w zapisanym czasie nadejścia.
- Rozpoczętej transakcji nie przerywamy, nawet jeśli później nadejdzie transakcja z większym fee.
- Spóźnione transakcje wykonujemy do końca. Zakończenie dokładnie w deadline oznacza wykonanie w terminie.
- Dla każdego przebiegu tworzymy nowy obiekt Symulator: z czasem 0, pustą kolejką, pustymi wynikami i wolnymi walidatorami. Dla FIFO i FEE przygotowujemy tę samą listę N losowań z zakresu [0, 1), gdzie N jest liczbą transakcji. Zużywamy jedno losowanie na kolejny przydział. Wolnych walidatorów rozpatrujemy według ID.
- Ta sama strategia, te same dane i to samo ziarno losowania mają dawać powtarzalne wyniki w naszym programie. FIFO i FEE mogą dawać różne wyniki, mimo wspólnej listy losowań.

## 1.2. Dane i klasy

### Dane transakcji

| Pole | Znaczenie | Zasada |
|---|---|---|
| `id` | Numer transakcji | Dodatni i niepowtarzalny wśród transakcji |
| `nadejscie` | Krok, w którym transakcja trafia do kolejki | Co najmniej 0 |
| `czasWykonania` | Liczba kroków potrzebnych do wykonania | Większa od 0 |
| `fee` | Opłata określająca priorytet w FEE | Co najmniej 0 |
| `deadline` | Najpóźniejszy termin zakończenia bez spóźnienia | Nie wcześniejszy niż nadejście |

Na początku używamy całkowitych wartości czasu i opłat. Deadline może być za krótki, by zdążyć z wykonaniem. Taka transakcja jest poprawnym wejściem i może zakończyć się jako spóźniona.

### Dane walidatora

| Pole | Znaczenie |
|---|---|
| `id` | Dodatni, niepowtarzalny numer walidatora |
| `stake` | Dodatnia liczba monet, stała podczas przebiegu |
| `zajety` | Czy walidator teraz wykonuje zadanie: true albo false |
| `aktualna` | Dane wykonywanej transakcji; korzystamy z nich tylko przy zajętym walidatorze |
| `start` | Czas rozpoczęcia aktualnego zadania |
| `koniec` | Czas zakończenia aktualnego zadania |

Walidator zaczyna jako wolny. Metoda rozpocznij ustawia zadanie i czasy, a zakoncz tworzy wynik i zwalnia wykonawcę. Pola pracy są prywatne, więc inne części programu zmieniają stan przez te metody. Numer transakcji i numer walidatora mogą być takie same, bo dotyczą różnych rodzajów danych.

### Prosta architektura

| Element | Zadanie | Pliki |
|---|---|---|
| `Dane` | Definiujemy rekordy Transakcja, DaneWalidatora, Konfiguracja, WynikTransakcji, Podsumowanie i wyliczenie Strategia | Dane.hpp |
| `Walidator` | Przechowujemy stan wykonawcy i obsługujemy rozpoczęcie oraz zakończenie zadania | Walidator.hpp i Walidator.cpp |
| `Symulator` | Prowadzimy czas, obie kolejki, losowanie wykonawcy i zbieranie wyników | Symulator.hpp i Symulator.cpp |
| `Statystyki` | Liczymy średnie i maksymalne oczekiwanie, terminowość oraz wykorzystanie walidatorów | Statystyki.hpp i Statystyki.cpp |
| `Pliki` | Wczytujemy, sprawdzamy i zapisujemy pliki tekstowe | Pliki.hpp i Pliki.cpp |
| `main` | Łączymy części programu, przygotowujemy wspólne losowania i uruchamiamy dwa osobne przebiegi | main.cpp |

Tworzymy cztery klasy i jeden wspólny nagłówek danych. Transakcja jest prostym rekordem, dlatego nie potrzebuje osobnego Transakcja.cpp. Nie używamy dziedziczenia ani ręcznego przydzielania pamięci. Wyjaśnienia trzymamy w planie; przykłady kodu nie zawierają komentarzy.

Transakcje, walidatorów, losowania i wyniki przechowujemy w std::vector. FIFO obsługujemy przez std::queue, a FEE przez std::priority_queue z własnym komparatorem PorownajFee. Przed symulacją sortujemy transakcje po nadejściu i ID, a początkowe dane walidatorów po ID.

## 1.3. Pliki wejściowe

Od etapu 3 używamy czytelnego formatu inspirowanego plikami systemd: `[sekcja]`, `pole = wartość` i komentarze `#` lub `;`. To uproszczony format własny, nie pełna składnia systemd. Puste linie i komentarze są ignorowane; komentarz może też wystąpić po wartości. Nie podajemy liczby rekordów — program zlicza sekcje.

`konfiguracja.txt` zawiera jedną sekcję `[symulacja]` z nieujemnym `ziarno` oraz co najmniej jedną sekcję `[walidator]` z dodatnimi `id` i `stake`. `zgloszenia.txt` zawiera sekcje `[transakcja]` z polami `id`, `nadejscie`, `czas_wykonania`, `fee`, `deadline`. Pusta lista transakcji to pusty plik albo same komentarze. Przykłady znajdują się w plikach wejściowych i aktualizacji planu.

Program odrzuca brakujące lub nadmiarowe pola, powtórzone pola i ID, nieznane sekcje, niepoprawne liczby i wartości spoza dopuszczalnych zakresów; komunikat zawiera nazwę pliku i numer wiersza. Stare pliki w formacie samych liczb trzeba przepisać do nowego formatu.

### Planowany zapis wyników

Każda strategia zapisuje osobny plik: `wyniki_fifo.txt` lub `wyniki_fee.txt`. W sekcji `[wynik]` umieszczamy nazwane pola: ID transakcji i walidatora, nadejście, start, koniec, oczekiwanie, deadline, opóźnienie i terminowość (`tak` lub `nie`). Na końcu dodajemy podsumowanie przebiegu. Zapis wyników implementujemy w etapie 2, a pełne statystyki w etapie 5.

## Stan pracy

Przechodzimy przez te same sześć etapów co na tablicy 03 / Plan pracy w Figmie. Odhaczamy działania po sprawdzeniu wyniku. Pod każdym etapem zapisujemy, co rzeczywiście zrobiliśmy, w których plikach i jaki był wynik testu. Pola poniżej uzupełniamy w tym dokumencie albo po wklejeniu do Google Docs.

Etapy 2 i 3 zostały zaimplementowane i sprawdzone testami. Poniższe listy pozostają materiałem do dalszej pracy; zapis wykonanych prób znajduje się pod etapem 3.

### Etap 1. Ustalamy zasady

Co robimy:

- [ ] Piszemy cel projektu i zasady FIFO/FEE w opis_projektu.md.
- [ ] Opisujemy pola transakcji i walidatora oraz zadania klas.
- [ ] Tworzymy konfiguracja.txt i zgloszenia.txt; opisujemy kolumny.
- [ ] Liczymy 3 transakcje: kolejność, start, koniec i oczekiwanie.
- [ ] Sprawdzamy zasady razem; uzgadniamy format danych i metody.

Na koniec etapu:

- opis_projektu.md: cel, reguły, pola i zadania klas.
- konfiguracja.txt: sekcja symulacji z ziarnem i sekcje walidatorów z ID i stake.
- zgloszenia.txt: sekcje transakcji z nazwanymi polami ID, nadejście, czas wykonania, fee i deadline.
- oczekiwane_wyniki.txt: obliczenia FIFO i FEE;
- start, koniec, oczekiwanie oraz kolejność.
- Test: potrafimy odtworzyć ten sam przykład.

Nasz zapis postępu:

- Status: zakończony funkcjonalnie; testy opisane poniżej.
- Data aktualizacji: ................................
- Co zostało zrobione: ................................
- Utworzone lub zmienione pliki i ich zawartość: ................................
- Sprawdzony przykład lub test: ................................
- Oczekiwany wynik i otrzymany wynik: ................................
- Co wymaga poprawy: ................................
- Następny krok: ................................

### Etap 2. Uruchamiamy FIFO

Co robimy:

- [ ] Kompilujemy main.cpp; zapisujemy uruchomienie w README.md.
- [ ] Tworzymy 8 plików kodu; wczytujemy dane do std::vector.
- [ ] Sprawdzamy dane; sortujemy po nadejściu i ID.
- [ ] Dodajemy FIFO, czas i wybór pierwszego wolnego walidatora.
- [ ] Zapisujemy wyniki FIFO; sprawdzamy przykład i puste wejście.

Na koniec etapu:

- Kod: main.cpp, Dane.hpp, Walidator, Pliki i Symulator.
- Instrukcja pisania: etap 2 planu, po 1–6 linii kodu.
- README.md: kompilacja, uruchomienie i pliki wejścia.
- wyniki_fifo.txt: rekord każdej zakończonej transakcji.
- Test: nadejście 0 i czas 3 dają start 0, koniec 3.
- Dla 3 transakcji mamy 3 wyniki bez powtórzeń.
- Błędne wejście daje komunikat z przyczyną.

Nasz zapis postępu:

- Status: zakończony funkcjonalnie; testy opisane poniżej.
- Data aktualizacji: ................................
- Co zostało zrobione: ................................
- Utworzone lub zmienione pliki i ich zawartość: ................................
- Sprawdzony przykład lub test: ................................
- Oczekiwany wynik i otrzymany wynik: ................................
- Co wymaga poprawy: ................................
- Następny krok: ................................

### Etap 3. Dodajemy FEE i stake

Co robimy:

- [x] Rozbudowujemy kod z etapu 2 według punktów 3.1–3.5 planu; dodajemy Strategia { FIFO, FEE }.
- [x] W FEE używamy priority_queue: fee, nadejście, potem ID.
- [x] Losujemy tylko wolnych walidatorów według ich stake.
- [x] Generujemy N losowań z seed; każdy przebieg ma świeży stan.
- [x] Uruchamiamy oba warianty; sprawdzamy remisy i powtarzalność.

Na koniec etapu:

- wyniki_fifo.txt i wyniki_fee.txt: po jednym wyniku
- dla każdej transakcji, bez powtórzeń.
- Test FEE: opłaty 1, 9, 4 dają kolejność T2, T3, T1.
- Remis: wcześniejsze nadejście, potem niższe ID.
- Ten sam wariant, dane i seed dają ten sam wynik.
- Zajęty walidator nie bierze udziału w losowaniu.
- FIFO i FEE używają wspólnej listy losowań.

Nasz zapis postępu:

- Status: zakończony; wyniki prób poniżej.
- Data aktualizacji: ................................
- Co zostało zrobione: ................................
- Utworzone lub zmienione pliki i ich zawartość: ................................
- Sprawdzony przykład lub test: ................................
- Oczekiwany wynik i otrzymany wynik: ................................
- Co wymaga poprawy: ................................
- Następny krok: ................................

#### Wyniki weryfikacji etapów 2–3 — 26.10.2026

- Zaimplementowano dwa niezależne przebiegi FIFO i FEE oraz wybór wolnego walidatora proporcjonalnie do stake, ze wspólną listą losowań.
- Przykład kontrolny: FIFO 1,2,3 i FEE 2,3,1; trzy wyniki na strategię.
- Sprawdzono remisy, dwóch walidatorów, brak nakładających się zadań, terminowość, powtarzalność, puste wejście i błędne pliki. Testy: `python tests/test_etap3.py`.
- Następny etap: analiza złożoności; statystyki zbiorcze i eksperymenty dopiero w etapie 5.

### Etap 4. Liczymy złożoność

Co robimy:

- [ ] Definiujemy N: transakcje, K: walidatorzy, T: kroki czasu.
- [ ] Porównujemy dodanie, pobranie i podgląd w FIFO oraz FEE.
- [ ] Liczymy sortowanie, skan walidatorów i wybór wykonawcy.
- [ ] Zapisujemy koszt całej symulacji i pamięci w zlozonosc.md.
- [ ] Opisujemy długie oczekiwanie niskiego fee i puste kroki czasu.

Na koniec etapu:

- zlozonosc.md: znaczenie N, K i T oraz tabela kolejek.
- FIFO: dodanie i pobranie O(1); FEE: O(log N).
- Sortowanie: O(N log N + K log K); kroki czasu: O(TK).
- Przydziały i wykorzystanie: O(NK); pamięć: O(N + K).
- Całość: O(N log(N+1) + K log(K+1) + TK + NK).
- Sprawdzamy: wskazujemy w kodzie źródło każdego kosztu.

Nasz zapis postępu:

- Status (niezaczęty / w trakcie / zakończony): ................................
- Data aktualizacji: ................................
- Co zostało zrobione: ................................
- Utworzone lub zmienione pliki i ich zawartość: ................................
- Sprawdzony przykład lub test: ................................
- Oczekiwany wynik i otrzymany wynik: ................................
- Co wymaga poprawy: ................................
- Następny krok: ................................

### Etap 5. Badamy wyniki

Co robimy:

- [ ] Dodajemy Podsumowanie oraz Statystyki.hpp/.cpp.
- [ ] Sprawdzamy średnią, maksimum, deadline i wykorzystanie.
- [ ] Tworzymy 3 scenariusze: mały, przeciążony i nagły napływ.
- [ ] Badamy FIFO i FEE z seed 11, 22 i 33: 18 przebiegów.
- [ ] Robimy porownanie.csv, 2 wykresy i sprawozdanie.pdf.

Na koniec etapu:

- Kod: Statystyki; podsumowanie w Pliki i main.
- Instrukcja dodania kodu: punkt 5.1 planu.
- Dane: male_obciazenie.txt, przeciazenie.txt, nagly_naplyw.txt.
- porownanie.csv: scenariusz, strategia, seed i pomiary.
- Wykresy: średnie oczekiwanie i odsetek terminowych.
- sprawozdanie.pdf: cel, klasy, złożoność, wyniki i wnioski.
- Test: średnia 2, maksimum 4; puste wejście daje zera.

Nasz zapis postępu:

- Status (niezaczęty / w trakcie / zakończony): ................................
- Data aktualizacji: ................................
- Co zostało zrobione: ................................
- Utworzone lub zmienione pliki i ich zawartość: ................................
- Sprawdzony przykład lub test: ................................
- Oczekiwany wynik i otrzymany wynik: ................................
- Co wymaga poprawy: ................................
- Następny krok: ................................

### Etap 6. Przygotowujemy obronę

Co robimy:

- [ ] Zbieramy kod, dane, README.md i raport w jednym folderze.
- [ ] W pokaz.md zapisujemy kolejność prezentacji i przykłady.
- [ ] Ćwiczymy wyjaśnianie klas, kolejek, stake i statystyk.
- [ ] Sprawdzamy 7 efektów; wskazujemy plik albo część pokazu.
- [ ] Uruchamiamy projekt z folderu do oddania; robimy kopię.

Na koniec etapu:

- Folder do oddania: kod .hpp/.cpp, dane, README.md
- i sprawozdanie.pdf; dołączamy wyniki eksperymentów.
- pokaz.md: mały przykład, większy test, wyniki i kod.
- opis_projektu.md: lista 7 efektów i miejsce ich spełnienia.
- Test: oboje kompilujemy, uruchamiamy i wyjaśniamy kod.
- Próba: pokaz 5–7 minut, jeśli prowadzący nie podał czasu.
- Na koniec zapisujemy kopię całego folderu.

Nasz zapis postępu:

- Status (niezaczęty / w trakcie / zakończony): ................................
- Data aktualizacji: ................................
- Co zostało zrobione: ................................
- Utworzone lub zmienione pliki i ich zawartość: ................................
- Sprawdzony przykład lub test: ................................
- Oczekiwany wynik i otrzymany wynik: ................................
- Co wymaga poprawy: ................................
- Następny krok: ................................

Szczegółową instrukcję czytamy po kolei w PLAN_PROJEKTU_POS.md: podstawowy program w etapie 2, FEE i losowanie w etapie 3, statystyki w etapie 5. Kod dopisujemy krótkimi fragmentami, a po przystankach porównujemy wynik z przykładem.
