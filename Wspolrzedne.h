#ifndef WSPOLRZEDNE_H
#define WSPOLRZEDNE_H

#include <opencv2/opencv.hpp>
#include <iostream>

#define ODLEGLOSC_OD_LEWEJ_KRAWEDZI 10
#define ODLEGLOSC_OD_GORNEJ_KRAWEDZI 10
#define ROZMIAR_PRZYCISKOW 48
#define ROZMIAR_OKNA 400

#define INFO 100
#define USTAWIENIA 101
#define INFORMACJE 102
#define EKRAN_USTAWIEN 103
#define ODSTEP_POMIEDZY_ZDJECIAMI 128
#define CZAS_WYKONANIA_USMIECHU 256
#define FILTR 512

using namespace cv;

class Wspolrzedne
{
    public:
        Wspolrzedne();
        virtual ~Wspolrzedne();

        Rect getWspolrzedne(int ktoryElement);
        int czyNajechanoNaPrzycisk(int x, int y);
    protected:
    private:
        Rect info;
        Rect ustawienia;
        Rect informacje;
        Rect ekranUstawien;
        Rect odstepPomiedzyZdjeciami[4];
        Rect czasWykonaniaUsmiechu[4];
        Rect filtr[2];
};

#endif // WSPOLRZEDNE_H
