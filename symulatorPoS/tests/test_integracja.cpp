#include "Pliki.hpp"
#include "Przebiegi.hpp"
#include "Statystyki.hpp"
#include "Symulator.hpp"
#include <cassert>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

int main() {
    Konfiguracja k; k.ziarno=11; k.walidatorzy={{1,10}};
    std::vector<Transakcja> dane={{1,0,2,1,10},{2,0,2,9,10},{3,0,2,4,10}};
    auto losy=przygotujLosowania(dane.size(),k.ziarno);
    assert(losy==przygotujLosowania(dane.size(),k.ziarno));
    Symulator fifo(k,dane,losy,Strategia::FIFO), fee(k,dane,losy,Strategia::FEE);
    auto a=fifo.uruchom(), b=fee.uruchom();
    assert(a.size()==3 && b.size()==3);
    assert(a[0].idTransakcji==1 && a[1].idTransakcji==2 && a[2].idTransakcji==3);
    assert(b[0].idTransakcji==2 && b[1].idTransakcji==3 && b[2].idTransakcji==1);
    auto s=Statystyki::oblicz(a,k.walidatorzy,fifo.pobierzCzasKonca());
    assert(s.srednieOczekiwanie==2 && s.maksymalneOczekiwanie==4);
    assert(s.terminowe==3 && s.spoznione==0 && s.czasKonca==6 && s.wykorzystanie[0]==100);
    assert(Statystyki::oblicz({},k.walidatorzy,0).wykorzystanie[0]==0);
    Symulator granice(k,{{1,3,2,1,5},{2,3,2,1,6}},{0.1,0.2},Strategia::FIFO);
    auto g=granice.uruchom();
    assert(g[0].terminowo && g[0].opoznienie==0);
    assert(!g[1].terminowo && g[1].opoznienie==1);
    std::vector<Transakcja> duzo;
    for(int id=1;id<=205;++id)duzo.push_back({id,0,1,1,1000});
    Symulator prog(k,duzo,przygotujLosowania(205,11),Strategia::FIFO);
    std::vector<long long> progi;
    prog.uruchom(100,[&](const auto& p){progi.push_back(p.liczbaObsluzonych);});
    assert((progi==std::vector<long long>{100,200,205}));

    auto tmp=std::filesystem::temp_directory_path();
    auto folder=tmp/('p'+std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::filesystem::create_directory(folder);
    const auto input=folder/"wejscie.txt";
    {
        std::ofstream f(input);
        f << "[transakcja]\nid=1\nnadejscie=0\nczas_wykonania=2\nfee=1\ndeadline=10\n";
    }
    assert(Pliki::wczytajTransakcje(input).size()==1);
    {
        std::ofstream f(input);
        f << "[transakcja]\nid=1\nid=2\n";
    }
    bool odrzucone=false;
    try {Pliki::wczytajTransakcje(input);}catch(const std::exception&){odrzucone=true;}
    assert(odrzucone);
    auto rezultaty=wykonajPrzebiegi(k,dane,folder);
    assert(rezultaty.size()==2);
    std::ifstream f(folder/"wyniki_fifo.txt");
    std::string tekst((std::istreambuf_iterator<char>(f)),{});f.close();
    assert(tekst.find("[wynik]")!=std::string::npos);
    assert(tekst.find("[podsumowanie]")!=std::string::npos);
    assert(tekst.find("[wykorzystanie]")!=std::string::npos);
    assert(tekst.find("terminowo = tak")!=std::string::npos);
    assert(std::filesystem::equivalent(folder.parent_path(),tmp));
    for(const auto& entry:std::filesystem::directory_iterator(folder)) {
        assert(entry.path().parent_path()==folder && entry.is_regular_file());
        std::filesystem::remove(entry.path());
    }
    std::filesystem::remove(folder);
    std::cout << "OK: FIFO/FEE, statystyki, deadline, raporty, parser i zapis\n";
}
