#include "WidokWynikow.hpp"
#include <cassert>

int main() {
    WynikTransakcji a; a.idTransakcji=10; a.idWalidatora=2; a.nadejscie=1; a.start=3; a.koniec=8;
    a.oczekiwanie=2; a.deadline=9; a.opoznienie=0; a.terminowo=true;
    WynikTransakcji b; b.idTransakcji=2; b.idWalidatora=1; b.nadejscie=0; b.start=0; b.koniec=8;
    b.oczekiwanie=0; b.deadline=7; b.opoznienie=1; b.terminowo=false;
    WynikTransakcji c=a; c.idTransakcji=1;
    std::vector<WynikTransakcji> wyniki{a,b,c};
    assert((uporzadkujWidok(wyniki,-1,true)==std::vector<std::size_t>{0,1,2}));
    assert((uporzadkujWidok(wyniki,0,true)==std::vector<std::size_t>{2,1,0}));
    assert((uporzadkujWidok(wyniki,0,false)==std::vector<std::size_t>{0,1,2}));
    assert((uporzadkujWidok(wyniki,4,true)==std::vector<std::size_t>{0,1,2}));
    for (int col : {1,2,3,5,6,8}) {
        assert((uporzadkujWidok(wyniki,col,true)==std::vector<std::size_t>{1,0,2}));
        assert((uporzadkujWidok(wyniki,col,false)==std::vector<std::size_t>{0,2,1}));
    }
    assert((uporzadkujWidok(wyniki,7,true)==std::vector<std::size_t>{0,2,1}));
    assert((uporzadkujWidok(wyniki,7,false)==std::vector<std::size_t>{1,0,2}));
    assert(wyniki[0].idTransakcji==10 && wyniki[1].idTransakcji==2);
    assert(uporzadkujWidok({},0,true).empty());
}
