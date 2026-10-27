# Analiza złożoności obliczeniowej i pamięciowej symulatora PoS

## 4.1. Znaczenie symboli
- **$N$**: Liczba transakcji (zgłoszeń w pliku wejściowym).
- **$K$**: Liczba walidatorów.
- **$T$**: Liczba wykonanych kroków czasu symulacji (kroki symulacji $t$, a nie czas rzeczywisty komputera).

## 4.2. Opis złożoności kolejek
- **Kolejka FIFO (`std::queue`)**: Dodanie (`push`) oraz usunięcie (`pop`) elementu kosztują $O(1)$.
- **Kolejka FEE (`std::priority_queue`)**: Dodanie (`push`) oraz usunięcie (`pop`) elementu kosztują $O(\log N)$, natomiast podejrzenie pierwszego elementu (`top`) kosztuje $O(1)$.
- **Wyjaśnienie**: $O(1)$ oznacza stały koszt operacji niezależnie od liczby elementów w kolejce, natomiast $O(\log N)$ oznacza, że koszt operacji rośnie logarytmicznie wraz z rozmiarem kolejki.

## 4.3. Analiza pętli i algorytmów
- **Sprawdzanie walidatorów w krokach symulacji**: W każdym z $T$ kroków sprawdzamy $K$ walidatorów pod kątem zakończenia pracy, co daje złożoność $O(TK)$.
- **Wybór wykonawcy oraz liczenie wykorzystania**: Wybór walidatora z uwzględnieniem przedziałów stake dla $N$ przydziałów wymaga przejścia po liście walidatorów i kosztuje $O(NK)$. Dodatkowo liczenie wykorzystania walidatorów w module statystyk polega na przeglądaniu $N$ wyników dla każdego z $K$ walidatorów, co również wynosi $O(NK)$.
- **Wstępne sortowanie**: Sortowanie listy transakcji kosztuje $O(N \log N)$, a danych walidatorów $O(K \log K)$.

## 4.4. Koszt całkowity i pamięć
- **Całkowity czas działania**: Dla tego modelu łączny koszt obliczeniowy wynosi $O(N \log(N + 1) + K \log(K + 1) + TK + NK)$. Dodanie wartości $+1$ w logarytmach pozwala objąć przypadki skrajne, takie jak puste wejście oraz obecność jednego walidatora.
- **Porównanie kolejek**: Koszt samych operacji na kolejkach różni się ($O(1)$ dla FIFO vs $O(\log N)$ dla FEE), jednak po uwzględnieniu wstępnego sortowania $O(N \log N)$ obie strategie uzyskują tę samą ogólną granicę złożoności.
- **Złożoność pamięciowa**: Szacujemy pamięć jako $O(N + K)$, ponieważ przechowujemy w pamięci listy transakcji, losowania ułamków stake, obiekty walidatorów oraz wektor wyników.

## 4.5. Ograniczenia modelu
- **Długie przerwy w nadejściach**: Długa przerwa czasowa między kolejnymi transakcjami powoduje wzrost liczby kroków symulacji $T$, co podnosi całkowitą liczbę operacji w pętli $TK$.
- **Głodzenie transakcji (starvation)**: W strategii FEE transakcja z niską opłatą (`fee`) może czekać bardzo długo na obsługę, jeśli do systemu ciągle napływają nowe zgłoszenia z wyższym `fee`.