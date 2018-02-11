#include "poker.h"

Poker::Poker() {
   // Inistierar kortleken
   kortlek.init();
   // Blanda kortleken
   kortlek.blanda_kortlek();
}

int Poker::drakort(Kort spelare[5], int antal) {

   // Drar kort
   for(int i=0;i<5||antal;i++)
      // Om valören på kortet är nothing så skall ett hämtas
      if ( spelare[i].getvalor()==nothing) {
         // Hämtar kort från kortleken
         spelare[i]=kortlek.takort();
         antal--;
      }
   return 1;
}

int Poker::gekort(Kort &kort) {

   // Ger ett kort till kortleken
   kortlek.gekort(kort);

   // Tar bort kortet
   kort.setvalor(nothing);
   return 0;
}

bool Poker::flush( Kort kontroll[5]) {

   // Array med färgerna
   int farg[5];

   // Hämtar färgerna till array
   for ( int i=0; i<5; i++)
      farg[i] = kontroll[i].getfarg();

   // Kollar om kortens färger stämmer element(n)'s färg = element(n-1)'s färg
   for ( int i = 1; i < 5; i++ )
     // Inte samma return false
     if ( farg[i]!=farg[i-1] )
        return false;

   // Samma färg
   return true;
}

void Poker::sortera( int valorer[5] ) {
   // Inte sorterad
   bool sorted=false;
   // Loopar till dess att alla värden ligger på rätt plats i ordning 0..13
   while ( !sorted ) {
      // Sorterad ?
      sorted = true;
      // Kollar element(n) med element(n+11)
      // element(n+1) ska vara större
      for ( int i=0;i<4;i++)
         if ( valorer[i]>valorer[i+1] ) {
            // Om det inte är så byter valörerna plats
            int a = valorer[i];
            valorer[i] = valorer[i+1];
            valorer[i+1] = a;
            // inte sorterat
            sorted = false;
         }
   }
}

int Poker::straight( Kort kontroll[5]  ) {

   // Värden i array
   int valor[5];

   // Hämtar värden till array
   for ( int i = 0; i <5; i++)
      valor[i] = kontroll[i].getvalor();

   // Sorterar värdena stigande
   sortera(valor);

   // Kontrollerar om det är ess 2 3 4 5   OBS specialfall
   if ( valor[4] == 12 && valor[0] == 0 &&
        valor[1] == 1  && valor[2] == 2 &&
        valor[3] == 3 ) return 4;

   // Kollar om element(n)'s värde = element(n-1)'s värde +1
   // stämmer inte det så returneras 0
   for ( int i=1; i<5; i++ ) {
      if ( valor[i]!=valor[i-1]+1 ) return false;
   }

   // Returnerar 9 om det är top
   if ( valor[0] == 9 ) return 9;
   // Vanlig stege
   else return 4;
}

void Poker::antalvalor( int antl[14], Kort kontroll[5] ) {
   int valor[5];  // Kortens värden

   // Hämtar värden
   for ( int i=0; i<5; i++ )
      valor[i] = kontroll[i].getvalor();
   // Nollställer array med värden för varje värde på kort
   for ( int i=0; i<14; i++ )
      antl[i] = 0;
   // Går igenom kortens värden och ökar värde arrayen där kort finns
   for ( int i = 0; i<5; i++ )
      antl[valor[i]]++;
}

int Poker::spelares_hand( Kort kontroll[5] ) {

   // Nollställer array med värden för varje värde på kort */
   int antalpervalor[14];

   // Nollställer array med antal par tretal och fyrtal
   // Position 0 -> ental, 1 -> par, 2 -> tretaö, 3 -> fyrtal
   int antalen[4] = { 0, 0, 0, 0 };

   // Går igenom kortens värden och ökar värde arrayen där kort finns
   antalvalor(antalpervalor,kontroll);

   // Lägger till ental, par, två par, tretal samt fyrtal
   for ( int i=0; i<14 ; i++)
      if ( antalpervalor[i] )
         antalen[antalpervalor[i]-1]++;

   // Kollar om det finns något par
   if ( antalen[1] )
      // finns det tretal är det kåk
      if ( antalen[2] ) return 6;
      // finns det mer en ett par är det två par
      else if ( antalen[1]>1 ) return 2;
      // Annars är det ett par
         else return 1;

   // Är det tretal
   if ( antalen[2] ) return 3;
   // Är det det fyrtal
   if ( antalen[3] ) return 7;

   // Hämtar flush och straignt
   int fl= flush(kontroll), st=straight(kontroll);
   // Är det flush och straight eller tom top straight flush
   if ( fl && st ) if ( st == 9 ) return 9;
      else return 8;

   // Är det bara straight
   if ( st ) return 4;
   // Är det flush
   if ( fl ) return 5;

   // Är det inget
   return 0;
}

int Poker::jamfor( Kort spelare1[5], Kort spelare2[5] ) {
   // Hämtar poäng för spelarnas händer
   int p = spelares_hand(spelare1);
   int p2 = spelares_hand(spelare2);

   if ( p == p2 ) {

      int antalpervalor[14];
      int antalpervalor2[14];
      // Hämtar antal förhållande till värden
      antalvalor(antalpervalor,spelare1);
      antalvalor(antalpervalor2,spelare2);

      // Kollar 4tal 3tal 2par och 1par och ental en efter en
      for (int n = 3; n>0; n--)
      // Kollar högsta värdet 13...0
      for ( int i=13; i>=0;i-- ) {
         // Om n st kort med värdet existerar hos endast en så returnera vinnaren
         if ( antalpervalor[i]==n && antalpervalor2[i]<n ) return 1;
         if ( antalpervalor2[i]==n && antalpervalor[i]<n ) return 2;
      }
   } else {
      // returnerar den som har mest poäng
      if ( p>p2 ) return 1; else return 2;
   }
   // Spelarna har samma poäng och värden på handen
   return 0;
}

int Poker::vinnare( Kort spelare[5][5], int s, int ordning[5] ) {
   // Sätter alla spelare till s-1 dvs den som inte vinner någon match får s-1 och den som vinner har 0
   int klart[5] = { s-1, s-1, s-1, s-1, s-1 };
   for ( int i = 0; i < s ; i++ )
      for ( int t = 0; t < s; t++ )
        // Vinner spelaren i minskar klart[i] men blir aldrig 0 för man man bara vinna s-1 gånger
        if ( jamfor(spelare[i],spelare[t]) == 1 ) klart[i]--;
   // Ordnar så att allt kommer rätt
   // ordning[klart[spelarens nummer]] = spelarens nummer =>
   // spelarens position = ordningen[spelarens nummer]
   for(int i=0;i<s;i++)
      ordning[klart[i]]=i;
   // Returnerar vinnaren
   return ordning[0];
}
