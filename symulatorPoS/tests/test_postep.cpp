#include "Symulator.hpp"
#include <cassert>
#include <cmath>
#include <vector>
int main() {
    Konfiguracja k; k.walidatorzy = {{1,10},{2,30}};
    std::vector<Transakcja> t = {{1,0,2,1,2},{2,0,4,2,3},{3,2,1,3,3}};
    Symulator s(k,t,{0.1,0.1,0.1},Strategia::FIFO);
    std::vector<Podsumowanie> raporty;
    auto wyniki = s.uruchom(1,[&](const auto& p){raporty.push_back(p);});
    assert(wyniki.size()==3 && raporty.size()==3);
    assert(raporty[0].liczbaObsluzonych==1 && raporty[0].czasKonca==2);
    assert(raporty[0].wykorzystanie[0]==100 && raporty[0].wykorzystanie[1]==100);
    assert(raporty.back().terminowe==2 && raporty.back().spoznione==1);
    assert(raporty.back().wykorzystanie[0]==75 && raporty.back().wykorzystanie[1]==100);
    Symulator razem(k,{{1,0,2,1,10},{2,0,2,1,10}},{0.1,0.1},Strategia::FIFO);
    std::vector<Podsumowanie> r;
    razem.uruchom(1,[&](const auto& p){r.push_back(p);});
    assert(r.size()==2 && r[0].liczbaObsluzonych==1 && r[1].liczbaObsluzonych==2);
    assert(r[0].wykorzystanie[0]==100 && r[0].wykorzystanie[1]==100);
}
