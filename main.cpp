/**
Zaawansowane przetwarzanie obrazow - projekt
Temat: Detekcja twarzy i usmiechu
Wykonawcy: Rafal Bebenek i Jacek Kominek
Grupa: 1ID22B, studia II stopnia
Rok akademicki: 2014/2015
**/

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Ustawienia.h"
#include "Wspolrzedne.h"

//Wspolczynnik alfa bitmapy z informacjami lub z ustawieniami (1 - brak przezroczystosci, 0 - bitmapa niewidzialna)
#define ALFA 0.5

using namespace cv;
using namespace std;

//Klasy
Ustawienia u;
Wspolrzedne w;

///Zmienne
Mat ikonaInformacji;
Mat ikonaUstawien;
Mat ikonaOdstepuPomiedzyZdjeciami[4];
Mat ikonaCzasuUsmiechu[4];
Mat ikonaFiltru[2];
Mat ekranInformacji;
Mat ekranUstawien;
//podkreslenie - obraz "podswietlony"
Mat _ikonaInformacji;
Mat _ikonaUstawien;
Mat _ikonaOdstepuPomiedzyZdjeciami[4];
Mat _ikonaCzasuUsmiechu[4];
Mat _ikonaFiltru[2];
//Kaskady Haara
CascadeClassifier twarz;
CascadeClassifier usmiech;


int coJestPodswietlone = 0;
bool czyEkranInformacjiJestWidoczny = false;
bool czyEkranUstawienJestWidoczny = false;
int ktoryOdstepJestZaznaczony;
int ktoryCzasJestZaznaczony;
int ktoryFiltrJestZaznaczony = 0;


//Zamiana wartosci typu int na string
string zamienIntaNaStringa(int wartosc) {
    ostringstream oss;
    oss << wartosc;
    string przekonwertowany = "";
    przekonwertowany = oss.str();

    return przekonwertowany;
}

//Wczytanie pliku o podanej nazwie
Mat wczytajObraz(string nazwaPliku) {
    Mat mat = imread(nazwaPliku,CV_LOAD_IMAGE_COLOR);
    if(! mat.data ) {
        cout <<"Blad ladowania obrazka " + nazwaPliku<<endl ;
    }
    else {
        return mat;
    }
}

//Wczytywanie wszystkich plikow graficznych
void wczytajPliki() {
    string sciezkaDoObrazkow = "data/img/";

    ikonaInformacji = wczytajObraz(sciezkaDoObrazkow + "info.png");
    ikonaUstawien = wczytajObraz(sciezkaDoObrazkow + "ustawienia.png");
    _ikonaInformacji = wczytajObraz(sciezkaDoObrazkow + "_info.png");
    _ikonaUstawien = wczytajObraz(sciezkaDoObrazkow + "_ustawienia.png");
    ekranInformacji = wczytajObraz(sciezkaDoObrazkow + "informacje.png");
    ekranUstawien = wczytajObraz(sciezkaDoObrazkow + "ekranUstawien.png");

    for (int i=0; i<4; i++) {
        ikonaOdstepuPomiedzyZdjeciami[i] = wczytajObraz(sciezkaDoObrazkow + "odstep" + zamienIntaNaStringa(i) + ".png");
        ikonaCzasuUsmiechu[i] = wczytajObraz(sciezkaDoObrazkow + "czas" + zamienIntaNaStringa(i) + ".png");
        _ikonaOdstepuPomiedzyZdjeciami[i] = wczytajObraz(sciezkaDoObrazkow + "_odstep" + zamienIntaNaStringa(i) + ".png");
        _ikonaCzasuUsmiechu[i] = wczytajObraz(sciezkaDoObrazkow + "_czas" + zamienIntaNaStringa(i) + ".png");
        if (i < LICZBA_FILTROW) {
            ikonaFiltru[i] = wczytajObraz(sciezkaDoObrazkow + "filtr" + zamienIntaNaStringa(i) + ".png");
            _ikonaFiltru[i] = wczytajObraz(sciezkaDoObrazkow + "_filtr" + zamienIntaNaStringa(i) + ".png");
        }
    }

    string sciezkaDoKaskad = "data/cascades/";
    //Ladowanie klasyfikatorow Haara z plikow .xml

    if (!twarz.load(sciezkaDoKaskad + "haarcascade_frontalface_alt.xml")) {
        cerr<<"blad ladowania pliku 'haarcascade_frontalface_alt.xml'"<<endl;
        //return -2;
    }

    if (!usmiech.load(sciezkaDoKaskad + "haarcascade_smile.xml")) {
        cerr<<"blad ladowania pliku 'haarcascade_smile.xml'"<<endl;
        //return -3;
    }
}

//Sprawdzenie, czy piksel nalezy wkleic na bitampe, czy nie w zaleznosci od jego koloru
bool czyPikselJestTlem(Vec3b piksel) {
    bool czyJest = false;

    if (piksel[1] >= 255 - u.getTolerancja()
        && piksel[0] <= u.getTolerancja()
        && piksel[2] <= u.getTolerancja()) {

            czyJest = true;
        }

    return czyJest;
}

//Metoda pozwalajaca na "wklejenie" jednej bitmapy do drugiej pomijajac piksele w zielonym kolorze
Mat wklejObrazZPrzezroczystoscia(Mat obrazZrodlowy, Mat obrazWklejany, int x, int y) {
    //sprawdzenie, czy obraz nie wychodzi poza bitmape zrodlowa
    if (x < 0 && y <0 && (x + obrazWklejany.cols > obrazZrodlowy.cols) &&(y + obrazWklejany.rows > obrazZrodlowy.rows)) {
        cerr << "Wyjscie poza zakres obrazu!" <<endl;
        Mat mat;
        return mat; //zwrocenie pustej bitmapy, zeby wiedziec, ze cos poszlo nie tak
    }

    Mat nowaBitmapa = obrazZrodlowy.clone();

    for (int i=0; i<obrazWklejany.cols; i++) {
        for (int j=0; j<obrazWklejany.rows; j++) {
            if (czyPikselJestTlem(obrazWklejany.at<Vec3b>(i,j)) == false) {
                    nowaBitmapa.at<Vec3b>(i + y, j + x) = obrazWklejany.at<Vec3b>(i,j);
            }
        }
    }

    return nowaBitmapa;
}

//Metoda pozwalajaca na "wklejenie" jednej bitmapy do drugiej uzyskujac efekt polprzezroczystosci
Mat wklejObrazZPrzezroczystoscia(Mat obrazZrodlowy, Mat obrazWklejany, int x, int y, float alfa) {
    //sprawdzenie, czy obraz nie wychodzi poza bitmape zrodlowa
    if (x < 0 && y <0 && (x + obrazWklejany.cols > obrazZrodlowy.cols) &&(y + obrazWklejany.rows > obrazZrodlowy.rows)) {
        cerr << "Wyjscie poza zakres obrazu!" <<endl;
        Mat mat;
        return mat; //zwrocenie pustej bitmapy, zeby wiedziec, ze cos poszlo nie tak
    }

    Mat nowaBitmapa = obrazZrodlowy.clone();

    for (int i=0; i<obrazWklejany.cols; i++) {
        for (int j=0; j<obrazWklejany.rows; j++) {
            nowaBitmapa.at<Vec3b>(i + y, j + x)[0] =
                ((nowaBitmapa.at<Vec3b>(i + y, j + x)[0] * (1-alfa)) + (obrazWklejany.at<Vec3b>(i,j)[0] * alfa)) / 2;
            nowaBitmapa.at<Vec3b>(i + y, j + x)[1] =
                ((nowaBitmapa.at<Vec3b>(i + y, j + x)[1] * (1-alfa)) + (obrazWklejany.at<Vec3b>(i,j)[1] * alfa)) / 2;
            nowaBitmapa.at<Vec3b>(i + y, j + x)[2] =
                ((nowaBitmapa.at<Vec3b>(i + y, j + x)[2] * (1-alfa)) + (obrazWklejany.at<Vec3b>(i,j)[2] * alfa)) / 2;
        }
    }

    return nowaBitmapa;
}

//Ustalenie ktore opcje z ustawien sa podswietlone
void ustalPodswietlenie() {
    switch (u.getOdstepPomiedzyZdjeciami()) {
    case 25:
       ktoryOdstepJestZaznaczony = 0;
       break;
    case 50:
        ktoryOdstepJestZaznaczony = 1;
        break;
    case 75:
        ktoryOdstepJestZaznaczony = 2;
        break;
    case 100:
        ktoryOdstepJestZaznaczony = 3;
        break;
    }

    switch (u.getliczbaKlatekUsmiechu()) {
    case 1:
       ktoryCzasJestZaznaczony = 0;
       break;
    case 3:
        ktoryCzasJestZaznaczony = 1;
        break;
    case 5:
        ktoryCzasJestZaznaczony = 2;
        break;
    case 10:
        ktoryCzasJestZaznaczony = 3;
        break;
    }

       ktoryFiltrJestZaznaczony = u.getktoryFiltr();
}

//Ustawienie odpowiedniego przycisku po klikniecu
void ustalPrzycisk() {
    switch (ktoryOdstepJestZaznaczony) {
    case 0:
        u.setOdstepPomiedzyZdjeciami(25);
        break;
    case 1:
        u.setOdstepPomiedzyZdjeciami(50);
        break;
    case 2:
        u.setOdstepPomiedzyZdjeciami(75);
        break;
    case 3:
        u.setOdstepPomiedzyZdjeciami(100);
        break;
    }

    switch (ktoryCzasJestZaznaczony) {
    case 0:
        u.setliczbaKlatekUsmiechu(1);
        break;
    case 1:
        u.setliczbaKlatekUsmiechu(3);
        break;
    case 2:
        u.setliczbaKlatekUsmiechu(5);
        break;
    case 3:
        u.setliczbaKlatekUsmiechu(10);
        break;
    }

    u.setktoryFiltr(ktoryFiltrJestZaznaczony);
}

//Nalozenie ikon i innych obrazow na bitmape przechwycona z kamerki
Mat rysujHUD (Mat obrazZrodlowy) {
    Mat mat = obrazZrodlowy;

    mat = wklejObrazZPrzezroczystoscia(mat, ikonaUstawien, w.getWspolrzedne(USTAWIENIA).x, w.getWspolrzedne(USTAWIENIA).y);
    mat = wklejObrazZPrzezroczystoscia(mat, ikonaInformacji, w.getWspolrzedne(INFO).x, w.getWspolrzedne(INFO).y);

    if (czyEkranInformacjiJestWidoczny) {
        mat = wklejObrazZPrzezroczystoscia(mat, ekranInformacji, w.getWspolrzedne(INFORMACJE).x, w.getWspolrzedne(INFORMACJE).y, ALFA);
    }
    if (czyEkranUstawienJestWidoczny) {
        mat = wklejObrazZPrzezroczystoscia(mat, ekranUstawien, w.getWspolrzedne(EKRAN_USTAWIEN).x, w.getWspolrzedne(EKRAN_USTAWIEN).y, ALFA);

        for (int i=0; i < 4; i++) {
            mat = wklejObrazZPrzezroczystoscia(mat, ikonaOdstepuPomiedzyZdjeciami[i], w.getWspolrzedne(ODSTEP_POMIEDZY_ZDJECIAMI+i).x, w.getWspolrzedne(ODSTEP_POMIEDZY_ZDJECIAMI+i).y);
            mat = wklejObrazZPrzezroczystoscia(mat, ikonaCzasuUsmiechu[i], w.getWspolrzedne(CZAS_WYKONANIA_USMIECHU+i).x, w.getWspolrzedne(CZAS_WYKONANIA_USMIECHU+i).y);
            if (i < LICZBA_FILTROW) {
                mat = wklejObrazZPrzezroczystoscia(mat, ikonaFiltru[i], w.getWspolrzedne(FILTR+i).x, w.getWspolrzedne(FILTR+i).y);
            }
        }

        for (int i=0; i < 4; i++) {
        if (ktoryOdstepJestZaznaczony == i)
            mat = wklejObrazZPrzezroczystoscia(mat, _ikonaOdstepuPomiedzyZdjeciami[i], w.getWspolrzedne(ODSTEP_POMIEDZY_ZDJECIAMI+i).x, w.getWspolrzedne(ODSTEP_POMIEDZY_ZDJECIAMI+i).y);
        if (ktoryCzasJestZaznaczony == i)
        mat = wklejObrazZPrzezroczystoscia(mat, _ikonaCzasuUsmiechu[i], w.getWspolrzedne(CZAS_WYKONANIA_USMIECHU+i).x, w.getWspolrzedne(CZAS_WYKONANIA_USMIECHU+i).y);
        if (i < LICZBA_FILTROW) {
            if (ktoryFiltrJestZaznaczony == i)
                mat = wklejObrazZPrzezroczystoscia(mat, _ikonaFiltru[i], w.getWspolrzedne(FILTR+i).x, w.getWspolrzedne(FILTR+i).y);
        }
    }
    }



    switch (coJestPodswietlone) {
    case INFO:
        mat = wklejObrazZPrzezroczystoscia(mat, _ikonaInformacji, w.getWspolrzedne(INFO).x, w.getWspolrzedne(INFO).y);
        break;
    case USTAWIENIA:
        mat = wklejObrazZPrzezroczystoscia(mat, _ikonaUstawien, w.getWspolrzedne(USTAWIENIA).x, w.getWspolrzedne(USTAWIENIA).y);
        break;
    }

    return mat;
}

//Metoda obslugujaca mysz
static void obslugaMyszy( int event, int x, int y, int flags, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        if (w.czyNajechanoNaPrzycisk(x,y) == INFO) {
            czyEkranUstawienJestWidoczny = false;

            if (czyEkranInformacjiJestWidoczny)
                czyEkranInformacjiJestWidoczny = false;
            else czyEkranInformacjiJestWidoczny = true;
        }

        if (w.czyNajechanoNaPrzycisk(x,y) == USTAWIENIA) {
            czyEkranInformacjiJestWidoczny = false;

            if (czyEkranUstawienJestWidoczny)
                czyEkranUstawienJestWidoczny = false;
            else czyEkranUstawienJestWidoczny = true;
        }

        if (w.czyNajechanoNaPrzycisk(x,y) & ODSTEP_POMIEDZY_ZDJECIAMI) {
            int i = w.czyNajechanoNaPrzycisk(x,y) - ODSTEP_POMIEDZY_ZDJECIAMI;
            ktoryOdstepJestZaznaczony = i;
            ustalPrzycisk();
        }

        if (w.czyNajechanoNaPrzycisk(x,y) & CZAS_WYKONANIA_USMIECHU) {
            int i = w.czyNajechanoNaPrzycisk(x,y) - CZAS_WYKONANIA_USMIECHU;
            ktoryCzasJestZaznaczony = i;
            ustalPrzycisk();
        }

        if (w.czyNajechanoNaPrzycisk(x,y) & FILTR) {
            int i = w.czyNajechanoNaPrzycisk(x,y) - FILTR;
            ktoryFiltrJestZaznaczony = i;
            ustalPrzycisk();
        }
    }
    if (event == EVENT_MOUSEMOVE) {
        coJestPodswietlone = w.czyNajechanoNaPrzycisk(x,y);
    }
}

//Zamiana obrazu w kolorze na czarno bialy
Mat zamienNaSkaleSzarosci(Mat mat) {
    for (int i=0; i<mat.cols; i++) {
        for (int j=0; j<mat.rows; j++) {
            Vec3b piksel;
            piksel = mat.at<Vec3b>(j, i);
            piksel[0] = (piksel[0] + piksel[1] + piksel[2])/3;
            piksel[1] = piksel[0];
            piksel[2] = piksel[0];
            mat.at<Vec3b>(j, i) = piksel;
        }
    }
    return mat;
}

//Metoda wykrywajaca twarz, dodatkowo wykrywa na niej usmiech
int wykryjTwarz() {
    Mat frame;
    Mat frameToDisplay;
    Mat skalaSzarosci;
    VideoCapture cap(0);

    //Znalezione wzorce sa przechowywane w tych tablicach prostokatow
    vector<Rect> znalezioneTwarze;
    vector<Rect> znalezioneUsmiechy;

    int ileWykrytoTwarzy;
    int ileWykrytoUsmiechow;
    bool czyWszystkieRegionyMajaUsmiech;
    int blysk;
    int ileMineloOdOstatniegoZdjecia;
    int numerZdjecia = 1;
    int czasOczekiwaniaDoZrobieniaZdjecia = 0;

    namedWindow("Projekt ZPO - Detekcja usmiechu",CV_WINDOW_AUTOSIZE);

    //Ustawienie obslugi myszy
    setMouseCallback( "Projekt ZPO - Detekcja usmiechu", obslugaMyszy, NULL );


    if (!cap.isOpened()) {
        cerr  << "Nie mozna otworzyc kamerki" << endl;
        return -1;
    }

    wczytajPliki();

    cap.read(frame);    //bugfix - z jakiegos powodu pierwsza klatka jest pusta

////////////////////////////////////////////////////////////////////////////////////////////////////
    ///Glowna petla///
    while (true) {
        ileWykrytoTwarzy = 0;
        ileWykrytoUsmiechow = 0;

        czyWszystkieRegionyMajaUsmiech = true;


       //Odczyt klatki obrazu z kamerki
       bool odczytano = cap.read(frame);
       if (!odczytano)
        {
         cout << "Nie mozna odczytac obrazu z kamerki" << endl;
            break;
        }

        //Nalozenie filtru jesli jest wlaczony
        if (u.getktoryFiltr()) {
            if (u.getktoryFiltr() == 1) {
                frame = zamienNaSkaleSzarosci(frame);
            }
        }

        //Dodatkowa bitmapa - to ona zostanie zapisana, na 'frame' beda pokazywane dodatkowe rzeczy, ktore beda widozne na ekranie
        Mat oryginal = frame.clone();

        //bialy ekran przez kilka klatek po zrobieniu "zdjecia", efekt przypomina flesz aparatu
        if (blysk) {
            frame = Mat(frame.rows, frame.cols, CV_8UC3, Scalar(255,255,255));
            blysk--;
        }

        ileMineloOdOstatniegoZdjecia++;

        //Zamiana na skale szarosci i wyrownanie histogramu dla lepszego efektu wykrywania twarzy
        skalaSzarosci = frame.clone();
        cvtColor(frame, skalaSzarosci, CV_BGR2GRAY);
        equalizeHist(skalaSzarosci, skalaSzarosci);

        //Wykrywanie twarzy za pomoca kaskad Haara
        twarz.detectMultiScale(skalaSzarosci, znalezioneTwarze, 1.2, 3, 0 | CASCADE_SCALE_IMAGE, Size(frame.rows/10, frame.cols/10));
        //Jesli znaleziono twarz(e) to:
        if (znalezioneTwarze.size() > 0) {
            ileWykrytoTwarzy = znalezioneTwarze.size();
            for (int i=0; i<znalezioneTwarze.size(); i++) {

                rectangle(frame, znalezioneTwarze[i], Scalar(0, 0, 255), 2, 8, 0);

                //Wycinanie prostokata ze znaleziona twarza, w nim beda szukane same usta
                Mat Region = frame(znalezioneTwarze[i]);
                cvtColor(Region, Region, CV_BGR2GRAY);
                equalizeHist(Region, Region);

                //Wykrywanie usmiechu za pomoca kaskad Haara
                usmiech.detectMultiScale(Region, znalezioneUsmiechy, 1.2, 3, 0 | CASCADE_SCALE_IMAGE,
                                         Size(znalezioneTwarze[i].height/5, znalezioneTwarze[i].width/5));
                for (int j=0; j<znalezioneUsmiechy.size(); j++) {
                    znalezioneUsmiechy[j].x += znalezioneTwarze[i].x;
                    znalezioneUsmiechy[j].y += znalezioneTwarze[i].y;
                    rectangle(frame, znalezioneUsmiechy[j], Scalar(0, 255, 0), 2, 8, 0);
                    ileWykrytoUsmiechow++;
                }

                //Jesli choc jedna twarz nie ma usmiechu, to "zdjecie" nie bedzie wykonane
                if (znalezioneUsmiechy.size() < 1) {
                    czyWszystkieRegionyMajaUsmiech = false;
                    czasOczekiwaniaDoZrobieniaZdjecia = 0;
                }
            }
        }

        //Sprawdzenie, czy spelniono warunki niezbedne do zrobienia "zdjecia"
        if (ileWykrytoTwarzy > 0) {
            if (ileWykrytoUsmiechow >= ileWykrytoTwarzy) {
                if (czyWszystkieRegionyMajaUsmiech) {
                    if (ileMineloOdOstatniegoZdjecia >= u.getOdstepPomiedzyZdjeciami())
                        if (czasOczekiwaniaDoZrobieniaZdjecia >= u.getliczbaKlatekUsmiechu()) {
                            imshow("Zdjecie", oryginal);
                            blysk = 3;
                            ileMineloOdOstatniegoZdjecia = 0;
                            czasOczekiwaniaDoZrobieniaZdjecia = 0;

                            //Parametry zapisu pliku .png
                            vector<int> parametry;
                            parametry.push_back(CV_IMWRITE_PNG_COMPRESSION);
                            parametry.push_back(5);

                            //Zapis do pliku zdjec normalnych i nalozonymi prostokatami pokazujacymi twarz i usmiech
                            string nazwaZdjecia;
                            nazwaZdjecia = u.getKatalogZeZdjeciami() + "Zdjecie" + zamienIntaNaStringa(numerZdjecia) + ".png";
                            imwrite(nazwaZdjecia,oryginal,parametry);
                            string nazwaZdjeciaZDetekcja;
                            nazwaZdjeciaZDetekcja = u.getKatalogZeZdjeciami() + "ZdjecieZDetekcja" + zamienIntaNaStringa(numerZdjecia) + ".png";
                            imwrite(nazwaZdjeciaZDetekcja,frame,parametry);

                            numerZdjecia++;
                        }
                        else {  //gdy wszystkie warunki sa spelnione - odczekanie kilku klatek, zeby usmiechy byly pelne
                            czasOczekiwaniaDoZrobieniaZdjecia++;
                        }
                    }
                }
            }

        if (odczytano) {
            //Rysowanie HUD'a
            ustalPodswietlenie();
            frameToDisplay = frame.clone();
            frameToDisplay = rysujHUD(frame);

            imshow("Projekt ZPO - Detekcja usmiechu", frameToDisplay);
        }

        if (waitKey(30) == 27) {
            destroyWindow("Projekt ZPO - Detekcja usmiechu");
            return 0;
        }
    }

    return 0;
}

int main ( ) {
    cout<<"Projekt z zaawansowanego przetwarzania obrazow"<<endl;
    cout<<"Wykonanie: Rafal Bebenek i Jacek Kominek"<<endl;
    cout<<"Rok akademicki: 2014/2015"<<endl<<endl;
    cout<<"ESC - wyjscie"<<endl;

    wykryjTwarz();

    return 0 ;
}
