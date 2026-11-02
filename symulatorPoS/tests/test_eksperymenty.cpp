#include "Pliki.hpp"
#include "Przebiegi.hpp"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>

int main() {
    std::ifstream csv("porownanie.csv");
    assert(csv && "Uruchom test z folderu projektu");
    std::string line;std::getline(csv,line);
    auto tmp=std::filesystem::temp_directory_path();
    auto folder=tmp/("pos_badania_"+std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    assert(std::filesystem::create_directory(folder));
    std::string ostatni;
    std::vector<RezultatPrzebiegu> wyniki;
    std::vector<Transakcja> transakcje;
    auto k=Pliki::wczytajKonfiguracje("konfiguracja.txt");
    int przebiegi=0;
    while(std::getline(csv,line)) {
        std::stringstream stream(line);std::string value;std::vector<std::string> pola;
        while(std::getline(stream,value,','))pola.push_back(value);
        assert(pola.size()==14);
        auto klucz=pola[0]+"/"+pola[2];
        if(klucz!=ostatni) {
            transakcje=Pliki::wczytajTransakcje(pola[0]+".txt");
            k.ziarno=static_cast<unsigned int>(std::stoul(pola[2]));
            wyniki=wykonajPrzebiegi(k,transakcje,folder);
            ostatni=klucz;
        }
        const auto& r=wyniki[pola[1]=="FIFO"?0:1];
        const auto& s=r.podsumowanie;
        assert(r.wyniki.size()==6000 && k.walidatorzy.size()==300);
        assert(std::abs(s.srednieOczekiwanie-std::stod(pola[5]))<0.00001);
        assert(s.maksymalneOczekiwanie==std::stoll(pola[6]));
        assert(s.terminowe==std::stoll(pola[7]) && s.spoznione==std::stoll(pola[8]));
        assert(s.czasKonca==std::stoll(pola[10]));
        double srednie=std::accumulate(s.wykorzystanie.begin(),s.wykorzystanie.end(),0.0)/300;
        assert(std::abs(srednie-std::stod(pola[11]))<0.00001);
        std::map<int,std::vector<std::pair<long long,long long>>> prace;
        std::map<int,Transakcja> wejscie;
        for(const auto& t:transakcje)wejscie.emplace(t.id,t);
        long double suma=0;long long terminowe=0,maksimum=0;
        for(const auto& w:r.wyniki) {
            auto it=wejscie.find(w.idTransakcji);assert(it!=wejscie.end());
            auto t=it->second;wejscie.erase(it);
            assert(w.start>=t.nadejscie && w.koniec-w.start==t.czasWykonania);
            assert(w.oczekiwanie==w.start-t.nadejscie);
            assert(w.opoznienie==std::max(0LL,w.koniec-t.deadline));
            assert(w.terminowo==(w.koniec<=t.deadline));
            suma+=w.oczekiwanie;terminowe+=w.terminowo;maksimum=std::max(maksimum,w.oczekiwanie);
            prace[w.idWalidatora].push_back({w.start,w.koniec});
        }
        assert(wejscie.empty() && terminowe==s.terminowe && maksimum==s.maksymalneOczekiwanie);
        assert(std::abs(static_cast<double>(suma/6000)-s.srednieOczekiwanie)<0.00001);
        for(std::size_t i=0;i<k.walidatorzy.size();++i) {
            auto& zadania=prace[k.walidatorzy[i].id];std::sort(zadania.begin(),zadania.end());
            long long praca=0,koniec=0;
            for(auto [start,stop]:zadania){assert(start>=koniec);praca+=stop-start;koniec=stop;}
            assert(std::abs(100.0*praca/s.czasKonca-s.wykorzystanie[i])<0.00001);
        }
        ++przebiegi;
    }
    assert(przebiegi==18 && std::filesystem::equivalent(folder.parent_path(),tmp));
    for(const auto& entry:std::filesystem::directory_iterator(folder)) {
        assert(entry.is_regular_file() && entry.path().parent_path()==folder);
        std::filesystem::remove(entry.path());
    }
    std::filesystem::remove(folder);
    std::cout << "OK: 18 przebiegow i 108000 wynikow - statystyki, czasy, deadline i brak nakladania pracy\n";
}
