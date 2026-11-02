#include "Pliki.hpp"
#include "Przebiegi.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <stdexcept>

void sprawdzZapis(std::ofstream& plik) {
    plik.flush();
    if (!plik) throw std::runtime_error("Blad zapisu danych eksperymentu");
}
int main() {
    try {
        constexpr int N=6000, K=300;
        std::ofstream konfiguracja("konfiguracja.txt");
        konfiguracja << "# Zestaw prezentacyjny: 300 walidatorow, stale stake 10..1000.\n[symulacja]\nziarno = 11\n";
        for (int id=1;id<=K;++id)
            konfiguracja << "\n[walidator]\nid = " << id << "\nstake = " << 10+(id*37)%991 << '\n';
        sprawdzZapis(konfiguracja); konfiguracja.close();
        std::mt19937 generator(20261102);
        std::vector<Transakcja> baza;
        for (int id=1;id<=N;++id) {
            long long czas=8+generator()%25, fee=1+generator()%100;
            baza.push_back({id,0,czas,fee,czas+10+(100-fee)/2});
        }
        for (const std::string scenariusz : {"male_obciazenie","przeciazenie","nagly_naplyw"}) {
            std::ofstream plik(scenariusz+".txt");
            plik << "# 6000 transakcji. Generator mt19937, seed danych 20261102.\n"
                 << "# " << scenariusz << ": czasy 8..32, fee 1..100, zapas deadline 10+(100-fee)/2.\n";
            for (auto t : baza) {
                if (scenariusz=="male_obciazenie") t.nadejscie=(t.id-1)/6;
                else if (scenariusz=="przeciazenie") t.nadejscie=(t.id-1)/100;
                t.deadline += t.nadejscie;
                plik << "\n[transakcja]\nid = " << t.id << "\nnadejscie = " << t.nadejscie
                     << "\nczas_wykonania = " << t.czasWykonania << "\nfee = " << t.fee << "\ndeadline = " << t.deadline << '\n';
            }
            sprawdzZapis(plik);
        }
        std::filesystem::copy_file("przeciazenie.txt","zgloszenia.txt",std::filesystem::copy_options::overwrite_existing);
        std::ofstream csv("porownanie.csv"), wykorzystanie("wykorzystanie.csv"), grupy("grupy_fee.csv");
        csv << "scenariusz,strategia,ziarno,N,K,srednie_oczekiwanie,maksymalne_oczekiwanie,terminowe,spoznione,odsetek_terminowych,czas_konca,wykorzystanie_srednie,wykorzystanie_min,wykorzystanie_max\n";
        wykorzystanie << "scenariusz,strategia,ziarno,id_walidatora,stake,wykorzystanie_procent\n";
        grupy << "scenariusz,strategia,ziarno,grupa,liczba,srednie_oczekiwanie,odsetek_terminowych\n";
        csv << std::fixed << std::setprecision(6);
        wykorzystanie << std::fixed << std::setprecision(6);
        grupy << std::fixed << std::setprecision(6);
        auto k=Pliki::wczytajKonfiguracje("konfiguracja.txt");
        for (const std::string scenariusz : {"male_obciazenie","przeciazenie","nagly_naplyw"}) {
            auto t=Pliki::wczytajTransakcje(scenariusz+".txt");
            std::map<int,long long> oplaty;
            for (const auto& tx:t) oplaty.emplace(tx.id,tx.fee);
            for (unsigned int seed : {11u,22u,33u}) {
                k.ziarno=seed;
                auto folder=std::filesystem::path("eksperymenty")/scenariusz/std::to_string(seed);
                auto wyniki=wykonajPrzebiegi(k,t,folder);
                for (const auto& r:wyniki) {
                    const auto& s=r.podsumowanie; const auto& u=s.wykorzystanie;
                    double srednie=std::accumulate(u.begin(),u.end(),0.0)/u.size();
                    csv << scenariusz << ',' << nazwaStrategii(r.strategia) << ',' << seed << ',' << t.size() << ',' << k.walidatorzy.size()
                        << ',' << s.srednieOczekiwanie << ',' << s.maksymalneOczekiwanie << ',' << s.terminowe << ',' << s.spoznione
                        << ',' << 100.0*s.terminowe/t.size() << ',' << s.czasKonca << ',' << srednie
                        << ',' << *std::min_element(u.begin(),u.end()) << ',' << *std::max_element(u.begin(),u.end()) << '\n';
                    for (std::size_t i=0;i<u.size();++i)
                        wykorzystanie << scenariusz << ',' << nazwaStrategii(r.strategia) << ',' << seed << ','
                                      << k.walidatorzy[i].id << ',' << k.walidatorzy[i].stake << ',' << u[i] << '\n';
                    for (const bool wysokie : {false,true}) {
                        long long licznik=0, terminowe=0; double oczekiwanie=0;
                        for (const auto& w:r.wyniki) {
                            auto fee=oplaty.at(w.idTransakcji);
                            if ((wysokie && fee>=75) || (!wysokie && fee<=25)) {
                                ++licznik; oczekiwanie+=w.oczekiwanie; terminowe+=w.terminowo;
                            }
                        }
                        grupy << scenariusz << ',' << nazwaStrategii(r.strategia) << ',' << seed << ','
                              << (wysokie ? "fee_75_100" : "fee_1_25") << ',' << licznik << ','
                              << (licznik ? oczekiwanie/licznik : 0) << ',' << (licznik ? 100.0*terminowe/licznik : 0) << '\n';
                    }
                    std::cout << scenariusz << ' ' << nazwaStrategii(r.strategia) << " seed=" << seed
                              << " srednie=" << s.srednieOczekiwanie << " terminowe=" << s.terminowe << '/' << t.size() << '\n';
                }
            }
        }
        sprawdzZapis(csv); sprawdzZapis(wykorzystanie); sprawdzZapis(grupy);
        auto domyslne=Pliki::wczytajKonfiguracje("konfiguracja.txt");
        wykonajPrzebiegi(domyslne,Pliki::wczytajTransakcje("zgloszenia.txt"),".");
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Blad eksperymentu: " << e.what() << '\n'; return 1;
    }
}
