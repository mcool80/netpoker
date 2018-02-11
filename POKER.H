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
      // Hämtar kort från kortlek
      // Indata: 5 kort som ska få kort,
      //         antal kort som skall hämtas
      // Utdata: 5 kort som fårr kort
      // Returnerar: 0 vid ok
      // ------------------------------------------------------
      int drakort(Kort spelare[5],int antal);

      // -- gekort( Kort & ) ----------------------------------
      // Ger ett kort till kortlek och sätter kortet till
      // nothing
      // Indata: Ett kort
      // Utdata: Ett kort
      // Returnerar: 0 vid ok
      // ------------------------------------------------------
      int gekort(Kort &kort);

      // -- spelares_hand( Kort[5] ) --------------------------
      // Hämtar vad för hand spelare har tex ett par
      // Indata: 5 kort
      // Utdata: Inget
      // Returnerar: 0 -> inget
      //				   1 -> ett par
      //				   2 -> två par
      //				   3 -> tretal
      //				   4 -> straight
      //				   5 -> flush
      //				   6 -> kåk
      //				   7 -> fyrtal
      //				   8 -> straight flush
      // 			   9 -> top straight flush
      // ------------------------------------------------------
      int spelares_hand( Kort kontroll[5] );

      // -- spelares_hand( Kort[5][5], int, int[5]) -----------
      // Returnerar vinnaren och hämtar övriga
      // spelares position
      // Indata: Kort[5][5] Spelarnas kort
      //         int        antal spelare
      // Utdata: int[5]     Ordning för spelarna
      // Returnerar: int    vinnarens nummer
      // ------------------------------------------------------
      int vinnare(Kort spelare[5][5], int s, int ordning[5]);
   private:

      // -- jamfor(Kort[5], Kort[5] ) -------------------------
      // Jämför vilken spelare som har bäst kort
      // Indata: 2 st 5 kort
      // Returnerar: 0  -> lika
      //             1  -> spelare1
      //             2  -> spelare2
      // ------------------------------------------------------
      int jamfor( Kort spelare1[5], Kort spelare2[5]);

      // -- antalvalor( int[14], Kort[5] ) --------------------
      // Lägger antalkort av var slag i en int lista
      // Tex finns det två treor läggs 2 in i int[1] = 2
      // (för att valör 2 har värdet 0 i listan)
      // Indata: 5 kort
      // Utdata: En int lista [14] med antal per valör
      // Returnerar: inget
      // ------------------------------------------------------
      void antalvalor(int antl[14], Kort kontroll[5]);

      // Kontrollerar om det är stege
      // -- straight( Kort[5] ) -------------------------------
      // Kontrollerar om korten är en stege
      // Indata: 5 kort
      // Returnerar: 0 vid ingen stege, 4 vid stege och
      //             9 vid topp stege
      // ------------------------------------------------------
      int straight( Kort kontroll[5]  );

      // -- sortera( int [5] ) --------------------------------
      // Sorterar 5 valörer i nummerordning med lägst tal först
      // Indata: en array med siffror
      // Returnerar: inget
      // ------------------------------------------------------
      void sortera( int valorer[5] );

      // -- flush( Kort[5] ) ----------------------------------
      // Kontrollerar om alla korten har samma färg
      // Indata: 5 kort
      // Returnerar: sant om det är färg falsk om inte
      // ------------------------------------------------------
      bool flush( Kort kontroll[5]);
      Kortlek kortlek; // Kortlek till spelet
};
#endif