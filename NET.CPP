/* Filename: net.cpp
 * This file contains the fundamental tcp/ip communication
 * using winsocket.
 * Created by: Markus Svensson 2002-04-xx
 * Modified by: Markus Svensson 2002-03-28
 */
 
#include <windows.h>
#include "net.h"
#include "netpoker.h"
#include "text.h"
LPHOSTENT hostn;
/* funktioner
   Server:
     connect(SOCKET &s_eg,SOCKET s[4],int antal);
     skicka_kort(KORT [5][5]);
     get_meddelande(char med[100]);

   Client:
     connect(SOCKET &s_eg, SOCKET &s_an);
     get_kort(KORT [5]);
     get_meddelande(char med[100]);


   För båda:
    Handle switch
    FD_READ:
      läs en byte A - Accept
                  S - Not accept
                  K - Kort
                  M - Meddelande
                  V - Vinnare skickar en struct med info
                  B - Baksida på korten en int[5] lista där 0 är inte baksida
                  Client
                  N - Namn på spelare som slängt kort
                  Server
                  N - Namn på spelare som anslutit sig
      sedan resten
 */

SOCKADDR_IN sockaddr;

// Översätter fel från sockets till text
char *SockerrToString( int serr )
{
    switch( serr )
    {
    case WSAENAMETOOLONG :
        return "Name too long";

    case WSANOTINITIALISED :
        return "Not initialized";

    case WSASYSNOTREADY :
        return "System not ready";

    case WSAVERNOTSUPPORTED :
        return "Version is not supported";

    case WSAESHUTDOWN :
        return "Can't send after socket shutdown";

    case WSAEINTR :
        return "Interrupted system call";

    case WSAHOST_NOT_FOUND :
        return "Host not found";

    case WSATRY_AGAIN :
        return "Try again";

    case WSANO_RECOVERY :
        return "Non-recoverable error";

    case WSANO_DATA :
        return "No data record available";

    case WSAEBADF :
        return "Bad file number";

    case WSAEWOULDBLOCK :
        return "Operation would block";

    case WSAEINPROGRESS :
        return "Operation now in progress";

    case WSAEALREADY :
        return "Operation already in progress";

    case WSAEFAULT :
        return "Bad address";

    case WSAEDESTADDRREQ :
        return "Destination address required";

    case WSAEMSGSIZE :
        return "Message too long";

    case WSAEPFNOSUPPORT :
        return "Protocol family not supported";

    case WSAENOTEMPTY :
        return "Directory not empty";

    case WSAEPROCLIM :
        return "EPROCLIM returned";

    case WSAEUSERS :
        return "EUSERS returned";

    case WSAEDQUOT :
        return "Disk quota exceeded";

    case WSAESTALE :
        return "ESTALE returned";

    case WSAEINVAL :
        return "Invalid argument";

    case WSAEMFILE :
        return "Too many open files";

    case WSAEACCES :
        return "Access denied";

    case WSAELOOP :
        return "Too many levels of symbolic links";

    case WSAEREMOTE :
        return "The object is remote";

    case WSAENOTSOCK :
        return "Socket operation on non-socket";

    case WSAEADDRNOTAVAIL :
        return "Can't assign requested address";

    case WSAEADDRINUSE :
        return "Address already in use";

    case WSAEAFNOSUPPORT :
        return "Address family not supported by protocol family";

    case WSAESOCKTNOSUPPORT :
        return "Socket type not supported";

    case WSAEPROTONOSUPPORT :
        return "Protocol not supported";

    case WSAENOBUFS :
        return "No buffer space is supported";

    case WSAETIMEDOUT :
        return "Connection timed out";

    case WSAEISCONN :
        return "Socket is already connected";

    case WSAENOTCONN :
        return "Socket is not connected";

    case WSAENOPROTOOPT :
        return "Bad protocol option";

    case WSAECONNRESET :
        return "Connection reset by peer";

    case WSAECONNABORTED :
        return "Software caused connection abort";

    case WSAENETDOWN :
        return "Network is down";

    case WSAENETRESET :
        return "Network was reset";

    case WSAECONNREFUSED :
        return "Connection refused";

    case WSAEHOSTDOWN :
        return "Host is down";

    case WSAEHOSTUNREACH :
        return "Host is unreachable";

    case WSAEPROTOTYPE :
        return "Protocol is wrong type for socket";

    case WSAEOPNOTSUPP :
        return "Operation not supported on socket";

    case WSAENETUNREACH :
        return "ICMP network unreachable";

    case WSAETOOMANYREFS :
        return "Too many references";

    default :
        return "Unknown";
    }
}

// Ansluter som server
int connectserver(HWND mainhwnd,SOCKET &s_eg/*,SOCKET s[4],int antal*/) {

   // Skapar en socket struct
   sockaddr.sin_family      = AF_INET;
   sockaddr.sin_addr.s_addr = INADDR_ANY;
   sockaddr.sin_port        = htons(PORT);

   // Skapar socket
   s_eg = socket( AF_INET,SOCK_STREAM,0);
   while( s_eg == INVALID_SOCKET ) {
      char felnummer[50];
      strcpy(felnummer,SockerrToString(WSAGetLastError()));
      MessageBox(NULL,"Fel",felnummer,NULL);
      s_eg = socket( AF_INET,SOCK_STREAM,0);
   }
   int reuse=0xffffffff;
   // Sätter socket till att användas flera gånger -> många klienter
   setsockopt(s_eg,SOL_SOCKET,SO_REUSEADDR,(char *)&reuse,sizeof(int));
   // Binder socket till adress
   while ( bind(s_eg,(LPSOCKADDR)&sockaddr,sizeof(sockaddr)))
   {
      char felnummer[50];

      strcpy(felnummer,SockerrToString(WSAGetLastError()));
      MessageBox(NULL,"Fel",felnummer,NULL);
   }


   // Socket lyssnar på anrop
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
   return 0;
}

// Ansluter som klient
int connectclient(HWND mainhwnd,SOCKET &s_eg/*,SOCKET &s_an*/, char ipnr[50]) {
   // Skapar TCP socket
   s_eg = socket( AF_INET,SOCK_STREAM,0);
   while( s_eg == INVALID_SOCKET ) {
      char felnummer[50];
      strcpy(felnummer,SockerrToString(WSAGetLastError()));
      MessageBox(NULL,"Fel",felnummer,NULL);
      s_eg = socket( AF_INET,SOCK_STREAM,0);
   }
   // Skriver in i socket strcut vilken addres och port man ska ansluta till
   if ( ipnr[0] > 59 ) {
      hostn = new HOSTENT;
      hostn = gethostbyname(ipnr);
      int yesno = IDYES;
      char felnummer[50];      
      while ( !hostn && yesno==IDYES ) {

         strcpy(felnummer,SockerrToString(WSAGetLastError()));
         yesno = MessageBox(NULL, ST_ASKCONTINUE,felnummer,MB_YESNO);
         hostn = gethostbyname(ipnr);

      }
      if ( yesno==IDNO ) {
			MessageBox(NULL, ST_CLOSEDCONNECTION1,felnummer,NULL);
         PostQuitMessage(1);
         return 1;
      }
      in_addr in;
      memcpy(&in.s_addr,hostn->h_addr_list[0],4);

      strcpy(ipnr,inet_ntoa(in));
   }
   sockaddr.sin_family      = AF_INET;
   sockaddr.sin_addr.s_addr = inet_addr(ipnr);
   sockaddr.sin_port        = htons(PORT);

   // Ansluter till adressen och porten
   if(connect(s_eg,(struct sockaddr FAR *)&sockaddr,sizeof(sockaddr))) {
      char felnummer[50];
      strcpy(felnummer,SockerrToString(WSAGetLastError()));
      MessageBox(mainhwnd,"Fel",felnummer,NULL);
   }

   // Bestämmer vilka händelser som skall hanteras
   while( WSAAsyncSelect( s_eg,
                      mainhwnd,
                      WM_SOCKET_SELECT_CLIENT,
                      FD_CONNECT|
                      FD_READ|
                      FD_WRITE|
                      FD_ACCEPT|
                      FD_CLOSE ) != 0 ) {
      char felnummer[50];
      strcpy(felnummer,SockerrToString(WSAGetLastError()));
      MessageBox(mainhwnd,"Fel",felnummer,NULL);
   }
   SendMessage(mainhwnd,WM_SOCKET_SELECT_CLIENT,NULL,FD_CONNECT);
   return 0;
}

// Hämtar meddelande kort mm
int recive(SOCKET s_an,Kort kort[5],char meddelande[500],char ip[20]) {
   char buf[10];
   // Hämtar en byte från nätbuffern
   recv(s_an,buf,1,0);
   buf[1]='\0';
//   char *npek = new char[sizeof(kort)*10];
   // Mha en byte kan man se vilket meddelande det gäller
   switch(buf[0]) {
      case 'M':
         // Textmeddelande läser in längd och därefter text
         recv(s_an,buf,1,0);
         recv(s_an,meddelande,int(buf[0])+2,0);
         return PO_MEDDELANDE;
      case 'N':
         // Namn på en spelare läser in längd sedan text
         recv(s_an,buf,1,0);
         recv(s_an,meddelande,int(buf[0])+2,0);
         return PO_NAMN;
      case 'S':
         // Inte accepterad meddelande
         return PO_NOTACCEPT;
      case 'K':
         // Kort struct
         {
            recv(s_an,(char *)kort,sizeof(kort)*10,0);
         }
         return PO_KORT;
      case 'V':
         // Vinnar struct
         recv(s_an,meddelande,sizeof(_Vinnarstruct),0);
         return PO_VINNARE;
      case 'A':
         // Uppkoppling accpeterad
         return PO_ACCEPT;
      case 'B':
         // baksida på kort lista
         recv(s_an,meddelande,sizeof(int)*5,0);
         return PO_BAKSIDA;
   }
   return 0;
}

// Skickar kort till klienter
int sendkort(SOCKET s_an, Kort kort[5]) {
   // Skapar temporära char buffrar
   char tmpbuf[100]="K";
   char *tmpkort = new char[sizeof(kort)*10];
   // kopierar addressen för kort till en char pekare
   tmpkort = (char *)kort;
   // Kopierar innehållet till en annan char temp buffer
   for ( int t = 0;t<sizeof(kort)*10;t++)
     tmpbuf[t+1] = tmpkort[t];
   // skickar buffern till klient
   send(s_an,tmpbuf,sizeof(kort)*10+1,0);
   return 0;
}

int sendmed(SOCKET s_an,char meddelande[100]) {
   // skapar dynamisk char pekare
   char *medmed = new char[strlen(meddelande)+3];
   // kopierar medelande notation samnt längd till medelande buffern
   strcpy(medmed,"M \0");
   medmed[1] = (char)strlen(meddelande+1);
   // Kopierar meddelande till meddelande buffern
   strcat(medmed,meddelande);
   // skickar hela meddelandet till klient
   while( strlen(medmed)+1 > send(s_an,medmed,strlen(medmed)+2,0) );
   return 0;
}
// Skickar accept meddelande till klient
int sendaccept(SOCKET s_an) {
   // skickar ett A till klient -> anslutning accpeterad
   while( 1 > send(s_an,"A\0",2,0) );
   return 0;
}

// Skickar namn till klient eller server
int sendnamn(SOCKET s_an,char meddelande[100]) {
   // skapar meddleade buffer
   char *medmed = new char[strlen(meddelande)+3];
   // Lägger till N för att ange Namn buffer
   strcpy(medmed,"N \0");
   // Lägger till namnets längd
   medmed[1] = (char)strlen(meddelande+1);
   // Kopierar namnet till meddelande buffer
   strcat(medmed,meddelande);
   // skickar meddelande buffern till klient/server
   while( strlen(medmed)+1 > send(s_an,medmed,strlen(medmed)+2,0) );
   return 0;
}

// skickar baksida lista till server
int sendbaksida(SOCKET s_an,int baksida[5]) {
   // skapar temp buffrar
   char tmpbuf[100]="B";
   char *tmpkort = new char[sizeof(int)*5];
   // Sätter addressen till tmp pekare
   tmpkort = (char *)baksida;
   // kopierar innehållet till temp buffer
   for ( int t = 0;t<sizeof(int)*5;t++)
     tmpbuf[t+1] = tmpkort[t];
   // Skickar till server
   send(s_an,tmpbuf,sizeof(int)*5+1,0);
   return 0;
}

// Skickar en vinnarestruct till klient
int sendvinnare(SOCKET s_an, Vinnarstruct vinn ) {
   // skapar en temp buffer
   char tmpbuf[5000]="V";
   // skapar en temp pekare
   char *tmpvinn = new char[sizeof(vinn)];
   // kopierar adressen
   tmpvinn = (char *)&vinn;
   // Kopierar innehållet
   for ( int t = 0;t<sizeof(vinn);t++)
     tmpbuf[t+1] = tmpvinn[t];
   // Skickar till klient
   send(s_an,tmpbuf,sizeof(vinn)+1,0);
   return 0;
}

// Skickar ren text
int sendtext(SOCKET s_an,char text[100]) {
   send(s_an,text,strlen(text)+1,0);
   return 1;
}
