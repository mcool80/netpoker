#include "poker.h"
#ifndef _GAMEH
#define _GAMEH
// De olika kombinationerna man kan få i poker
// Swedish words
//char szspel[10][50] = { "inget", "ett par", "två par", "tretal", "stege", "färg", "kåk", "fyrtal", "stege med färg", "hög stege med färg" };
// English
char szspel[10][50] = { "nothing",
                        "one pair",
                        "two pair",
                        "trippel",
                        "Straight",
                        "flush",
                        "full house",
                        "four",
                        "straight flush",
                        "high straight flush" };
class Game {
   public:
      Game(int ant);
      ~Game() { delete poker; }
      // Spela driver spelet där väljer spelarna kort
//    	void spela(void);

      // visar resultatet vem som vunnit coh vilka kort spelarna hade
//      void visaresultat(void);

      // Callback funktioner tillagda för att kunna köra spelet i händelse styrt os
      // Returnerar antal spelare i spelet
      int antalspelare(void) { return antal_spelare; };
      // Hämtar kort för en spelare
      void kortforspelare(int n, Kort k[5]);
      // Slänger kort för en spelare
      int slangkort(int n, int baksida[5]);
      // Hämtar kort till en spelare
      void hemtakort(int n, int ant);
      // Hämtar ordning i spelet samt vinnarens kort
      void spelvinnare(int &n,Kort kort[5],int ordning[5]);
      // Returnerar en spelares hand i text
      char *handforspelare(int n) { return szspel[poker->spelares_hand( spelarnaskort[n] )]; }

      // --------------------------------------------------------------------------
   private:
      // antal spelare
      int antal_spelare;
      // korten spelarna har
      Kort spelarnaskort[5][5];
      // Poker spelet
      Poker *poker;
      // Grafik modul
//      Grafik *grafik;
};

#endif