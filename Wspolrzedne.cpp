#include "Wspolrzedne.h"

Wspolrzedne::Wspolrzedne() {
    info = Rect(ODLEGLOSC_OD_LEWEJ_KRAWEDZI, ODLEGLOSC_OD_GORNEJ_KRAWEDZI, ROZMIAR_PRZYCISKOW, ROZMIAR_PRZYCISKOW);
    ustawienia = Rect(ODLEGLOSC_OD_LEWEJ_KRAWEDZI, ODLEGLOSC_OD_GORNEJ_KRAWEDZI + ROZMIAR_PRZYCISKOW, ROZMIAR_PRZYCISKOW, ROZMIAR_PRZYCISKOW);
    informacje = Rect(ODLEGLOSC_OD_LEWEJ_KRAWEDZI + ROZMIAR_PRZYCISKOW, info.y, ROZMIAR_OKNA, ROZMIAR_OKNA);
    ekranUstawien = Rect(ODLEGLOSC_OD_LEWEJ_KRAWEDZI + ROZMIAR_PRZYCISKOW, ustawienia.y, ROZMIAR_OKNA, ROZMIAR_OKNA);
    for (int i=0; i<4; i++) {
        odstepPomiedzyZdjeciami[i] = Rect((ROZMIAR_PRZYCISKOW * i) + ekranUstawien.x + (ROZMIAR_OKNA/2), ekranUstawien.y + 80, ROZMIAR_PRZYCISKOW, ROZMIAR_PRZYCISKOW);
        czasWykonaniaUsmiechu[i] = Rect(odstepPomiedzyZdjeciami[i].x, odstepPomiedzyZdjeciami[i].y + 100, ROZMIAR_PRZYCISKOW, ROZMIAR_PRZYCISKOW);
        if (i<2) {
            filtr[i] = Rect(odstepPomiedzyZdjeciami[i].x, czasWykonaniaUsmiechu[i].y + 80, ROZMIAR_PRZYCISKOW, ROZMIAR_PRZYCISKOW);
        }
    }
}

Wspolrzedne::~Wspolrzedne() {
    //dtor
}

Rect Wspolrzedne::getWspolrzedne(int ktoryElement) {
    switch (ktoryElement) {
    case INFO:
        return info;
        break;
    case USTAWIENIA:
        return ustawienia;
        break;
    case INFORMACJE:
        return informacje;
        break;
    case EKRAN_USTAWIEN:
        return ekranUstawien;
        break;
    }

    if (ktoryElement >= ODSTEP_POMIEDZY_ZDJECIAMI && ktoryElement < CZAS_WYKONANIA_USMIECHU) {
        int i = ktoryElement - ODSTEP_POMIEDZY_ZDJECIAMI;
        return odstepPomiedzyZdjeciami[i];
    }
    if (ktoryElement >= CZAS_WYKONANIA_USMIECHU && ktoryElement < FILTR) {
        int i = ktoryElement - CZAS_WYKONANIA_USMIECHU;
        return czasWykonaniaUsmiechu[i];
    }
    if (ktoryElement >= FILTR) {
        int i = ktoryElement - FILTR;
        return filtr[i];
    }
}

int Wspolrzedne::czyNajechanoNaPrzycisk(int x, int y) {
    if (info.contains(Point(x,y)))
        return INFO;
    if (ustawienia.contains(Point(x,y)))
        return USTAWIENIA;

    for (int i=0; i<4; i++) {
        if (odstepPomiedzyZdjeciami[i].contains(Point(x,y)))
            return ODSTEP_POMIEDZY_ZDJECIAMI + i;
        if (czasWykonaniaUsmiechu[i].contains(Point(x,y)))
            return CZAS_WYKONANIA_USMIECHU + i;
        if (i < 2) {
            if (filtr[i].contains(Point(x,y)))
                return FILTR + i;
        }
    }
}
