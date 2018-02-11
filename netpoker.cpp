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

HWND bhwnd;                // Knapp för släng kort
HWND bmhwnd;               // Knapp för skicka meddelande
HWND ehwnd;                // edit box för meddelanden
HWND nhwnd;                // Knapp nytt spel
HWND bnhwnd;					// Knapp för att visa nästa spelares kort
HWND mainhwnd;             // Huvudfönster
HINSTANCE hinst;           // Program instance
Game *spel;                // Spelet
bool connaccept = false;   // Connection accepted
int typ_av_spel=-1;        // Server eller Client mode PO_SERVER PO_SERVER
int anslutna=0;            // Antal ansluta
SOCKET s[4];               // Anslutna sockets
SOCKET s_an;               // Ansluten socket
SOCKET s_eg;               // Egen socket
char skrivmed[5][100] = { "","","","","" };  // Meddelande buffer
char ipadresser[4][20];                      // Ip addresser används av server
SOCKADDR_IN sockAddr[4];                     // Socket addresser används avserver
Kort kort[5];                                // Synliga kort
int baksida[5] = { 0, 0, 0, 0, 0 };          // Baksidan på kortet som visas 1 är baksida 0 inte basida
char Namn[5][20] = { "","","","","" };       // Namn på spelarna finns endast på server
                                             // Ens eget namn lagrat i index 0 för client 4 för server
Grafik grafik;
int antal_slang[4];                          // Antalkort som skall slängas
int spelare_som_slangt=0;                    // Antal spelare som slängt sina kort
bool spelar = false;                         // Spelet är i gång
bool slangakort=true;                        // Korten är slängda eller ej
char somslangt[4][20]={ "", "", "", ""  };   // Namn på dem som slängt kort
Vinnarstruct vinnare;								// Vinnarstuct med info om hur spelet gick
Vinnarstruct *vinn;									// Pekare till vinnarstruct
bool visa_vinnare = false;							// visa vem som vann
bool nytt_spel = true;
int omgang = 1;										// Spel omgång
int vald = 0;

int addmed(char text[50]);
BOOL FAR StartDialogProc ( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

#include "startdialog.cpp"
// Lägger till ett meddelande i meddelande buffer
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

// Windowoproc har hand om utskrift för skärm händelser sam nät
LRESULT CALLBACK WindowProc ( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
   int i;
   HDC hdc;
   switch ( msg ) {
      case WM_CREATE:
         // Knapp för att slänga kort
         bhwnd  = CreateWindow("Button",/*"Nöjd"*/ST_SATISFIED,WS_CHILDWINDOW | BS_PUSHBUTTON,350,150,100,30,hwnd,(HMENU)IDI_NASTA,hinst,NULL);
         // Edit fält för att skriva meddelanden
         ehwnd  = CreateWindow("Edit","", WS_BORDER	| ES_AUTOHSCROLL | ES_WANTRETURN | WS_TABSTOP | WS_VISIBLE | WS_CHILDWINDOW ,10,400,200,25,hwnd,NULL,hinst,NULL);
         // Knapp för att skicka meddelanden
         bmhwnd = CreateWindow("Button",/*"Skicka meddelande"*/ST_SENDMSG,WS_TABSTOP | WS_VISIBLE | WS_CHILDWINDOW | BS_DEFPUSHBUTTON,230,402,150,25,hwnd,(HMENU)IDI_SKICKA_MED,hinst,NULL);
         // Knapp nästa
         nhwnd  = CreateWindow("Button",/*"Nytt spel"*/ST_NEWGAME,WS_TABSTOP | WS_VISIBLE | WS_CHILDWINDOW | BS_DEFPUSHBUTTON,430,402,150,25,hwnd,(HMENU)IDI_NYTT,hinst,NULL);
         bnhwnd  = CreateWindow("Button",/*"Nästa spelare"*/ST_NEXTPLAYER,WS_TABSTOP | WS_CHILDWINDOW | BS_DEFPUSHBUTTON,360,22,100,15,hwnd,(HMENU)IDI_NASTA_KORT,hinst,NULL);
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
         // Hämtar kordinater för fönster och fyller med grön
         GetClientRect(hwnd,&trect);
         FillRect(hdc,&trect,CreateSolidBrush(0x017F01));
         // Fyller meddelande del med annan grön färg
         trect.left = 0;
         trect.top  = 440;
         FillRect(hdc,&trect,CreateSolidBrush(0x4E804E));

         // Skriver ut de 5 korten
         for ( i=0;i<5;i++)
         {
            // Punkter för kortet
            lp.x = 20+i*90;  // Kortet plats måste anges
            lp.y = 50;
            Rectangle(hdc,20+i*90-1,47,20+i*90+76,142);
            // Om man inte spelar ritas ett kort vänt på baksidan ut
            // annars ritas kortet ut på rätt sida
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
         TextOut(hdc,500,44,/*"slängt kort:"*/ST_PLAYERSTHROW2,strlen(ST_PLAYERSTHROW2));
         for ( i=0;i<4;i++ )
            TextOut(hdc,500,60+i*16,somslangt[i],strlen(somslangt[i]));

         // Skriver ut antalet anslutna spelare för servern  ÄNDRA DET SEN
         if ( typ_av_spel == PO_SERVER  ) {
            char text[100];
            strcpy(text,/*"Anslutna spelare: "*/ST_CONNECTEDPLAYERS);
            char tal[5];
            itoa(anslutna,tal,10);
            strcat(text,tal);
            TextOut(hdc,1,1,text,strlen(text));
         }
         SetBkColor(hdc,0x4E804E);

         // Skriver ut meddelanden från meddelande buffer
         for ( i=0;i<5;i++ )
            TextOut(hdc,5,450+i*16,skrivmed[i],strlen(skrivmed[i]));
         if ( visa_vinnare && typ_av_spel == PO_SERVER)
            ShowWindow(nhwnd,SW_SHOW);
         EndPaint(hwnd,&ps);
         break;

      case WM_TIMER:
         // Timer för anslutning om ingen anslutning på 10 sek så avslutas programmet
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
         // Om man spelar och inte slängt korten kan man vända korten
         if ( spelar && !visa_vinnare)
         for ( i=0;i<5;i++)
         {
            // Kollar om koordinaterna och ser om man trycker på ett kort
            if ( x>20+i*90 && x < 20+i*90+75 && y > 50 && y < 140 ) {
               // Får man inte slänga kort skriv meddelande annars vänd kort
               if ( !slangakort )
                  addmed(/*"Man kan inte slänga kort än vänta på dina nya kort"*/ST_ERR_NOTTHROW);
               else {
                  baksida[i] = !baksida[i];
                  RedrawWindow(hwnd,NULL,NULL,RDW_ERASE|RDW_INVALIDATE);
               }
               break;
            }
         }
         if ( baksida[0] || baksida[1] || baksida[2] || baksida[3] || baksida[4] )
            SetWindowText(bhwnd,/*"Släng kort"*/ST_THROWCARDS);
         else
            SetWindowText(bhwnd,/*"Nöjd"*/ST_SATISFIED);
         break;

      case WM_COMMAND:
         switch(LOWORD(wparam)) {
            // trycker på knappen för att slänga
            case IDI_NASTA:
               // får man slänga kort
               if ( slangakort ) {
                  // är man klient skickas dom till server och man får inte slänga kort
                  if ( typ_av_spel == PO_CLIENT ) {
                     sendbaksida(s_eg,baksida);
                     SetWindowText(bhwnd,/*"Vänta"*/ST_WAIT);
                     EnableWindow(bhwnd,FALSE);
                     slangakort=false;
                  }
                  // Är man server
                  if ( typ_av_spel == PO_SERVER && anslutna > 0) {
                     // Hur många kort är slängda och de slängs
                     antal_slang[anslutna] = spel->slangkort(anslutna,baksida);

                     for ( int u = 0; u < anslutna; u++ )
                        sendnamn(s[u],Namn[anslutna]);
                     int u;
                     u=0;
                     while ( somslangt[u][0]!='\0' ) u++;
                     strncpy(somslangt[u],Namn[anslutna],20);
                     SetWindowText(bhwnd,/*"Vänta"*/ST_WAIT);
                     EnableWindow(bhwnd,FALSE);
                     // Kollar om alla slängt om ej så skrivs ett vänta meddelande annars
                     // ska alla kort skickas ut
                     if ( spelare_som_slangt == anslutna )
                        SendMessage(hwnd,WM_GEKORT,0,0);
                     else {
                        // Vänta meddelande
                        addmed(/*"Vänta till alla slängt sina kort då får du nya!"*/ST_WAITFORCARDS);
                        slangakort = false;
                        spelare_som_slangt++;
                     }
                  }
                  if ( typ_av_spel == PO_SERVER && anslutna == 0 )
                     addmed(/*"Vänta tills alla spelare anslutit sig"*/ST_WAITFORPLAYERS);
               } else
                  addmed(/*"Du får vänta till alla spelare slängt sina kort"*/ST_WAITFORCARDS);

               break;
            // Skicka ett meddelande
            case IDI_SKICKA_MED:
               // Kollar om det är ett meddelande
               char m[10];
               GetWindowText(ehwnd,m,10);
               if ( m[0] != '\0' ) {
                  // Klienten skickar det till servern
                  if ( typ_av_spel == PO_CLIENT ) {
                     char med[100];
                     GetWindowText(ehwnd,med,100);
                     sendmed(s_eg,med);
                     SetWindowText(ehwnd,"");
                  // Servern skriver ut det själv och skickar det till alla klienter
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
               // Nytt spel endast servern kan ändra detta
               if ( anslutna ) {
             		if ( nytt_spel && typ_av_spel == PO_SERVER  && ( !spelar || visa_vinnare ) ) {
               		visa_vinnare = false;
	                  ShowWindow(bhwnd,SW_SHOW);
   	               for ( int i=0;i<anslutna;i++) {
      	            	sendaccept(s[i]);
         	            sendmed(s[i],/*"Nu börjar ett nytt spel. Nya kort till alla"*/ST_NEWGAMESTARTED);
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
               	   addmed(/*"Nu börjar ett nytt spel"*/ST_NEWGAMESTARTED);
	                  spelar = true;
   	               nytt_spel = false;
						}
         	      if ( visa_vinnare && typ_av_spel == PO_SERVER ) {
            	      addmed(/*"Väntar på anslutande spelare."*/ST_WAITFORCONNECTIONS);
               	   addmed(/*"Tryck på Nytt spel, för att spela"*/ST_INFONEWGAME);
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

      // Ger kort till alla som slängt kort
      // Endast servern kan anropa detta meddelandet
      case WM_GEKORT:

         int ok;
         char med[100];
         char tal[5];
         itoa(omgang,tal,10);
         char text[20];
         strcpy(text,"Omgång ");
         strcat(text,tal);
         ok = MessageBox(hwnd,/*"Vill du fortsätta att spela!"*/ST_CONTTOPLAY,text,MB_YESNO|MB_DEFBUTTON1|MB_SYSTEMMODAL);
         // Hämtar nya kort till alla även server
         for ( i = 0; i <= anslutna; i++ ) {
            // Hämtar kort till användare i
            spel->hemtakort(i, antal_slang[i]);
            Kort tmpkort[5];
            spel->kortforspelare(i,tmpkort);
            int o;
            // Om man är server skriv korten dirket till kort variabel
            if ( i == anslutna )
               for ( o = 0; o < 5 ; o++ )
                  kort[o] = tmpkort[o];
            else
               // Annars skickas korten till Klient
               sendkort(s[i],tmpkort);
            // Man skapar ett meddelande om att användare slängt x kort
            char antal[2];
            itoa(antal_slang[i],antal,10);
            strcpy(med, /*"Spelare "*/ST_PLAYER);
            strcat(med, Namn[i] );
            strcat(med, /*" slängde "*/ ST_THREW);
            strcat(med, antal );
            strcat(med, /*" kort."*/ST_CARDSDOT);
            int u = 0;
            for ( u = 0; u < anslutna; u++)
               sendmed(s[u],med);
            // Skriver ut för server med
            addmed(med);
         }
         omgang ++ ;
         EnableWindow(bhwnd,TRUE);
         SetWindowText(bhwnd,/*"Nöjd"*/ST_SATISFIED);
         ShowWindow(bhwnd,SW_SHOW);
         if ( ok == IDNO ) {
            int vinnint;
            int ord[5];
            Kort vinnkort[5];
            spel->spelvinnare(vinnint,vinnkort,ord);
            // Kopierar den vinnande handen
            for ( int u = 0; u < 5; u++ )
               vinnare.kort[u]=vinnkort[u];
            // Kopierar allas händer av kort för att redovisa det senare
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
         // Inget kort är vänt på baksida
         for ( u = 0 ; u< 5 ; u ++ ) {
            baksida[u] = 0;
            if ( u < 4 ) strcpy(somslangt[u],"\0");
         }
         RedrawWindow(hwnd,NULL,NULL,RDW_ERASE|RDW_INVALIDATE);
         // Nu har ingen slängt kort
         spelare_som_slangt = 0;
         // Och severn får slänga kort
         slangakort = true;
         break;

      // SERVER För nätet  --> till separat fil
      case WM_SOCKET_SELECT_SERVER:
         switch(LOWORD(lparam)) {
            // Accpetera en anslutning från klient ?
            case FD_ACCEPT:

               // Finns det plats för klient
               if ( anslutna < 3 && !spelar ) {

                  WSAAsyncSelect( s_eg,
                            hwnd,
                            WM_SOCKET_SELECT_SERVER,
                            FD_READ | FD_WRITE | FD_CLOSE );
                  int length;
                  length=sizeof(sockAddr);
                  // Hämtar socket för klient
                  s[anslutna]=accept(s_eg,(struct sockaddr *)&sockAddr,&length);
                  // för en ny anslutnings möjlighet för servern
                  // kopierar ipadress är det nödvändigt ?

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
   // bestämer vilka händelser som skall tas om hand om
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
                  // Om alla möjliga anslutningar är genomförda så skickas ett ok meddelande från server
                  sendmed(s[anslutna-1],/*"Väntar på att spelare skall ansluta..."*/ST_INFOWAITFORPLAYERS);
               }
               break;

            // Läs data från en klient
            case FD_READ:
               char meddelande[100];
               // Går igenom alla anslutna sockets
               for ( int i=0;i<anslutna;i++) {
                  // Hämtar meddelnade
                  strcpy(meddelande,"");
                  char ip[20];
                  int typ = recive(s[i],NULL,meddelande,ip);
                  // Är det ett skriftilig meddelande
                  if ( typ == PO_MEDDELANDE ) {
                     // Skicka vidare det och säg vem det kommer i från
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
                  // är det ett namn på en ansluten klient
                  if ( typ == PO_NAMN ) {
                     // Hämtar namnet till namn listan
                     strcpy(Namn[i],meddelande);
                     char med[100];
                     // Skapar meddelande till andra anslutna
                     strcpy(med,/*"Användare "*/ST_PLAYER);
                     strcat(med,Namn[i]);
                     strcat(med,/*" är ansluten"*/ST_ISCONNECTED);
                     for ( int u=0; u <= anslutna;u++ )
                        sendmed(s[u],med);
                     addmed(med);
                  }
                  // Är det vilka som är baksidor på korten
                  if ( typ == PO_BAKSIDA ) {
                     int *bak;
                     bak = new int[5];
                     // Dom ligger i meddelande buffer
                     bak = (int *)meddelande;
                     // Slänger kort och hämtar hur många som slängs
                     antal_slang[i] = spel->slangkort(i,bak);

                     for ( int u = 0; u < anslutna; u++ )
                        sendnamn(s[u],Namn[i]);
                     u = 0;
                     while ( somslangt[u][0]!='\0') u++;
                     strncpy(somslangt[u],Namn[i],20);

                     // Om alla slängt så skicka meddelande om att hämta kort
                     if ( spelare_som_slangt == anslutna )
                        SendMessage(hwnd,WM_GEKORT,0,0);
                     else {
                        // Annars får man vänta på nya kort
                        sendmed(s[i],/*"Vänta till alla slängt sina kort då får du nya!"*/ST_WAITFORCARDS);
                        spelare_som_slangt++;
                     }
                     RedrawWindow(hwnd,NULL,NULL,RDW_ERASE|RDW_INVALIDATE);
                  }
                  if ( typ == PO_NOTACCEPT ) {
                     char med[100];
                     strcpy(med, /*"Spelare "*/ST_PLAYER);
                     strcat(med,Namn[i]);
                     strcat(med,/*" kopplade ifrån sig"*/ST_DISCONNECT);
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
               // Säger att man är ansluten
               addmed(/*"Väntar på att servern accepterar."*/ST_ACCEPTED);
               // Sätter timer för att kolla om anslutningen håller
               SetTimer(hwnd,1,10000,(TIMERPROC)WindowProc);
               break;
            case FD_READ:
               char *meddelande;
               meddelande = new char[500];
               char ip[20];
               int typ;
               // Hämtar meddelande från servern
               typ = recive(s_eg,kort,meddelande,ip);
               // är det ett text meddelande skrivs det ut
               if ( typ == PO_MEDDELANDE )
                  addmed(meddelande);

               // Ett accpetering meddelande, man får vara med och spela
               if ( typ == PO_ACCEPT ) {
                  connaccept = true;
                  visa_vinnare = false;
                  // Skickar klientens namn till servern                  
                  sendnamn(s_eg,Namn[0]);
                  ShowWindow(bhwnd,SW_SHOW);
               }

               //KORT
               // Får kort från servern
               if ( typ == PO_KORT ) {
                  // Man spelar och får slänga kort
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
                  SetWindowText(bhwnd,/*"Nöjd"*/ST_SATISFIED);

                  RedrawWindow(hwnd,NULL,NULL,RDW_ERASE|RDW_INVALIDATE);
               }

               // Namn på dem som slängt
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
   					addmed(/*"Vänta till ett nytt spel startas"*/ST_INFOWAITFORGAME);
              //    MessageBox(hwnd,"Tryck på ok för att spela på nytt","Nu är spelet slut",NULL);
               }
               break;
            case FD_CLOSE:
               MessageBox(hwnd,/*"Serverns anslutning stängdes\nProgrammet kommer avslutas."*/ST_CLOSEDCONNECTION1,/*"Spelet avbröts"*/ST_CLOSEDCONNECTION2,NULL);
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
   // Version av Winsocket som skall användas
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
      // Skapar fönster
      mainhwnd = CreateWindow( "Poker_Game", /*"Poker Spel"*/ST_GAMETITLE, WS_OVERLAPPEDWINDOW | WS_SYSMENU,  0,0,640,600,NULL,NULL,hInstance,NULL);
      if ( !mainhwnd )
         return 0;
   }
   ShowWindow(mainhwnd,SW_SHOW);
   UpdateWindow(mainhwnd);

   // Öppnar ett dialogfönster
   DialogBox(hinst,"STARTDIALOG",mainhwnd,(DLGPROC)StartDialogProc);

   // Meddelande loop
   while ( GetMessage(&msg,NULL,NULL,NULL) ) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      UpdateWindow(mainhwnd);
   }
   // Säger till servern att man kopplar ifrån
	if (typ_av_spel == PO_CLIENT )
      sendtext(s_eg,"S");
   // Stänger winsock.dll
   WSACleanup();
   return 0;
}

