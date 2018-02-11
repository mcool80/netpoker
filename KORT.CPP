#include "kort.h"

char cvalorer[15][10] = {"2","3","4","5","6","7","8","9","10","Kn","D","K","E"};
char cfarger[5][5]= { "\x6", "\x4", "\x5", "\x3" };

// --- KORT KLASS ---

char *Kort::getchfarg(void) {
   // Returnerar färgen i text
   return cfarger[(int)farg];
}

char *Kort::getchvalor(void) {
   // Returnerar valören i text
   return cvalorer[(int)valor];
}
// ------------------

// -- KORTLEK KLASS --
Kort Kortlek::takort(void) {

   // Hämtar kortet som skall tas ur leken
   Kort ret = allakort[kort_i_lek[antal_i_lek]];

   // Sätter kortets valör till -1 dvs det finns inget
   allakort[kort_i_lek[antal_i_lek--]].setvalor(-1);

   // Om inga kort finns i leken i kortleken blanda om kortleken
   if ( antal_i_lek < 0 ) blanda_kortlek();
      return ret;
}

void Kortlek::gekort(Kort kort) {

   // Ger ett kort till leken
   // Letar efter en tom position dvs där valören är -1
   for( int i=0;i<ANTAL_KORT;i++ )
      // Lägger kortet där
      if ( allakort[i].getvalor() == -1 ) {
         allakort[i]=kort;
         // Kortet är inlagt en gång avsluta loopen
         break;
      }
}

int Kortlek::init(void) {

   // Antalkort från början är 52 position 0...51
   antal_i_lek=ANTAL_KORT-1;

   // Lägger till alla kort i leken
   for(int i=0;i<ANTAL_KORT;i++) {
        // Valörer 0..13 och färger 0..3
   	  allakort[i].setfarg((Kort::farger)i/13);
   	  allakort[i].setvalor((valorer)i%13);
        // Lägger korten i en sorterad ordning
   	  kort_i_lek[i] = i;
   }
   return 1;
}

int Kortlek::blanda_kortlek(void) {      

   // Randomize nu ska det slumpas kort
   randomize();

   // Antalet är högst 52 (0...51)
   int ant=ANTAL_KORT-1;

   // Letar efter ett kort med valör -1 dvs det finns inte dvs totala antalet minskar
   for(int i=0;i<ANTAL_KORT;i++)
      if ( allakort[i].getvalor() == -1 ) ant--;

   // Totalt antal kort
   antal_i_lek=ant;

   // Blandar kvarvarande kort
   for(int i=0;i<=ant;i++) {

     // Slumpar ett tal som används som index i allakort[i]
     kort_i_lek[i]=random(ANTAL_KORT);
     int n=i-1;

     // Kollar alla tidigare kort
     while(n>-1)
        // Om kortet redan är valt eller det kortet har valören -1 så välj ett annat kort
        if ( allakort[kort_i_lek[i]].getvalor() == -1 || kort_i_lek[i] == kort_i_lek[n--] ) {
           i--;
           n=-1;
     }
   }
   return 1;
}
// -------------------
