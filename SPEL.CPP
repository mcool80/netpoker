#include "spel.h"
//char spel[10][50] = { "inget", "ett par", "tv\x86 par", "tretal", "stege", "f\x84rg", "k\x86k", "fyrtal", "stege med f\x84rg", "h\x94g stege med f\x84rg" };
//enum valorer { nothing=-1,tva=0,tre,fyra,fem,sex,sju,atta,nio,tio,knekt,dam,kung,ess};
Game::Game(int ant) : antal_spelare(ant) {
   // Skapar poker spel
   poker = new Poker;
   // Skapar grafik modul
//   grafik = new Grafik;
   // Skriver överskrift
//   grafik->skrivtext(1,1,"Markus Objekt Orienterade Poker");
   // Hämtar kort till spelarna
   for ( int i=0;i<ant;i++ ) {
      for( int t=0;t<5;t++ )
         // Sätter valör på spelarnas kort till -1 dvs nothing
         spelarnaskort[i][t].setvalor(nothing);
      // Dra 5 kort till spelarens hand
      poker->drakort(spelarnaskort[i],5);
   }
}

void Game::kortforspelare(int n, Kort k[5])
{
   for ( int t=0;t<5;t++)
      k[t] = spelarnaskort[n][t];
}
// Slänger en spelare kort
int Game::slangkort(int n, int baksida[5]) {

   // Anta att inga slängs
   int ant_slang=0;

   for ( int t=0;t<5;t++)
   // Om någon ligger med baksida så ska den slängas
      if ( baksida[t] ) {
         ant_slang++;
         poker->gekort(spelarnaskort[n][t]);
      }
   return ant_slang;
}
void Game::hemtakort(int n, int ant) {
   // Drar kort från kortlek
   poker->drakort(spelarnaskort[n],ant);
}
void Game::spelvinnare(int &n,Kort kort[5],int ordning[5]) {
   // Hämtar aktuell ställning i spelet
   n = poker->vinnare(spelarnaskort,antal_spelare,ordning);
   // skriver korten från vinnare i kort[5] variabeln
   for ( int i = 0; i < 5 ; i++)
      kort[i] = spelarnaskort[n][i];

}
