#include "kort.h"

char cvalorer[15][10] = {"2","3","4","5","6","7","8","9","10","Kn","D","K","E"};
char cfarger[5][5]= { "\x6", "\x4", "\x5", "\x3" };

// --- KORT KLASS ---

char *Kort::getchfarg(void) {
   // Returnerar f�rgen i text
   return cfarger[(int)farg];
}

char *Kort::getchvalor(void) {
   // Returnerar val�ren i text
   return cvalorer[(int)valor];
}
// ------------------

// -- KORTLEK KLASS --
Kort Kortlek::takort(void) {

   // H�mtar kortet som skall tas ur leken
   Kort ret = allakort[kort_i_lek[antal_i_lek]];

   // S�tter kortets val�r till -1 dvs det finns inget
   allakort[kort_i_lek[antal_i_lek--]].setvalor(-1);

   // Om inga kort finns i leken i kortleken blanda om kortleken
   if ( antal_i_lek < 0 ) blanda_kortlek();
      return ret;
}

void Kortlek::gekort(Kort kort) {

   // Ger ett kort till leken
   // Letar efter en tom position dvs d�r val�ren �r -1
   for( int i=0;i<ANTAL_KORT;i++ )
      // L�gger kortet d�r
      if ( allakort[i].getvalor() == -1 ) {
         allakort[i]=kort;
         // Kortet �r inlagt en g�ng avsluta loopen
         break;
      }
}

int Kortlek::init(void) {

   // Antalkort fr�n b�rjan �r 52 position 0...51
   antal_i_lek=ANTAL_KORT-1;

   // L�gger till alla kort i leken
   for(int i=0;i<ANTAL_KORT;i++) {
        // Val�rer 0..13 och f�rger 0..3
   	  allakort[i].setfarg((Kort::farger)i/13);
   	  allakort[i].setvalor((valorer)i%13);
        // L�gger korten i en sorterad ordning
   	  kort_i_lek[i] = i;
   }
   return 1;
}

int Kortlek::blanda_kortlek(void) {      

   // Randomize nu ska det slumpas kort
   randomize();

   // Antalet �r h�gst 52 (0...51)
   int ant=ANTAL_KORT-1;

   // Letar efter ett kort med val�r -1 dvs det finns inte dvs totala antalet minskar
   for(int i=0;i<ANTAL_KORT;i++)
      if ( allakort[i].getvalor() == -1 ) ant--;

   // Totalt antal kort
   antal_i_lek=ant;

   // Blandar kvarvarande kort
   for(int i=0;i<=ant;i++) {

     // Slumpar ett tal som anv�nds som index i allakort[i]
     kort_i_lek[i]=random(ANTAL_KORT);
     int n=i-1;

     // Kollar alla tidigare kort
     while(n>-1)
        // Om kortet redan �r valt eller det kortet har val�ren -1 s� v�lj ett annat kort
        if ( allakort[kort_i_lek[i]].getvalor() == -1 || kort_i_lek[i] == kort_i_lek[n--] ) {
           i--;
           n=-1;
     }
   }
   return 1;
}
// -------------------
