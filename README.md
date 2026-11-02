# Symulator Proof of Stake

Projekt w C++17 pokazuje obsługę transakcji przez ograniczoną liczbę walidatorów, czyli wykonawców. Czas płynie w dyskretnych krokach. Porównujemy dwie kolejności obsługi: FIFO (według nadejścia) i FEE (najpierw wyższa opłata). Wolny walidator jest losowany z uwzględnieniem jego stawki, zgodnie z uproszczonym modelem Proof of Stake.

Celem jest sprawdzenie, jak sposób obsługi wpływa na oczekiwanie, dotrzymanie terminów i wykorzystanie walidatorów. Każdy z trzech dużych scenariuszy ma 6000 transakcji i 300 walidatorów.

Autorzy: Vladyslav Rosol i Olesia Voronina, grupa 22 INF-SP/B.

## Pliki

```text
.
├── symulatorPoS/
│   ├── symulator_gui.exe        # Program z oknem
│   ├── symulator.exe            # Program konsolowy
│   ├── eksperymenty.exe         # Odtworzenie danych i badań dla FIFO/FEE
│   ├── main.cpp, Gui.cpp        # Wejście konsoli i interfejs okienkowy
│   ├── Eksperymenty.cpp         # Generowanie scenariuszy i porównanie wyników
│   ├── Dane.hpp                 # Dane transakcji, konfiguracji i wyników
│   ├── Symulator.hpp/.cpp       # Upływ czasu, kolejki i przydział transakcji
│   ├── Walidator.hpp/.cpp       # Stan i praca wykonawcy
│   ├── Pliki.hpp/.cpp           # Odczyt danych i zapis wyników
│   ├── Statystyki.hpp/.cpp      # Oczekiwanie, terminy i wykorzystanie
│   ├── Przebiegi.hpp/.cpp       # Uruchamianie obu strategii na tych samych danych
│   ├── WidokWynikow.hpp         # Sortowanie tabeli w GUI
│   ├── gui.rc, gui.manifest     # Zasoby i wygląd okna Windows
│   ├── build.cmd, testy.cmd     # Kompilacja i uruchamianie testów C++
│   ├── konfiguracja.txt        # Walidatorzy, stawki i ziarno losowania
│   ├── zgloszenia.txt          # Domyślny zestaw, taki sam jak przeciążenie
│   ├── male_obciazenie.txt     # Spokojny napływ transakcji
│   ├── przeciazenie.txt        # Napływ szybszy niż obsługa
│   ├── nagly_naplyw.txt        # Wszystkie transakcje nadchodzą jednocześnie
│   ├── wyniki_fifo.txt, wyniki_fee.txt  # Wyniki domyślnego zestawu
│   ├── porownanie.csv, wykorzystanie.csv, grupy_fee.csv  # Tabele badań
│   ├── eksperymenty/           # Pełne wyniki 18 przebiegów
│   ├── przyklady/              # Mały zestaw do pokazania kolejności obsługi
│   ├── oczekiwane_wyniki.txt   # Oczekiwany wynik małego przykładu
│   ├── tests/                 # Pięć testów C++
│   ├── sprawozdanie.pdf        # Opis, analiza złożoności i wnioski dla prowadzącego
│   └── diagramy.pdf            # Diagram działania i diagram klas
└── PRZYGOTOWANIE/
    ├── diagramy.pdf            # Diagramy do nauki
    └── przygotowanie_do_obrony.pdf  # Pokaz do 5 minut, pytania i wyjaśnienia
```

## Uruchomienie

Na Windows rozpakuj pobrane repozytorium. Uruchom `symulatorPoS/symulator_gui.exe` albo w PowerShellu:

```powershell
cd symulatorPoS
.\symulator.exe
```

Konsola domyślnie porównuje FIFO i FEE, zapisuje wyniki i podaje statystyki co 100 zakończonych transakcji. Inny scenariusz wybierzesz np. tak:

```powershell
.\symulator.exe --zgloszenia nagly_naplyw.txt
```

Ponowne uruchomienie nadpisuje wyniki. `eksperymenty.exe` odtwarza również konfigurację i dane scenariuszy.

Do kompilacji potrzebny jest GCC/MinGW z C++17 oraz `windres` w `PATH`. W folderze `symulatorPoS`:

```powershell
.\testy.cmd           # Kompilacja i uruchomienie pięciu testów C++
.\build.cmd --tests   # Kompilacja trzech programów i uruchomienie testów
```
