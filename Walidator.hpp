<!doctype html><html lang="pl"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Symulator PoS — obsługa transakcji</title><style>body{font-family:Arial,sans-serif;line-height:1.5;margin:24px;color:#111}code{background:#f1f1f1;padding:2px 4px;border-radius:4px}table{border-collapse:collapse;width:100%;margin:16px 0}th,td{border:1px solid #ccc;padding:8px;text-align:left;vertical-align:top}h1,h2,h3{margin-top:24px}</style>
<h1>Symulator PoS — obsługa transakcji</h1>
<p>Tworzymy projekt w parze na przedmiot Podstawy informatyki. Program piszemy w C++. Ten dokument opisuje zasady, których trzymamy się podczas pracy.</p>
<h2>1.1. Co budujemy i jak działa symulacja</h2>
<p>Budujemy symulator obsługi transakcji w czasie dyskretnym. Czas liczymy jako kolejne kroki: 0, 1, 2, 3… Jeden krok jest umowną jednostką czasu. Program nie musi czekać rzeczywistej sekundy między krokami.</p>
<p><strong>Transakcja</strong> jest zgłoszeniem do wykonania. Ma czas nadejścia, czas wykonania, opłatę i termin zakończenia. <strong>Kolejka</strong> przechowuje transakcje, które już nadeszły i czekają na obsługę.</p>
<p>Korzystamy z uproszczonego pomysłu Proof of Stake (PoS): liczba monet walidatora wpływa na jego szansę wyboru. Nasz program symuluje kolejkę i przydział pracy. Nie implementujemy pełnego blockchaina ani konsensusu.</p>
<h3>Wybór transakcji</h3>
<p>Porównujemy dwie strategie na tych samych danych:</p>
<table><tr><th>Strategia</th><th>Którą oczekującą transakcję wybieramy</th></tr><tr><td>FIFO</td><td>Tę, która najwcześniej nadeszła. Przy tym samym czasie nadejścia wybieramy niższe ID.</td></tr><tr><td>FEE</td><td>Tę, która ma największą opłatę. Przy równych opłatach wybieramy wcześniejsze nadejście, a potem niższe ID.</td></tr></table>
<p>Opłata (<code>fee</code>) jest naszym priorytetem finansowym. Deadline służy do oceny terminowości, a nie do ustalania kolejności obsługi.</p>
<h3>Wybór walidatora</h3>
<p>Po wybraniu transakcji losujemy wykonawcę spośród wolnych walidatorów. Szansa danego walidatora to jego stake podzielony przez sumę stake wszystkich aktualnie wolnych walidatorów.</p>
<p>W etapie 3 ta logika została dopisana i działa zgodnie z ziarniem losowania.</p>
<h2>Pliki wejściowe i wyniki</h2>
<p>Wszystkie pliki tekstowe zostały przeredagowane do formatu czytelnego, z nazwami kolumn i prostym opisem znaczenia wartości. Przykładem są <code>konfiguracja.txt</code>, <code>zgloszenia.txt</code>, <code>wyniki_fifo.txt</code> i <code>wyniki_fee.txt</code>.</p>
<p>W formacie tekstowym zastosowano nagłówki typu:</p>
<ul><li><code># kolumny: ...</code></li><li><code>seed=12345</code></li><li><code>transaction_id arrival_time execution_time fee deadline</code></li></ul>
<p>To pozwala szybko odczytać znaczenie każdej liczby bez potrzeby zapamiętywania kolejności pól.</p>
<h2>Stan etapu 3</h2>
<p>Etap 3 jest już zakończony. Repo zawiera logikę FEE, wybór walidatora na podstawie stake i zapis do osobnego pliku <code>wyniki_fee.txt</code>. Etapy 4–6 nie były robione, zgodnie z wymaganiem ograniczenia do etapu 3.</p>
