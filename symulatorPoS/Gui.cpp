#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#define NOMINMAX
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include "Pliki.hpp"
#include "Przebiegi.hpp"
#include "WidokWynikow.hpp"
#include <atomic>
#include <exception>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <thread>

namespace {
HWND okno, konfiguracjaEdit, transakcjeEdit, wybierzK, wybierzT, startButton, tabs, tabela, statusBar, metadane, podsumowanieLabel;
HWND kolejnoscLabel, resetKolejnosci;
int kolumnaSortowania = -1;
bool rosnaco = true;
std::vector<std::size_t> indeksyWidoku;
const wchar_t* nazwyKolumn[] = {L"Transakcja",L"Walidator",L"Nadejście",L"Start",L"Koniec",L"Oczekiwanie",L"Deadline",L"Opóźnienie",L"Terminowo"};
HFONT font, tytulFont;
bool gotowe = false, zajety = false, inicjalizacja = true, zamknijPoPracy = false;
std::thread obliczenia;
std::atomic<bool> skonczone{false};
std::mutex mutex;
std::wstring postep, bladPracy;
std::vector<RezultatPrzebiegu> rezultaty, wynikPracy;

std::wstring szeroko(const std::string& tekst) {
    int n = MultiByteToWideChar(CP_UTF8, 0, tekst.data(), static_cast<int>(tekst.size()), nullptr, 0);
    std::wstring wynik(n, L' ');
    MultiByteToWideChar(CP_UTF8, 0, tekst.data(), static_cast<int>(tekst.size()), wynik.data(), n);
    return wynik;
}
std::wstring tekst(HWND h) {
    int n = GetWindowTextLengthW(h);
    std::wstring s(n+1, L'\0');
    GetWindowTextW(h, s.data(), n+1);
    s.resize(n);
    return s;
}
HWND kontrolka(const wchar_t* klasa, const wchar_t* napis, DWORD styl, int id) {
    HWND h = CreateWindowExW(0, klasa, napis, WS_CHILD|WS_VISIBLE|styl, 0,0,0,0,okno,
                            reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),GetModuleHandleW(nullptr),nullptr);
    if (!h) throw std::runtime_error("Nie mozna utworzyc kontrolki GUI");
    SendMessageW(h, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
    return h;
}
const RezultatPrzebiegu* wybrany() {
    int i = TabCtrl_GetCurSel(tabs);
    return i >= 0 && static_cast<std::size_t>(i) < rezultaty.size() ? &rezultaty[i] : nullptr;
}
void odswiezTabele() {
    const auto* r = wybrany();
    indeksyWidoku = r ? uporzadkujWidok(r->wyniki,kolumnaSortowania,rosnaco) : std::vector<std::size_t>{};
    std::wstring opis = L"Widok: kolejność zakończenia. Kliknij nagłówek, aby posortować.";
    if (kolumnaSortowania >= 0)
        opis = std::wstring(L"Widok: ") + nazwyKolumn[kolumnaSortowania] + (rosnaco ? L" · rosnąco." : L" · malejąco.")
             + L" Kliknij nagłówek, aby zmienić.";
    SetWindowTextW(kolejnoscLabel,opis.c_str());
    EnableWindow(resetKolejnosci,r && kolumnaSortowania>=0 && !zajety);
    auto header = ListView_GetHeader(tabela);
    for (int i=0;i<9;++i) {
        HDITEMW item{}; item.mask=HDI_FORMAT; Header_GetItem(header,i,&item);
        item.fmt &= ~(HDF_SORTUP|HDF_SORTDOWN);
        if (i==kolumnaSortowania) item.fmt |= rosnaco ? HDF_SORTUP : HDF_SORTDOWN;
        Header_SetItem(header,i,&item);
    }
    ListView_SetItemCountEx(tabela, r ? static_cast<int>(r->wyniki.size()) : 0, 0);
    InvalidateRect(tabela, nullptr, TRUE);
    if (r) {
        const auto& s = r->podsumowanie;
        std::wostringstream out;
        out << std::fixed << std::setprecision(2) << L"Obsłużone: " << s.liczbaObsluzonych
            << L"   Średnie oczekiwanie: " << s.srednieOczekiwanie << L"   Maksymalne: " << s.maksymalneOczekiwanie
            << L"   Terminowe: " << s.terminowe << L"   Spóźnione: " << s.spoznione;
        SetWindowTextW(podsumowanieLabel, out.str().c_str());
    } else SetWindowTextW(podsumowanieLabel, L"");
}
void waliduj() {
    if (inicjalizacja || zajety) return;
    gotowe = false;
    kolumnaSortowania = -1; rosnaco = true;
    rezultaty.clear(); odswiezTabele();
    try {
        auto k = Pliki::wczytajKonfiguracje(std::filesystem::path(tekst(konfiguracjaEdit)));
        auto t = Pliki::wczytajTransakcje(std::filesystem::path(tekst(transakcjeEdit)));
        std::wstring opis = std::to_wstring(k.walidatorzy.size()) + L" walidatorów · "
            + std::to_wstring(t.size()) + L" transakcji · Ziarno: " + std::to_wstring(k.ziarno);
        SetWindowTextW(metadane, opis.c_str());
        SetWindowTextW(statusBar, L"Dane poprawne. Uruchom symulację. Zapis: folder wyniki obok pliku zgłoszeń.");
        gotowe = true;
    } catch (const std::exception& e) {
        SetWindowTextW(metadane, L"Wybierz dwa poprawne pliki wejściowe");
        SetWindowTextW(statusBar, szeroko(e.what()).c_str());
    }
    EnableWindow(startButton, gotowe);
}
void wybierzPlik(HWND pole) {
    wchar_t sciezka[32768] = {};
    OPENFILENAMEW ofn{}; ofn.lStructSize = sizeof(ofn); ofn.hwndOwner = okno;
    ofn.lpstrFilter = L"Pliki tekstowe (*.txt)\0*.txt\0Wszystkie pliki\0*.*\0";
    ofn.lpstrFile = sciezka; ofn.nMaxFile = 32768;
    ofn.Flags = OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_NOCHANGEDIR;
    if (GetOpenFileNameW(&ofn)) SetWindowTextW(pole, sciezka);
}
void ustawZajety(bool stan) {
    zajety = stan;
    for (HWND h : {konfiguracjaEdit, transakcjeEdit, wybierzK, wybierzT}) EnableWindow(h, !stan);
    EnableWindow(startButton, !stan && gotowe);
    SetWindowTextW(startButton, stan ? L"Obliczanie…" : L"Uruchom symulację");
}
void uruchom() {
    if (zajety) return;
    waliduj();
    if (!gotowe) return;
    auto kPath = std::filesystem::path(tekst(konfiguracjaEdit));
    auto tPath = std::filesystem::path(tekst(transakcjeEdit));
    auto folder = tPath.parent_path() / L"wyniki";
    ustawZajety(true);
    SetWindowTextW(statusBar, L"Obliczanie FIFO i FEE…");
    skonczone = false;
    {
        std::lock_guard<std::mutex> lock(mutex);
        postep.clear(); bladPracy.clear(); wynikPracy.clear();
    }
    try {
        obliczenia = std::thread([kPath,tPath,folder] {
            try {
                auto k = Pliki::wczytajKonfiguracje(kPath);
                auto t = Pliki::wczytajTransakcje(tPath);
                auto r = wykonajPrzebiegi(k,t,folder,100,[](Strategia strategia,const Podsumowanie& s) {
                    std::lock_guard<std::mutex> lock(mutex);
                    postep = szeroko(nazwaStrategii(strategia)) + L" · zakończone: " + std::to_wstring(s.liczbaObsluzonych);
                });
                std::lock_guard<std::mutex> lock(mutex);
                wynikPracy = std::move(r);
            } catch (const std::exception& e) {
                std::lock_guard<std::mutex> lock(mutex);
                bladPracy = szeroko(e.what());
            }
            skonczone = true;
        });
        SetTimer(okno,1,75,nullptr);
    } catch (const std::exception& e) {
        ustawZajety(false);
        SetWindowTextW(statusBar,szeroko(e.what()).c_str());
    }
}
void zakonczLubPostep() {
    if (!zajety) return;
    if (!skonczone) {
        std::lock_guard<std::mutex> lock(mutex);
        if (!postep.empty()) SetWindowTextW(statusBar,postep.c_str());
        return;
    }
    obliczenia.join(); KillTimer(okno,1);
    std::wstring blad;
    {
        std::lock_guard<std::mutex> lock(mutex);
        rezultaty = std::move(wynikPracy); blad = bladPracy;
    }
    ustawZajety(false); odswiezTabele();
    if (!blad.empty()) {
        SetWindowTextW(statusBar,blad.c_str());
    } else {
        std::wstring opis = rezultaty.front().wyniki.empty() ? L"Brak transakcji — przebieg zakończony. " : L"Zakończono. ";
        auto folder = std::filesystem::path(tekst(transakcjeEdit)).parent_path()/L"wyniki";
        opis += L"Zapisano oba pliki w: " + folder.wstring();
        SetWindowTextW(statusBar,opis.c_str());
    }
    if (zamknijPoPracy) DestroyWindow(okno);
}
void uklad(int width,int height) {
    int pole = width-280;
    MoveWindow(GetDlgItem(okno,110),24,18,width-48,46,TRUE);
    MoveWindow(GetDlgItem(okno,111),24,86,135,30,TRUE);
    MoveWindow(GetDlgItem(okno,112),24,132,135,30,TRUE);
    MoveWindow(konfiguracjaEdit,160,82,pole,32,TRUE); MoveWindow(wybierzK,width-104,82,80,32,TRUE);
    MoveWindow(transakcjeEdit,160,128,pole,32,TRUE); MoveWindow(wybierzT,width-104,128,80,32,TRUE);
    MoveWindow(metadane,24,180,width-290,28,TRUE); MoveWindow(startButton,width-252,172,228,40,TRUE);
    MoveWindow(tabs,24,235,width-48,34,TRUE); MoveWindow(podsumowanieLabel,24,281,width-48,28,TRUE);
    MoveWindow(kolejnoscLabel,24,320,width-370,28,TRUE);
    MoveWindow(resetKolejnosci,width-330,313,306,32,TRUE);
    MoveWindow(tabela,24,355,width-48,height-415,TRUE); MoveWindow(statusBar,24,height-42,width-48,30,TRUE);
}
LRESULT CALLBACK procedura(HWND h,UINT msg,WPARAM wp,LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        okno=h;
        font=CreateFontW(-17,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,L"Segoe UI");
        tytulFont=CreateFontW(-30,0,0,0,FW_SEMIBOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,L"Segoe UI");
        auto title=kontrolka(L"STATIC",L"Symulator blockchainu PoS",0,110);
        SendMessageW(title,WM_SETFONT,reinterpret_cast<WPARAM>(tytulFont),TRUE);
        kontrolka(L"STATIC",L"Konfiguracja",0,111); kontrolka(L"STATIC",L"Zgłoszenia",0,112);
        konfiguracjaEdit=kontrolka(L"EDIT",L"",WS_BORDER|WS_TABSTOP|ES_AUTOHSCROLL,101);
        transakcjeEdit=kontrolka(L"EDIT",L"",WS_BORDER|WS_TABSTOP|ES_AUTOHSCROLL,102);
        wybierzK=kontrolka(L"BUTTON",L"Wybierz…",WS_TABSTOP,103); wybierzT=kontrolka(L"BUTTON",L"Wybierz…",WS_TABSTOP,104);
        startButton=kontrolka(L"BUTTON",L"Uruchom symulację",WS_TABSTOP|BS_DEFPUSHBUTTON,105);
        metadane=kontrolka(L"STATIC",L"",0,109);
        tabs=kontrolka(WC_TABCONTROLW,L"",WS_TABSTOP,106);
        for (const wchar_t* s : {L"FIFO",L"FEE"}) {
            TCITEMW item{}; item.mask=TCIF_TEXT; item.pszText=const_cast<wchar_t*>(s);
            TabCtrl_InsertItem(tabs,TabCtrl_GetItemCount(tabs),&item);
        }
        podsumowanieLabel=kontrolka(L"STATIC",L"",0,113);
        kolejnoscLabel=kontrolka(L"STATIC",L"",0,114);
        resetKolejnosci=kontrolka(L"BUTTON",L"Przywróć kolejność zakończenia",WS_TABSTOP,115);
        tabela=kontrolka(WC_LISTVIEWW,L"",WS_TABSTOP|WS_BORDER|LVS_REPORT|LVS_OWNERDATA|LVS_SHOWSELALWAYS,107);
        ListView_SetExtendedListViewStyle(tabela,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_DOUBLEBUFFER);
        int col=0;
        for (const wchar_t* s : nazwyKolumn) {
            LVCOLUMNW c{}; c.mask=LVCF_TEXT|LVCF_WIDTH; c.cx=120; c.pszText=const_cast<wchar_t*>(s);
            ListView_InsertColumn(tabela,col++,&c);
        }
        statusBar=kontrolka(L"STATIC",L"Wybierz pliki i uruchom symulację",SS_PATHELLIPSIS,108);
        auto cwd=std::filesystem::current_path();
        SetWindowTextW(konfiguracjaEdit,(cwd/L"konfiguracja.txt").c_str());
        SetWindowTextW(transakcjeEdit,(cwd/L"zgloszenia.txt").c_str());
        inicjalizacja=false; waliduj();
        return 0;
    }
    case WM_SIZE: if (tabela) uklad(LOWORD(lp),HIWORD(lp)); return 0;
    case WM_GETMINMAXINFO: {
        auto p=reinterpret_cast<MINMAXINFO*>(lp); p->ptMinTrackSize={940,600}; return 0;
    }
    case WM_COMMAND:
        if ((LOWORD(wp)==101 || LOWORD(wp)==102) && HIWORD(wp)==EN_CHANGE) waliduj();
        if (LOWORD(wp)==103) wybierzPlik(konfiguracjaEdit);
        if (LOWORD(wp)==104) wybierzPlik(transakcjeEdit);
        if (LOWORD(wp)==105) uruchom();
        if (LOWORD(wp)==115) { kolumnaSortowania=-1; rosnaco=true; odswiezTabele(); }
        return 0;
    case WM_NOTIFY: {
        auto n=reinterpret_cast<NMHDR*>(lp);
        if (n->hwndFrom==tabs && n->code==TCN_SELCHANGE) odswiezTabele();
        if (n->hwndFrom==tabela && n->code==LVN_COLUMNCLICK) {
            int kolumna=reinterpret_cast<NMLISTVIEW*>(lp)->iSubItem;
            if (kolumna>=0 && kolumna<9) {
                rosnaco = kolumna==kolumnaSortowania ? !rosnaco : true;
                kolumnaSortowania=kolumna;
                ListView_SetItemState(tabela,-1,0,LVIS_SELECTED|LVIS_FOCUSED);
                odswiezTabele();
            }
        }
        if (n->hwndFrom==tabela && n->code==LVN_GETDISPINFOW) {
            auto info=reinterpret_cast<NMLVDISPINFOW*>(lp);
            auto r=wybrany(); auto& item=info->item;
            if ((item.mask&LVIF_TEXT) && r && item.iItem>=0 && static_cast<std::size_t>(item.iItem)<r->wyniki.size()) {
                const auto& w=r->wyniki[indeksyWidoku[item.iItem]];
                long long values[]={w.idTransakcji,w.idWalidatora,w.nadejscie,w.start,w.koniec,w.oczekiwanie,w.deadline,w.opoznienie};
                std::wstring s=item.iSubItem<8 ? std::to_wstring(values[item.iSubItem]) : (w.terminowo ? L"Tak" : L"Nie");
                lstrcpynW(item.pszText,s.c_str(),item.cchTextMax);
            }
        }
        return 0;
    }
    case WM_TIMER: zakonczLubPostep(); return 0;
    case WM_CTLCOLORSTATIC:
        SetBkColor(reinterpret_cast<HDC>(wp),RGB(255,255,255));
        SetTextColor(reinterpret_cast<HDC>(wp),RGB(28,43,54));
        return reinterpret_cast<LRESULT>(GetStockObject(WHITE_BRUSH));
    case WM_CLOSE:
        if (zajety) { zamknijPoPracy=true; SetWindowTextW(statusBar,L"Okno zamknie się po zakończeniu obliczeń i zapisie wyników."); }
        else DestroyWindow(h);
        return 0;
    case WM_DESTROY:
        DeleteObject(font); DeleteObject(tytulFont); PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(h,msg,wp,lp);
}
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE,PWSTR,int show) {
    try {
        SetProcessDPIAware();
        INITCOMMONCONTROLSEX init{sizeof(init),ICC_LISTVIEW_CLASSES|ICC_TAB_CLASSES};
        InitCommonControlsEx(&init);
        WNDCLASSW cls{}; cls.lpfnWndProc=procedura; cls.hInstance=instance;
        cls.lpszClassName=L"SymulatorPoSWindow"; cls.hCursor=LoadCursorW(nullptr,IDC_ARROW);
        cls.hbrBackground=static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
        RegisterClassW(&cls);
        HWND h=CreateWindowExW(0,cls.lpszClassName,L"Symulator blockchainu PoS",WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT,CW_USEDEFAULT,1260,820,nullptr,nullptr,instance,nullptr);
        if (!h) return 1;
        ShowWindow(h,show); UpdateWindow(h);
        MSG msg{};
        while (GetMessageW(&msg,nullptr,0,0)>0) {
            if (!IsDialogMessageW(h,&msg)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
        }
        return static_cast<int>(msg.wParam);
    } catch (const std::exception& e) {
        MessageBoxW(nullptr,szeroko(e.what()).c_str(),L"Błąd symulatora",MB_ICONERROR);
        return 1;
    }
}
