/* Filename: netpoker.cpp
 * This file contains the fundamental window processing
 * it controls the display.
 * Comments in swedish so far.
 * Created by: Markus Svensson 2001-04-xx
 * Changed by: Markus Svensson 2002-03-28
 */

#include <windows.h>
#include "cards.h"
#include "spel.h"
#include "net.h"
#include "grafik.h"
#include "netpoker.h"
#include "text.h"

HWND bhwnd;                // Knapp f�r sl�ng kort
HWND bmhwnd;               // Knapp f�r skicka meddelande
HWND ehwnd;                // edit box f�r meddelanden
HWND nhwnd;                // Knapp nytt spel
HWND bnhwnd;					// Knapp f�r att visa n�sta spelares kort
HWND mainhwnd;             // Huvudf�nster
HINSTANCE hinst;           // Program instance
Game *spel;                // Spelet
bool connaccept = false;   // Connection accepted
int typ_av_spel=-1;        // Server eller Client mode PO_SERVER PO_SERVER
int anslutna=0;            // Antal ansluta
SOCKET s[4];               // Anslutna sockets
SOCKET s_an;               // Ansluten socket
SOCKET s_eg;               // Egen socket
char skrivmed[5][100] = { "","","","","" };  // Meddelande buffer
char ipadresser[4][20];                      // Ip addresser anv�nds av server
SOCKADDR_IN sockAddr[4];                     // Socket addresser anv�nds avserver
Kort kort[5];                                // Synliga kort
int baksida[5] = { 0, 0, 0, 0, 0 };          // Baksidan p� kortet som visas 1 �r baksida 0 inte basida
char Namn[5][20] = { "","","","","" };       // Namn p� spelarna finns endast p� server
                                             // Ens eget namn lagrat i index 0 f�r client 4 f�r server
Grafik grafik;
int antal_slang[4];                          // Antalkort som skall sl�ngas
int spelare_som_slangt=0;                    // Antal spelare som sl�ngt sina kort
bool spelar = false;                         // Spelet �r i g�ng
bool slangakort=true;                        // Korten �r sl�ngda eller ej
char somslangt[4][20]={ "", "", "", ""  };   // Namn p� dem som sl�ngt kort
Vinnarstruct vinnare;								// Vinnarstuct med info om hur spelet gick
Vinnarstruct *vinn;									// Pekare till vinnarstruct
bool visa_vinnare = false;							// visa vem som vann
bool nytt_spel = true;
int omgang = 1;										// Spel omg�ng
int vald = 0;

int addmed(char text[50]);
BOOL FAR StartDialogProc ( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

#include "startdialog.cpp"
// L�gger till ett meddelande i meddelande buffer
int addmed(char text[50]) {
   for ( int i=4;i>0;i-- )
      strcpy(skrivmed[i],skrivmed[i-1]);
   strcpy(skrivmed[0],text);
   RECT r;
   GetClientRect(mainhwnd,&r);
   r.top = 400;
   InvalidateRect(mainhwnd,&r,FALSE);
   return 0;
}

// Windowoproc har hand om utskrift f�r sk�rm h�ndelser sam n�t
LRESULT CALLBACK WindowProc ( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
   int i;
   HDC hdc;
   switch ( msg ) {
      case WM_CREATE:
         // Knapp f�r att sl�nga kort
         bhwnd  = CreateWindow("Button",/*"N�jd"*/ST_SATISFIED,WS_CHILDWINDOW | BS_PUSHBUTTON,350,150,100,30,hwnd,(HMENU)IDI_NASTA,hinst,NULL);
         // Edit f�lt f�r att skriva meddelanden
         ehwnd  = CreateWindow("Edit","", WS_BORDER	| ES_AUTOHSCROLL | ES_WANTRETURN | WS_TABSTOP | WS_VISIBLE | WS_CHILDWINDOW ,10,400,200,25,hwnd,NULL,hinst,NULL);
         // Knapp f�r att skicka meddelanden
         bmhwnd = CreateWindow("Button",/*"Skicka meddelande"*/ST_SENDMSG,WS_TABSTOP | WS_VISIBLE | WS_CHILDWINDOW | BS_DEFPUSHBUTTON,230,402,150,25,hwnd,(HMENU)IDI_SKICKA_MED,hinst,NULL);
         // Knapp n�sta
         nhwnd  = CreateWindow("Button",/*"Nytt spel"*/ST_NEWGAME,WS_TABSTOP | WS_VISIBLE | WS_CHILDWINDOW | BS_DEFPUSHBUTTON,430,402,150,25,hwnd,(HMENU)IDI_NYTT,hinst,NULL);
         bnhwnd  = CreateWindow("Button",/*"N�sta spelare"*/ST_NEXTPLAYER,WS_TABSTOP | WS_CHILDWINDOW | BS_DEFPUSHBUTTON,360,22,100,15,hwnd,(HMENU)IDI_NASTA_KORT,hinst,NULL);
         ShowWindow(nhwnd,SW_HIDE);
         break;

      case WM_QUIT:
         PostQuitMessage(0);
         break;
      case WM_DESTROY:
         PostQuitMessage(0);
         break;
      // Uppdatera bakgrund
      case WM_PAINT:
         PAINTSTRUCT ps;
         POINT lp;
         hdc = BeginPaint(hwnd,&ps);

         RECT trect;
         // H�mtar kordinater f�r f�nster och fyller med gr�n
         GetClientRect(hwnd,&trect);
         FillRect(hdc,&trect,CreateSolidBrush(0x017F01));
         // Fyller meddelande del med annan gr�n f�rg
         trect.left = 0;
         trect.top  = 440;
         FillRect(hdc,&trect,CreateSolidBrush(0x4E804E));

         // Skriver ut de 5 korten
         for ( i=0;i<5;i++)
         {
            // Punkter f�r kortet
            lp.x = 20+i*90;  // Kortet plats m�ste anges
            lp.y = 50;
            Rectangle(hdc,20+i*90-1,47,20+i*90+76,142);
            // Om man inte spelar ritas ett kort v�nt p� baksidan ut
            // annars ritas kortet ut p� r�tt sida
            if ( spelar )
               grafik.ritakort(hwnd,hinst,lp,baksida[i],kort[i].getfarg(),kort[i].getvalor());
            else
               grafik.ritakort(hwnd,hinst,lp,1,kort[i].getfarg(),kort[i].getvalor());
            if ( spelar && visa_vinnare ) {
               grafik.ritakort(hwnd,hinst,lp,0,kort[i].getfarg(),kort[i].getvalor());
               lp.y = 260;
               Rectangle(hdc,20+i*90-1,lp.y-3,20+i*90+76,lp.y+91);
               grafik.ritakort(hwnd,hinst,lp,0,vinn->kort[i].getfarg(),vinn->kort[i].getvalor());
               lp.y = 50;
               for ( int o = 0; o < vinn->antal; o ++ ) {
                  Rectangle(hdc,lp.x-1,lp.y-3,lp.x+76,lp.y+91);
                  grafik.ritakort(hwnd,hinst,lp,0,vinn->allakort[o][i].getfarg(),vinn->allakort[o][i].getvalor());
                  lp.y += 20;
                  lp.x += 10;
               }
            }
         }
         for ( i = 0 ; i < 5 && visa_vinnare; i ++ ) {
            lp.y = 50+20*vald;
            lp.x = 20+i*90+10*vald;
            Rectangle(hdc,lp.x-1,lp.y-3,lp.x+76,lp.y+91);
            grafik.ritakort(hwnd,hinst,lp,0,vinn->allakort[vald][i].getfarg(),vinn->allakort[vald][i].getvalor());
         }

         SetBkColor(hdc,0x017F01);
         if ( visa_vinnare ) {
            char text[150];
            strcpy(text,vinn->namn[vald]);
            if ( vinn->namn[vald][strlen(vinn->namn[vald])-1] == 's' )
               strcat(text,/*" kort, "*/ST_CARDS);
            else
               strcat(text,/*"s kort, "*/ST_CARDS);
            strcat(text,vinn->hand[vald]);
            strcat(text,":");
            TextOut(hdc,20,30,text,strlen(text));
            strcpy(text,/*"Kort hos vinnaren, "*/ST_WINNERCARDS);
            strcat(text,vinn->namn[0]);
            strcat(text,":");
            TextOut(hdc,20,220,text,strlen(text));
         }

         TextOut(hdc,500,30,/*"Spelare som"*/ST_PLAYERSTHROW1,strlen(ST_PLAYERSTHROW1));
         TextOut(hdc,500,44,/*"sl�ngt kort:"*/ST_PLAYERSTHROW2,strlen(ST_PLAYERSTHROW2));
         for ( i=0;i<4;i++ )
            TextOut(hdc,500,60+i*16,somslangt[i],strlen(somslangt[i]));

         // Skriver ut antalet anslutna spelare f�r servern  �NDRA DET SEN
         if ( typ_av_spel == PO_SERVER  ) {
            char text[100];
            strcpy(text,/*"Anslutna spelare: "*/ST_CONNECTEDPLAYERS);
            char tal[5];
            itoa(anslutna,tal,10);
            strcat(text,tal);
            TextOut(hdc,1,1,text,strlen(text));
         }
         SetBkColor(hdc,0x4E804E);

         // Skriver ut meddelanden fr�n meddelande buffer
         for ( i=0;i<5;i++ )
            TextOut(hdc,5,450+i*16,skrivmed[i],strlen(skrivmed[i]));
         if ( visa_vinnare && typ_av_spel == PO_SERVER)
            ShowWindow(nhwnd,SW_SHOW);
         EndPaint(hwnd,&ps);
         break;

      case WM_TIMER:
         // Timer f�r anslutning om ingen anslutning p� 10 sek s� avslutas programmet
         if ( !connaccept ) {
            addmed(/*"Spelet fullt eller existerar ej och programmet avslutas"*/ST_ERR_GAMEFULL1);
            MessageBox(hwnd,/*"Spel ej funnet programmet avslutas"*/ST_ERR_GAMEFULL2,/*"Inget spel funnet"*/ST_ERR_GAMEFULL3,NULL);
            PostQuitMessage(0);
         }
         KillTimer(hwnd,wparam);
         break;

      case WM_SETFOCUS: case WM_SIZE: case WM_MOVE: case WM_KILLFOCUS:
         RedrawWindow(hwnd,NULL,NULL,RDW_ERASE|RDW_INVALIDATE);
         break;

      case WM_LBUTTONDOWN:
         int x,y;
         x=LOWORD(lparam);
         y=HIWORD(lparam);
         // Om man spelar och inte sl�ngt korten kan man v�nda korten
         if ( spelar && !visa_vinnare)
         for ( i=0;i<5;i++)
         {
            // Kollar om koordinaterna och ser om man trycker p� ett kort
            if ( x>20+i*90 && x < 20+i*90+75 && y > 50 && y < 140 ) {
               // F�r man inte sl�nga kort skriv meddelande annars v�nd kort
               if ( !slangakort )
                  addmed(/*"Man kan inte sl�nga kort �n v�nta p� dina nya kort"*/ST_ERR_NOTTHROW);
               else {
                  baksida[i] = !baksida[i];
                  RedrawWindow(hwnd,NULL,NULL,RDW_ERASE|RDW_INVALIDATE);
               }
               break;
            }
         }
         if ( baksida[0] || baksida[1] || baksida[2] || baksida[3] || baksida[4] )
            SetWindowText(bhwnd,/*"Sl�ng kort"*/ST_THROWCARDS);
         else
            SetWindowText(bhwnd,/*"N�jd"*/ST_SATISFIED);
         break;

      case WM_COMMAND:
         switch(LOWORD(wparam)) {
            // trycker p� knappen f�r att sl�nga
            case IDI_NASTA:
               // f�r man sl�nga kort
               if ( slangakort ) {
                  // �r man klient skickas dom till server och man f�r inte sl�nga kort
                  if ( typ_av_spel == PO_CLIENT ) {
                     sendbaksida(s_eg,baksida);
                     SetWindowText(bhwnd,/*"V�nta"*/ST_WAIT);
                     EnableWindow(bhwnd,FALSE);
                     slangakort=false;
                  }
                  // �r man server
                  if ( typ_av_spel == PO_SERVER && anslutna > 0) {
                     // Hur m�nga kort �r sl�ngda och de sl�ngs
                     antal_slang[anslutna] = spel->slangkort(anslutna,baksida);

                     for ( int u = 0; u < anslutna; u++ )
                        sendnamn(s[u],Namn[anslutna]);
                     int u;
                     u=0;
                     while ( somslangt[u][0]!='\0' ) u++;
                     strncpy(somslangt[u],Namn[anslutna],20);
                     SetWindowText(bhwnd,/*"V�nta"*/ST_WAIT);
                     EnableWindow(bhwnd,FALSE);
                     // Kollar om alla sl�ngt om ej s� skrivs ett v�nta meddelande annars
                     // ska alla kort skickas ut
                     if ( spelare_som_slangt == anslutna )
                        SendMessage(hwnd,WM_GEKORT,0,0);
                     else {
                        // V�nta meddelande
                        addmed(/*"V�nta till alla sl�ngt sina kort d� f�r du nya!"*/ST_WAITFORCARDS);
                        slangakort = false;
                        spelare_som_slangt++;
                     }
                  }
                  if ( typ_av_spel == PO_SERVER && anslutna == 0 )
                     addmed(/*"V�nta tills alla spelare anslutit sig"*/ST_WAITFORPLAYERS);
               } else
                  addmed(/*"Du f�r v�nta till alla spelare sl�ngt sina kort"*/ST_WAITFORCARDS);

               break;
            // Skicka ett meddelande
            case IDI_SKICKA_MED:
               // Kollar om det �r ett meddelande
               char m[10];
               GetWindowText(ehwnd,m,10);
               if ( m[0] != '\0' ) {
                  // Klienten skickar det till servern
                  if ( typ_av_spel == PO_CLIENT ) {
                     char med[100];
                     GetWindowText(ehwnd,med,100);
                     sendmed(s_eg,med);
                     SetWindowText(ehwnd,"");
                  // Servern skriver ut det sj�lv och skickar det till alla klienter
                  } else if ( typ_av_spel == PO_SERVER ) {
                     char med[100];
                     char medtillserver[100];
                     strcpy(medtillserver,Namn[4]);
                     strcat(medtillserver," > ");
                     GetWindowText(ehwnd,med,100);
                     strcat(medtillserver,med);
                     for ( int t=0;t<anslutna;t++)
                        sendmed(s[t],medtillserver);
                     addmed(medtillserver);
                     SetWindowText(ehwnd,"");
                  }
               }
               break;

            case IDI_NYTT:
               // Nytt spel endast servern kan �ndra detta
               if ( anslutna ) {
             		if ( nytt_spel && typ_av_spel == PO_SERVER  && ( !spelar || visa_vinnare ) ) {
               		visa_vinnare = false;
	                  ShowWindow(bhwnd,SW_SHOW);
   	               for ( int i=0;i<anslutna;i++) {
      	            	sendaccept(s[i]);
         	            sendmed(s[i],/*"Nu b�rjar ett nytt spel. Nya kort till alla"*/ST_NEWGAMESTARTED);
            	      }
               	   delete spel;
	                  spel = new Game(anslutna+1);
   	               strcpy( Namn[spel->antalspelare()-1],Namn[4]);
      	            for ( int i = 0; i < anslutna;i++) {
         	         	Kort tmpkort[5];
            	         spel->kortforspelare(i,tmpkort);
               	      sendkort(s[i],tmpkort);
	                  }
   	               spel->kortforspelare(anslutna,kort);
      	            omgang = 1;
         	         spelare_som_slangt = 0;
            	      ShowWindow(nhwnd,SW_HIDE);
               	   addmed(/*"Nu b�rjar ett nytt spel"*/ST_NEWGAMESTARTED);
	                  spelar = true;
   	               nytt_spel = false;
						}
         	      if ( visa_vinnare && typ_av_spel == PO_SERVER ) {
            	      addmed(/*"V�ntar p� anslutande spelare."*/ST_WAITFORCONNECTIONS);
               	   addmed(/*"Tryck p� Nytt spel, f�r att spela"*/ST_INFONEWGAME);
	                  ShowWindow(nhwnd,SW_HIDE);
   	               nytt_spel = true;
      	         }
         		}
               break;
            case IDI_NASTA_KORT:
               vald = vald+1;
               vald = vald%vinn->antal;
               RECT r;
               r.top = 20;
               r.left = 0;
               r.right = 500;
               r.bottom = 200;
               InvalidateRect(hwnd,&r,FALSE);
               break;
            case MN_INFO:
   				CreateDialog(hinst,"INFODIALOG",mainhwnd,(DLGPROC)StartDialogProc);
               break;
            case MN_NYTT:
               if ( typ_av_spel == PO_SERVER && anslutna) {
                  visa_vinnare = true;
                  SendMessage(hwnd,WM_COMMAND,IDI_NYTT,0);
               }
               break;
            case MN_AVSLUTA:
               PostQuitMessage(0);
               break;
			}


         break;

      // Ger kort till alla som sl�ngt kort
      // Endast servern kan anropa detta meddelandet
      case WM_GEKORT:

         int ok;
         char med[100];
         char tal[5];
         itoa(omgang,tal,10);
         char text[20];
         strcpy(text,"Omg�ng ");
         strcat(text,tal);
         ok = MessageBox(hwnd,/*"Vill du forts�tta att spela!"*/ST_CONTTOPLAY,text,MB_YESNO|MB_DEFBUTTON1|MB_SYSTEMMODAL);
         // H�mtar nya kort till alla �ven server
         for ( i = 0; i <= anslutna; i++ ) {
            // H�mtar kort till anv�ndare i
            spel->hemtakort(i, antal_slang[i]);
            Kort tmpkort[5];
            spel->kortforspelare(i,tmpkort);
            int o;
            // Om man �r server skriv korten dirket till kort variabel
            if ( i == anslutna )
               for ( o = 0; o < 5 ; o++ )
                  kort[o] = tmpkort[o];
            else
               // Annars skickas korten till Klient
               sendkort(s[i],tmpkort);
            // Man skapar ett meddelande om att anv�ndare sl�ngt x kort
            char antal[2];
            itoa(antal_slang[i],antal,10);
            strcpy(med, /*"Spelare "*/ST_PLAYER);
            strcat(med, Namn[i] );
            strcat(med, /*" sl�ngde "*/ ST_THREW);
            strcat(med, antal );
            strcat(med, /*" kort."*/ST_CARDSDOT);
            int u = 0;
            for ( u = 0; u < anslutna; u++)
               sendmed(s[u],med);
            // Skriver ut f�r server med
            addmed(med);
         }
         omgang ++ ;
         EnableWindow(bhwnd,TRUE);
         SetWindowText(bhwnd,/*"N�jd"*/ST_SATISFIED);
         ShowWindow(bhwnd,SW_SHOW);
         if ( ok == IDNO ) {
            int vinnint;
            int ord[5];
            Kort vinnkort[5];
            spel->spelvinnare(vinnint,vinnkort,ord);
            // Kopierar den vinnande handen
            for ( int u = 0; u < 5; u++ )
               vinnare.kort[u]=vinnkort[u];
            // Kopierar allas h�nder av kort f�r att redovisa det senare
            for ( int u = 0; u < spel->antalspelare(); u++ ) {
               strcpy(vinnare.hand[u],spel->handforspelare(ord[u]));
               strcpy(vinnare.namn[u],Namn[ord[u]]);
               Kort *tmpkort = new Kort[5];
               tmpkort = vinnare.allakort[u];
               spel->kortforspelare(ord[u], tmpkort);
            }
            vinnare.antal = spel->antalspelare();

            for ( int u = 0; u < anslutna; u ++ )
               sendvinnare(s[u],vinnare);
            visa_vinnare = true;
            ShowWindow(bhwnd,SW_HIDE);
            ShowWindow(bnhwnd,SW_SHOW);

         }
         vinn = &vinnare;

         int u;
         // Inget kort �r v�nt p� baksida
         for ( u = 0 ; u< 5 ; u ++ ) {
            baksida[u] = 0;
            if ( u < 4 ) strcpy(somslangt[u],"\0");
         }
         RedrawWindow(hwnd,NULL,NULL,RDW_ERASE|RDW_INVALIDATE);
         // Nu har ingen sl�ngt kort
         spelare_som_slangt = 0;
         // Och severn f�r sl�nga kort
         slangakort = true;
         break;

      // SERVER F�r n�tet  --> till separat fil
      case WM_SOCKET_SELECT_SERVER:
         switch(LOWORD(lparam)) {
            // Accpetera en anslutning fr�n klient ?
            case FD_ACCEPT:

               // Finns det plats f�r klient
               if ( anslutna < 3 && !spelar ) {

                  WSAAsyncSelect( s_eg,
                            hwnd,
                            WM_SOCKET_SELECT_SERVER,
                            FD_READ | FD_WRITE | FD_CLOSE );
                  int length;
                  length=sizeof(sockAddr);
                  // H�mtar socket f�r klient
                  s[anslutna]=accept(s_eg,(struct sockaddr *)&sockAddr,&length);
                  // f�r en ny anslutnings m�jlighet f�r servern
                  // kopierar ipadress �r det n�dv�ndigt ?

                  // Skicka ett accept meddelande till klient
                  sendaccept(s[anslutna]);
                  addmed(/*"Skickar accept"*/ST_SENDACCEPT);
                  // En ansluten till
                  anslutna++;
						int reuse=true;

   //---------------
   while (listen(s_eg,5) ) {
      char felnummer[50];
      strcpy(felnummer,SockerrToString(WSAGetLastError()));
      MessageBox(NULL,"Fel",felnummer,NULL);
   }
   setsockopt(s_eg,SOL_SOCKET,SO_ACCEPTCONN,(char *)&reuse,sizeof(int));
   // best�mer vilka h�ndelser som skall tas om hand om
   while( WSAAsyncSelect( s_eg,
                        mainhwnd,
                        WM_SOCKET_SELECT_SERVER,
                        FD_CONNECT|
                        FD_WRITE|
                        FD_READ|
                        FD_ACCEPT|
                        FD_CLOSE ) != 0 ) {
      char felnummer[50];
      strcpy(felnummer,SockerrToString(WSAGetLastError()));
      MessageBox(NULL,"Fel",felnummer,NULL);
   }
   //---------------


                  if ( anslutna == 3 )
                     SendMessage(hwnd,WM_COMMAND,IDI_NYTT,NULL);
                  ShowWindow(nhwnd,SW_SHOW);
                  // Om alla m�jliga anslutningar �r genomf�rda s� skickas ett ok meddelande fr�n server
                  sendmed(s[anslutna-1],/*"V�ntar p� att spelare skall ansluta..."*/ST_INFOWAITFORPLAYERS);
               }
               break;

            // L�s data fr�n en klient
            case FD_READ:
               char meddelande[100];
               // G�r igenom alla anslutna sockets
               for ( int i=0;i<anslutna;i++) {
                  // H�mtar meddelnade
                  strcpy(meddelande,"");
                  char ip[20];
                  int typ = recive(s[i],NULL,meddelande,ip);
                  // �r det ett skriftilig meddelande
                  if ( typ == PO_MEDDELANDE ) {
                     // Skicka vidare det och s�g vem det kommer i fr�n
                     char med[101];
                     strcpy(med,Namn[i]);
                     char mmed[101];
                     strcpy(mmed,"M");
                     strcat(med," > ");
                     strcat(med,meddelande);
                     strcat(mmed,med);
                     for ( int t=0;t<anslutna;t++)
                        sendmed(s[t],med);
                     addmed(med);
                  }
                  // �r det ett namn p� en ansluten klient
                  if ( typ == PO_NAMN ) {
                     // H�mtar namnet till namn listan
                     strcpy(Namn[i],meddelande);
                     char med[100];
                     // Skapar meddelande till andra anslutna
                     strcpy(med,/*"Anv�ndare "*/ST_PLAYER);
                     strcat(med,Namn[i]);
                     strcat(med,/*" �r ansluten"*/ST_ISCONNECTED);
                     for ( int u=0; u <= anslutna;u++ )
                        sendmed(s[u],med);
                     addmed(med);
                  }
                  // �r det vilka som �r baksidor p� korten
                  if ( typ == PO_BAKSIDA ) {
                     int *bak;
                     bak = new int[5];
                     // Dom ligger i meddelande buffer
                     bak = (int *)meddelande;
                     // Sl�nger kort och h�mtar hur m�nga som sl�ngs
                     antal_slang[i] = spel->slangkort(i,bak);

                     for ( int u = 0; u < anslutna; u++ )
                        sendnamn(s[u],Namn[i]);
                     u = 0;
                     while ( somslangt[u][0]!='\0') u++;
                     strncpy(somslangt[u],Namn[i],20);

                     // Om alla sl�ngt s� skicka meddelande om att h�mta kort
                     if ( spelare_som_slangt == anslutna )
                        SendMessage(hwnd,WM_GEKORT,0,0);
                     else {
                        // Annars f�r man v�nta p� nya kort
                        sendmed(s[i],/*"V�nta till alla sl�ngt sina kort d� f�r du nya!"*/ST_WAITFORCARDS);
                        spelare_som_slangt++;
                     }
                     RedrawWindow(hwnd,NULL,NULL,RDW_ERASE|RDW_INVALIDATE);
                  }
                  if ( typ == PO_NOTACCEPT ) {
                     char med[100];
                     strcpy(med, /*"Spelare "*/ST_PLAYER);
                     strcat(med,Namn[i]);
                     strcat(med,/*" kopplade ifr�n sig"*/ST_DISCONNECT);
                     addmed(med);
                     anslutna --;
                     for (int y =0; y < anslutna-1; y++)
                       s[y]=(y>=i)?s[y+1]:s[y];
                  }
               }
               break;
         }
         break;
      // CLIENT
      case WM_SOCKET_SELECT_CLIENT:
         switch(LOWORD(lparam)) {
            case FD_CONNECT:
               // S�ger att man �r ansluten
               addmed(/*"V�ntar p� att servern accepterar."*/ST_ACCEPTED);
               // S�tter timer f�r att kolla om anslutningen h�ller
               SetTimer(hwnd,1,10000,(TIMERPROC)WindowProc);
               break;
            case FD_READ:
               char *meddelande;
               meddelande = new char[500];
               char ip[20];
               int typ;
               // H�mtar meddelande fr�n servern
               typ = recive(s_eg,kort,meddelande,ip);
               // �r det ett text meddelande skrivs det ut
               if ( typ == PO_MEDDELANDE )
                  addmed(meddelande);

               // Ett accpetering meddelande, man f�r vara med och spela
               if ( typ == PO_ACCEPT ) {
                  connaccept = true;
                  visa_vinnare = false;
                  // Skickar klientens namn till servern                  
                  sendnamn(s_eg,Namn[0]);
                  ShowWindow(bhwnd,SW_SHOW);
               }

               //KORT
               // F�r kort fr�n servern
               if ( typ == PO_KORT ) {
                  // Man spelar och f�r sl�nga kort
                  spelar = true;
                  slangakort = true;
                  int u;
                  // Visar inga baksidor
                  for ( u = 0 ; u < 5 ; u ++ ) {
                     baksida[u] = 0;
                     if ( u < 5 ) strcpy(somslangt[u],"\0");
                  }
                  ShowWindow(bhwnd,SW_SHOW);
                  EnableWindow(bhwnd,TRUE);
                  SetWindowText(bhwnd,/*"N�jd"*/ST_SATISFIED);

                  RedrawWindow(hwnd,NULL,NULL,RDW_ERASE|RDW_INVALIDATE);
               }

               // Namn p� dem som sl�ngt
               if ( typ == PO_NAMN ) {
                  int u;
                  u = 0;
                  while ( somslangt[u][0]!='\0' ) u++;
                  strncpy(somslangt[u],meddelande,20);
                  RedrawWindow(hwnd,NULL,NULL,RDW_ERASE|RDW_INVALIDATE);
               }

               if ( typ == PO_VINNARE ) {
                  vinn = new Vinnarstruct[1];
                  vinn = (Vinnarstruct *)meddelande;
                  visa_vinnare = true;
                  RedrawWindow(hwnd,NULL,NULL,RDW_ERASE|RDW_INVALIDATE);
                  ShowWindow(bhwnd,SW_HIDE);
                  ShowWindow(bnhwnd,SW_SHOW);
   					addmed(/*"V�nta till ett nytt spel startas"*/ST_INFOWAITFORGAME);
              //    MessageBox(hwnd,"Tryck p� ok f�r att spela p� nytt","Nu �r spelet slut",NULL);
               }
               break;
            case FD_CLOSE:
               MessageBox(hwnd,/*"Serverns anslutning st�ngdes\nProgrammet kommer avslutas."*/ST_CLOSEDCONNECTION1,/*"Spelet avbr�ts"*/ST_CLOSEDCONNECTION2,NULL);
               PostQuitMessage(0);
               break;
         }
         break;
      default:
         return DefWindowProc(hwnd,msg,wparam,lparam);
   }
   return 0;
}

int WINAPI WinMain(    HINSTANCE  hInstance,
                HINSTANCE  hPrevInstance,
                LPSTR  lpszCmdLine,
                int  nCmdShow  )
{
   // Version av Winsocket som skall anv�ndas
   WORD wVersionRequested = MAKEWORD(2, 0);
   WSADATA wsaData;

   // Startar upp winsock.dll
   WSAStartup(wVersionRequested,&wsaData);

   WNDCLASS wnd;
   MSG msg;
   hinst = hInstance;
   if ( !hPrevInstance ) {
      // Skapar windows klass
      wnd.lpfnWndProc = WindowProc;
      wnd.lpszClassName = "Poker_Game";
      wnd.hbrBackground = GetStockObject(WHITE_BRUSH);
      wnd.style = 0;
      wnd.hIcon = LoadIcon(hinst,MAKEINTRESOURCE(ICON_1));
      wnd.hInstance = hinst;

      wnd.cbClsExtra = 0;
      wnd.cbWndExtra = 0;
      wnd.hCursor = LoadCursor(NULL,IDC_ARROW);
      wnd.lpszMenuName = MAKEINTRESOURCE( MENU_1 );
      // Registerar klass
      if ( !RegisterClass(&wnd) )
         return 0;
      // Skapar f�nster
      mainhwnd = CreateWindow( "Poker_Game", /*"Poker Spel"*/ST_GAMETITLE, WS_OVERLAPPEDWINDOW | WS_SYSMENU,  0,0,640,600,NULL,NULL,hInstance,NULL);
      if ( !mainhwnd )
         return 0;
   }
   ShowWindow(mainhwnd,SW_SHOW);
   UpdateWindow(mainhwnd);

   // �ppnar ett dialogf�nster
   DialogBox(hinst,"STARTDIALOG",mainhwnd,(DLGPROC)StartDialogProc);

   // Meddelande loop
   while ( GetMessage(&msg,NULL,NULL,NULL) ) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      UpdateWindow(mainhwnd);
   }
   // S�ger till servern att man kopplar ifr�n
	if (typ_av_spel == PO_CLIENT )
      sendtext(s_eg,"S");
   // St�nger winsock.dll
   WSACleanup();
   return 0;
}

