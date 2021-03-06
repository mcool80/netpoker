#include "kort.h"
#ifndef _POKER
#define _POKER
class Poker {
   public:
      // -- Konstruktor ---------------------------------------
      // Initsierar kortlek och blandar den
      // ------------------------------------------------------
      Poker();

      // -- drakort( Kort [5], int ) --------------------------
      // H�mtar kort fr�n kortlek
      // Indata: 5 kort som ska f� kort,
      //         antal kort som skall h�mtas
      // Utdata: 5 kort som f�rr kort
      // Returnerar: 0 vid ok
      // ------------------------------------------------------
      int drakort(Kort spelare[5],int antal);

      // -- gekort( Kort & ) ----------------------------------
      // Ger ett kort till kortlek och s�tter kortet till
      // nothing
      // Indata: Ett kort
      // Utdata: Ett kort
      // Returnerar: 0 vid ok
      // ------------------------------------------------------
      int gekort(Kort &kort);

      // -- spelares_hand( Kort[5] ) --------------------------
      // H�mtar vad f�r hand spelare har tex ett par
      // Indata: 5 kort
      // Utdata: Inget
      // Returnerar: 0 -> inget
      //				   1 -> ett par
      //				   2 -> tv� par
      //				   3 -> tretal
      //				   4 -> straight
      //				   5 -> flush
      //				   6 -> k�k
      //				   7 -> fyrtal
      //				   8 -> straight flush
      // 			   9 -> top straight flush
      // ------------------------------------------------------
      int spelares_hand( Kort kontroll[5] );

      // -- spelares_hand( Kort[5][5], int, int[5]) -----------
      // Returnerar vinnaren och h�mtar �vriga
      // spelares position
      // Indata: Kort[5][5] Spelarnas kort
      //         int        antal spelare
      // Utdata: int[5]     Ordning f�r spelarna
      // Returnerar: int    vinnarens nummer
      // ------------------------------------------------------
      int vinnare(Kort spelare[5][5], int s, int ordning[5]);
   private:

      // -- jamfor(Kort[5], Kort[5] ) -------------------------
      // J�mf�r vilken spelare som har b�st kort
      // Indata: 2 st 5 kort
      // Returnerar: 0  -> lika
      //             1  -> spelare1
      //             2  -> spelare2
      // ------------------------------------------------------
      int jamfor( Kort spelare1[5], Kort spelare2[5]);

      // -- antalvalor( int[14], Kort[5] ) --------------------
      // L�gger antalkort av var slag i en int lista
      // Tex finns det tv� treor l�ggs 2 in i int[1] = 2
      // (f�r att val�r 2 har v�rdet 0 i listan)
      // Indata: 5 kort
      // Utdata: En int lista [14] med antal per val�r
      // Returnerar: inget
      // ------------------------------------------------------
      void antalvalor(int antl[14], Kort kontroll[5]);

      // Kontrollerar om det �r stege
      // -- straight( Kort[5] ) -------------------------------
      // Kontrollerar om korten �r en stege
      // Indata: 5 kort
      // Returnerar: 0 vid ingen stege, 4 vid stege och
      //             9 vid topp stege
      // ------------------------------------------------------
      int straight( Kort kontroll[5]  );

      // -- sortera( int [5] ) --------------------------------
      // Sorterar 5 val�rer i nummerordning med l�gst tal f�rst
      // Indata: en array med siffror
      // Returnerar: inget
      // ------------------------------------------------------
      void sortera( int valorer[5] );

      // -- flush( Kort[5] ) ----------------------------------
      // Kontrollerar om alla korten har samma f�rg
      // Indata: 5 kort
      // Returnerar: sant om det �r f�rg falsk om inte
      // ------------------------------------------------------
      bool flush( Kort kontroll[5]);
      Kortlek kortlek; // Kortlek till spelet
};
#endif