
#ifndef _OOINL1H
#define _OOINL1H
#define IDI_NASTA 201
#define IDAVSLUTA 203
#define IDRADIO 204
#define ID2 205
#define ID3 206
#define ID4 207
#define IDGE 208
#define IDANSLUT 209
#define IDIPNUMMER 210
#define IDSTANG 220
#define IDC_STATICTEXT1	101
#define IDC_STATICTEXT2	102
#define PO_SERVER 201   // Spelet agerar server
#define PO_CLIENT 202    // Spelet agerar client
#define WM_SOCKET_SELECT_SERVER         (WM_USER + 100)
#define WM_SOCKET_SELECT_CLIENT         (WM_USER + 101)
#define WM_SOCKET_ASYNC                 (WM_USER + 102)
#define WM_GEKORT                       (WM_USER + 103)
#define WM_ANSLUT_SERVER                (WM_USER + 104)
#define IDI_SKICKA_MED 211
#define IDNAMN 212
#define IDOKANTAL 213
#define IDOKIP 214
#define ICON_1	1
#define IDI_NYTT 215
#define MENU_1	216
#define MN_NYTT 217
#define MN_AVSLUTA 218
#define MN_INFO 219
#define IDI_NASTA_KORT 225
#define VERINFO 300

typedef struct _Vinnarstruct {
   int antal;					  // Antal spelare
   char namn[4][20];         // Namn i ordning på hur reslutatet blev
   char hand[4][20];			  // Hand           -   "    -
   Kort kort[5];				  // Vinnarens kort
   Kort allakort[4][20];     // Alla spelares kort
} Vinnarstruct;

#endif
