#include <stdlib.h>
#include <string.h>
#include "grafik.h"
#define ANTAL_KORT 52
#ifndef _KORTH
#define _KORTH
enum valorer { nothing=-1,tva=0,tre,fyra,fem,sex,sju,atta,nio,tio,knekt,dam,kung,ess};

class Kort {
   public:
      // -- Default konstruktor -------------------------------
      Kort(void) {}

      // -- Konstruktor ---------------------------------------
      // Lägger till färgen och valören
      // ------------------------------------------------------
      Kort(int f, int v) : farg(f), valor(v) {}

      // -- getfarg() -----------------------------------------
      // Returnerar: Färgen för kortet i int
      // ------------------------------------------------------
      int getfarg(void) { return (int)farg; };

      // -- getvalor() ----------------------------------------
      // Returnerar: Valören för kortet i int
      // ------------------------------------------------------
      int getvalor(void) { return (int)valor; };

      // -- getchfarg() ---------------------------------------
      // Returnerar: Färgen för kortet i text(char *)
      // ------------------------------------------------------
      char *getchfarg(void);

      // -- getchvalor() --------------------------------------
      // Returnerar: Valören för kortet i text(char *)
      // ------------------------------------------------------
      char *getchvalor(void);

      // -- setfarg( int ) ------------------------------------
      // Sätter färgen på kortet
      // Indata: Färgens värde i int
      // ------------------------------------------------------
      void setfarg(int f) { farg = f; };

      // -- setvalor( int ) -----------------------------------
      // Sätter valören på kortet
      // Indata: Valörens värde i int
      // ------------------------------------------------------
      void setvalor(int v) { valor = v; };
      // Färger
      enum farger { klover=0, ruter, spader, hjarter };
   private:
      // Färger klöver  = 0
      //        ruter   = 1
      //        spader  = 2
      //        hjärter = 3
      int farg;
      // Värden på kort två = 0 ... ess = 13
      int valor;
};

class Kortlek {
   public:
      Kortlek(void) { init(); }
      // Intisiera kortlek
      int init(void);
      // Hämta ett kort från kortleken
      Kort takort(void);
      // Ge ett kort till kortleken
      void gekort(Kort);
      // Blandar kortlek
      int blanda_kortlek(void);
      // Hämtar antal kort som finns kvar i leken
      int getantal_kort(void) { return antal_i_lek; }
      // Hämta standard utskrift char * dvs "siffra""symbol" tex 2(hjärt symbol)
      char *getkort(int n);
   private:
      // Alla valörer och färger i kortleken
      Kort allakort[ANTAL_KORT];
      // Kort kvar i leken
      int antal_i_lek;
      // Kort som finns i leken index till allakort
      int kort_i_lek[ANTAL_KORT];
};
#endif



