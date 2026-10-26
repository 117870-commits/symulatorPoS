#include "Symulator.hpp"
#include <cassert>
#include <vector>
int main() {
    Konfiguracja k; k.walidatorzy = {{1, 10}, {2, 30}};
    std::vector<Transakcja> t = {{1,0,3,1,10},{2,0,1,1,10},{3,1,1,1,10}};
    Symulator s(k,t,{0.1,0.1,0.9},Strategia::FIFO);
    auto wynik = s.uruchom();
    assert(wynik.size()==3);
    for (const auto& w : wynik) {
        if (w.idTransakcji==1) assert(w.idWalidatora==1 && w.start==0 && w.koniec==3);
        if (w.idTransakcji==2) assert(w.idWalidatora==2 && w.start==0 && w.koniec==1);
        if (w.idTransakcji==3) assert(w.idWalidatora==2 && w.start==1 && w.koniec==2);
    }
    Symulator s2(k,{{1,0,1,1,10}},{0.25},Strategia::FIFO);
    assert(s2.uruchom().front().idWalidatora==2);
    Symulator s3(k,{{1,0,1,1,10}},{0.249},Strategia::FIFO);
    assert(s3.uruchom().front().idWalidatora==1);
}
