#ifndef USTAWIENIA_H
#define USTAWIENIA_H

#include <string>

//przez ile klatek nie mozna zrobic "zdjecia", w przeciwnym razie program wykonywalby mnostwo zdjec
#define ODSTEP_POMIEDZY_ZDJECIAMI 100
//z jaka tolerancja wykonywac "wklejanie" obrazka z zielonym tlem
#define TOLERANCJA 15
//ile klatek odczekac, ýeby zrobic "zdjecie", zeby uchwycic pelny usmiech
#define LICZBA_KLATEK_USMIECHU 5
//ile jest zdefiniowanych filtrow
#define LICZBA_FILTROW 2
//domyslny filtr - 0 brak filtra
#define KTORY_FILTR 0

using namespace std;

class Ustawienia
{
    public:
        Ustawienia();
        virtual ~Ustawienia();
        unsigned int getOdstepPomiedzyZdjeciami() { return odstepPomiedzyZdjeciami; }
        void setOdstepPomiedzyZdjeciami(unsigned int val) { odstepPomiedzyZdjeciami = val; }
        unsigned int getTolerancja() { return tolerancja; }
        void setTolerancja(unsigned int val) { tolerancja = val; }
        unsigned int getliczbaKlatekUsmiechu() { return liczbaKlatekUsmiechu; }
        void setliczbaKlatekUsmiechu(unsigned int val) { liczbaKlatekUsmiechu = val; }
        unsigned int getktoryFiltr() { return ktoryFiltr; }
        void setktoryFiltr(unsigned int val) { ktoryFiltr = val; }
        string getKatalogZeZdjeciami() { return katalogZeZdjeciami; }
        void setKatalogZeZdjeciami(string val) { katalogZeZdjeciami = val; }
    protected:
    private:
        unsigned int odstepPomiedzyZdjeciami;
        unsigned int tolerancja;
        unsigned int liczbaKlatekUsmiechu;
        unsigned int ktoryFiltr;
        string katalogZeZdjeciami;
};

#endif // USTAWIENIA_H
