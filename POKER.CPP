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
      // Om val�ren p� kortet �r nothing s� skall ett h�mtas
      if ( spelare[i].getvalor()==nothing) {
         // H�mtar kort fr�n kortleken
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

   // Array med f�rgerna
   int farg[5];

   // H�mtar f�rgerna till array
   for ( int i=0; i<5; i++)
      farg[i] = kontroll[i].getfarg();

   // Kollar om kortens f�rger st�mmer element(n)'s f�rg = element(n-1)'s f�rg
   for ( int i = 1; i < 5; i++ )
     // Inte samma return false
     if ( farg[i]!=farg[i-1] )
        return false;

   // Samma f�rg
   return true;
}

void Poker::sortera( int valorer[5] ) {
   // Inte sorterad
   bool sorted=false;
   // Loopar till dess att alla v�rden ligger p� r�tt plats i ordning 0..13
   while ( !sorted ) {
      // Sorterad ?
      sorted = true;
      // Kollar element(n) med element(n+11)
      // element(n+1) ska vara st�rre
      for ( int i=0;i<4;i++)
         if ( valorer[i]>valorer[i+1] ) {
            // Om det inte �r s� byter val�rerna plats
            int a = valorer[i];
            valorer[i] = valorer[i+1];
            valorer[i+1] = a;
            // inte sorterat
            sorted = false;
         }
   }
}

int Poker::straight( Kort kontroll[5]  ) {

   // V�rden i array
   int valor[5];

   // H�mtar v�rden till array
   for ( int i = 0; i <5; i++)
      valor[i] = kontroll[i].getvalor();

   // Sorterar v�rdena stigande
   sortera(valor);

   // Kontrollerar om det �r ess 2 3 4 5   OBS specialfall
   if ( valor[4] == 12 && valor[0] == 0 &&
        valor[1] == 1  && valor[2] == 2 &&
        valor[3] == 3 ) return 4;

   // Kollar om element(n)'s v�rde = element(n-1)'s v�rde +1
   // st�mmer inte det s� returneras 0
   for ( int i=1; i<5; i++ ) {
      if ( valor[i]!=valor[i-1]+1 ) return false;
   }

   // Returnerar 9 om det �r top
   if ( valor[0] == 9 ) return 9;
   // Vanlig stege
   else return 4;
}

void Poker::antalvalor( int antl[14], Kort kontroll[5] ) {
   int valor[5];  // Kortens v�rden

   // H�mtar v�rden
   for ( int i=0; i<5; i++ )
      valor[i] = kontroll[i].getvalor();
   // Nollst�ller array med v�rden f�r varje v�rde p� kort
   for ( int i=0; i<14; i++ )
      antl[i] = 0;
   // G�r igenom kortens v�rden och �kar v�rde arrayen d�r kort finns
   for ( int i = 0; i<5; i++ )
      antl[valor[i]]++;
}

int Poker::spelares_hand( Kort kontroll[5] ) {

   // Nollst�ller array med v�rden f�r varje v�rde p� kort */
   int antalpervalor[14];

   // Nollst�ller array med antal par tretal och fyrtal
   // Position 0 -> ental, 1 -> par, 2 -> treta�, 3 -> fyrtal
   int antalen[4] = { 0, 0, 0, 0 };

   // G�r igenom kortens v�rden och �kar v�rde arrayen d�r kort finns
   antalvalor(antalpervalor,kontroll);

   // L�gger till ental, par, tv� par, tretal samt fyrtal
   for ( int i=0; i<14 ; i++)
      if ( antalpervalor[i] )
         antalen[antalpervalor[i]-1]++;

   // Kollar om det finns n�got par
   if ( antalen[1] )
      // finns det tretal �r det k�k
      if ( antalen[2] ) return 6;
      // finns det mer en ett par �r det tv� par
      else if ( antalen[1]>1 ) return 2;
      // Annars �r det ett par
         else return 1;

   // �r det tretal
   if ( antalen[2] ) return 3;
   // �r det det fyrtal
   if ( antalen[3] ) return 7;

   // H�mtar flush och straignt
   int fl= flush(kontroll), st=straight(kontroll);
   // �r det flush och straight eller tom top straight flush
   if ( fl && st ) if ( st == 9 ) return 9;
      else return 8;

   // �r det bara straight
   if ( st ) return 4;
   // �r det flush
   if ( fl ) return 5;

   // �r det inget
   return 0;
}

int Poker::jamfor( Kort spelare1[5], Kort spelare2[5] ) {
   // H�mtar po�ng f�r spelarnas h�nder
   int p = spelares_hand(spelare1);
   int p2 = spelares_hand(spelare2);

   if ( p == p2 ) {

      int antalpervalor[14];
      int antalpervalor2[14];
      // H�mtar antal f�rh�llande till v�rden
      antalvalor(antalpervalor,spelare1);
      antalvalor(antalpervalor2,spelare2);

      // Kollar 4tal 3tal 2par och 1par och ental en efter en
      for (int n = 3; n>0; n--)
      // Kollar h�gsta v�rdet 13...0
      for ( int i=13; i>=0;i-- ) {
         // Om n st kort med v�rdet existerar hos endast en s� returnera vinnaren
         if ( antalpervalor[i]==n && antalpervalor2[i]<n ) return 1;
         if ( antalpervalor2[i]==n && antalpervalor[i]<n ) return 2;
      }
   } else {
      // returnerar den som har mest po�ng
      if ( p>p2 ) return 1; else return 2;
   }
   // Spelarna har samma po�ng och v�rden p� handen
   return 0;
}

int Poker::vinnare( Kort spelare[5][5], int s, int ordning[5] ) {
   // S�tter alla spelare till s-1 dvs den som inte vinner n�gon match f�r s-1 och den som vinner har 0
   int klart[5] = { s-1, s-1, s-1, s-1, s-1 };
   for ( int i = 0; i < s ; i++ )
      for ( int t = 0; t < s; t++ )
        // Vinner spelaren i minskar klart[i] men blir aldrig 0 f�r man man bara vinna s-1 g�nger
        if ( jamfor(spelare[i],spelare[t]) == 1 ) klart[i]--;
   // Ordnar s� att allt kommer r�tt
   // ordning[klart[spelarens nummer]] = spelarens nummer =>
   // spelarens position = ordningen[spelarens nummer]
   for(int i=0;i<s;i++)
      ordning[klart[i]]=i;
   // Returnerar vinnaren
   return ordning[0];
}
