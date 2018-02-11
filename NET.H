#include "spel.h"
#include "netpoker.h"

#define PO_MEDDELANDE 500
#define PO_KORT       501
#define PO_ACCEPT     502
#define PO_NAMN       503
#define PO_NOTACCEPT  504
#define PO_VINNARE    505
#define PO_BAKSIDA    506
#define PORT 222
char *SockerrToString( int serr );
int connectserver(HWND mainhwnd,SOCKET &s_eg/*,SOCKET s[4],int antal*/);
int connectclient(HWND mainhwnd,SOCKET &s_eg/*,SOCKET &s_an*/, char ipnr[50]);

int sendmed(SOCKET s_an,char meddelande[500]);
int sendkort(SOCKET s_an,Kort kort[5]);
int sendaccept(SOCKET s_an);
int sendnamn(SOCKET s_an,char meddelande[100]);
int sendtext(SOCKET s_an,char text[100]);

int sendbaksida(SOCKET s_an,int baksida[5]);
int sendvinnare(SOCKET s_an, Vinnarstruct vinn );
int recive(SOCKET s_an,Kort kort[5],char meddelande[100], char ip[20]);