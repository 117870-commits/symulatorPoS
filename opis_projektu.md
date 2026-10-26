<!doctype html><html lang="pl"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Plan pracy nad symulatorem PoS</title><style>body{font-family:Arial,sans-serif;line-height:1.5;margin:24px;color:#111}code{background:#f1f1f1;padding:2px 4px;border-radius:4px}table{border-collapse:collapse;width:100%;margin:16px 0}th,td{border:1px solid #ccc;padding:8px;text-align:left;vertical-align:top}h1,h2,h3{margin-top:24px}</style>
<h1>Plan pracy nad symulatorem PoS</h1>
<p>Mamy przygotowany schemat działania i diagram klas. Przygotowujemy opis zasad, dane testowe i program w C++.</p>
<p>Przy każdym kroku zapisujemy działania i oczekiwany wynik. Odhaczamy kroki po sprawdzeniu wyniku. Na koniec spotkania notujemy jedną rzecz, od której zaczniemy następnym razem.</p>
<h2>Co budujemy</h2>
<p>Użytkownik wysyła transakcję. Transakcja czeka w kolejce, aż zwolni się walidator. Walidator wykonuje jedno zadanie naraz. Porównujemy dwie kolejki: według kolejności nadejścia i według opłaty.</p>
<p>W projekcie korzystamy z uproszczonego pomysłu PoS: większy stake daje większą szansę wyboru wykonawcy. Nie odtwarzamy całego blockchaina ani konsensusu.</p>
<table><tr><th>Nazwa</th><th>Co oznacza u nas</th></tr><tr><td>FIFO</td><td>Najpierw obsługujemy transakcję, która wcześniej przyszła</td></tr><tr><td>FEE</td><td>Najpierw obsługujemy oczekującą transakcję z najwyższą opłatą</td></tr><tr><td>Stake</td><td>Liczba określająca szansę wyboru walidatora</td></tr><tr><td>Deadline</td><td>Termin, do którego transakcja powinna skończyć obsługę</td></tr><tr><td>Czas symulacji t</td><td>Licznik kroków: 0, 1, 2, 3…</td></tr></table>
<p>Przy równym fee wybieramy wcześniejsze nadejście, a potem mniejsze ID. Dla FIFO transakcje z jednakowym czasem nadejścia porządkujemy po ID.</p>
<p>Walidatorzy mają jednakową szybkość. Każdy walidator zachowuje swój stake przez cały przebieg. Walidatorzy mogą mieć różne liczby monet. Rozpoczętej pracy nie przerywamy. Spóźnioną transakcję wykonujemy do końca.</p>
<h2>Etap 3. Dodajemy FEE i losowanie walidatora</h2>
<p>Etap 3 jest już zrealizowany i zatwierdzony w repozytorium. Dodatkowo w plikach wejściowych i wynikowych wprowadzono czytelne nagłówki z nazwami kolumn.</p>
<ul>
  <li>dodanie strategii <code>Strategia::FIFO</code> i <code>Strategia::FEE</code></li>
  <li>kolejka <code>std::priority_queue</code> z komparatorem po fee, następnie nadejściu, potem ID</li>
  <li>wybór wolnego walidatora według stake i ziarna losowania</li>
  <li>uruchomienie dwóch przebiegów: FIFO oraz FEE</li>
  <li>zapis do <code>wyniki_fifo.txt</code> i <code>wyniki_fee.txt</code></li>
  <li>pliki tekstowe w formacie czytelnym, z nazwami kolumn i opisami</li>
</ul>
<p>Dla danych testowych z <code>konfiguracja.txt</code> i <code>zgloszenia.txt</code> oba przebiegi pracują poprawnie, a w wynikach widać kolumny z nazwami i wartościami opisanymi wprost.</p>
<p>Etap 3 jest zakończony. Etapy 4, 5 i 6 nie były wykonywane, zgodnie z prośbą ograniczenia do samego etapu 3.</p>
