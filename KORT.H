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
      // L�gger till f�rgen och val�ren
      // ------------------------------------------------------
      Kort(int f, int v) : farg(f), valor(v) {}

      // -- getfarg() -----------------------------------------
      // Returnerar: F�rgen f�r kortet i int
      // ------------------------------------------------------
      int getfarg(void) { return (int)farg; };

      // -- getvalor() ----------------------------------------
      // Returnerar: Val�ren f�r kortet i int
      // ------------------------------------------------------
      int getvalor(void) { return (int)valor; };

      // -- getchfarg() ---------------------------------------
      // Returnerar: F�rgen f�r kortet i text(char *)
      // ------------------------------------------------------
      char *getchfarg(void);

      // -- getchvalor() --------------------------------------
      // Returnerar: Val�ren f�r kortet i text(char *)
      // ------------------------------------------------------
      char *getchvalor(void);

      // -- setfarg( int ) ------------------------------------
      // S�tter f�rgen p� kortet
      // Indata: F�rgens v�rde i int
      // ------------------------------------------------------
      void setfarg(int f) { farg = f; };

      // -- setvalor( int ) -----------------------------------
      // S�tter val�ren p� kortet
      // Indata: Val�rens v�rde i int
      // ------------------------------------------------------
      void setvalor(int v) { valor = v; };
      // F�rger
      enum farger { klover=0, ruter, spader, hjarter };
   private:
      // F�rger kl�ver  = 0
      //        ruter   = 1
      //        spader  = 2
      //        hj�rter = 3
      int farg;
      // V�rden p� kort tv� = 0 ... ess = 13
      int valor;
};

class Kortlek {
   public:
      Kortlek(void) { init(); }
      // Intisiera kortlek
      int init(void);
      // H�mta ett kort fr�n kortleken
      Kort takort(void);
      // Ge ett kort till kortleken
      void gekort(Kort);
      // Blandar kortlek
      int blanda_kortlek(void);
      // H�mtar antal kort som finns kvar i leken
      int getantal_kort(void) { return antal_i_lek; }
      // H�mta standard utskrift char * dvs "siffra""symbol" tex 2(hj�rt symbol)
      char *getkort(int n);
   private:
      // Alla val�rer och f�rger i kortleken
      Kort allakort[ANTAL_KORT];
      // Kort kvar i leken
      int antal_i_lek;
      // Kort som finns i leken index till allakort
      int kort_i_lek[ANTAL_KORT];
};
#endif



