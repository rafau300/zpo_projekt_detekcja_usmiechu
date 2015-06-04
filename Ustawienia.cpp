#include "Ustawienia.h"

Ustawienia::Ustawienia()
{
    odstepPomiedzyZdjeciami = ODSTEP_POMIEDZY_ZDJECIAMI;
    tolerancja = TOLERANCJA;
    liczbaKlatekUsmiechu = LICZBA_KLATEK_USMIECHU;
    ktoryFiltr = KTORY_FILTR;
    katalogZeZdjeciami = "zdjecia/";
}

Ustawienia::~Ustawienia()
{
    //dtor
}
