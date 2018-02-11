// Dialog rutorna vid starten hanteras 3 st STARTDIALOG, ANTALDIALOG och IPDIALOG
BOOL FAR StartDialogProc ( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
//   int antal=0;
   switch ( msg ) {
      case WM_INITDIALOG:
         SetFocus(hwnd);
         CheckRadioButton(hwnd,ID2,ID4,ID2);
         CheckRadioButton(hwnd,IDGE,IDANSLUT,IDGE);
         return 0;
      case WM_COMMAND:
         switch ( wparam ) {
            // ANTAL dialog
		      case IDOKANTAL:
               // Antal spelare 2, 3 eller 4
               addmed(/*"Välkommen till Poker spelet vänta nu tills spelare anslutit sig för att börja spela"*/ST_WELCOME);
               // Sparar namnet för servern
               strcpy( Namn[spel->antalspelare()-1],Namn[4]);
               // Hämtar servers kort
               spel->kortforspelare(spel->antalspelare()-1,kort);
               // Visar släng knapp
               ShowWindow(bhwnd,SW_SHOW);
               // Stänger dialog
          		DestroyWindow(hwnd);
               break;
            // Typ av spel dialog
            case IDOK:
               // Server
               if ( IsDlgButtonChecked(hwnd,IDGE) == BST_CHECKED ) {
                  // Connect som server
                  connectserver(mainhwnd,s_eg);
                  // Spara serverns namn index 4 till rätt index efter man vet antalet spelare
                  char dlgnamn[20];
                  GetDlgItemText(hwnd,IDNAMN,dlgnamn,20);
                  strcpy(Namn[4],dlgnamn);

                  typ_av_spel=PO_SERVER;
                  // Öppna antal dialogruta
//                  HWND dhwnd;
//                  dhwnd = CreateDialog(hinst,"ANTALDIALOG",mainhwnd,(DLGPROC)StartDialogProc);

//                  SetFocus(dhwnd);
               }
               // Client
               if ( IsDlgButtonChecked(hwnd,IDANSLUT) == BST_CHECKED ) {
                  // Spara namnet i index 0
                  char dlgnamn[20];
                  GetDlgItemText(hwnd,IDNAMN,dlgnamn,20);
                  strcpy(Namn[0],dlgnamn);
                  HMENU hm = GetMenu(mainhwnd);
                  DeleteMenu(hm,MN_NYTT,NULL);

                  // Öppnar ip dialog ruta
//                  HWND dhwnd;
//                  dhwnd = CreateDialog(hinst,"IPDIALOG",mainhwnd,(DLGPROC)StartDialogProc);
                  DialogBox(hinst,"IPDIALOG",mainhwnd,(DLGPROC)StartDialogProc);
//                  SetFocus(dhwnd);
               }
					//DestroyWindow(hwnd);
               EndDialog(hwnd,TRUE);
               break;
            // IP dialog
            case IDOKIP:
               // Hämtar ipnummer och connectar som client
               char ipnr[50];
    			   GetDlgItemText(hwnd,IDIPNUMMER,ipnr,50);
               connectclient(mainhwnd,s_eg,ipnr);
               typ_av_spel=PO_CLIENT;
               // Stänger dialogrutan
// 					DestroyWindow(hwnd);
               EndDialog(hwnd,TRUE);
               break;

            // Stänger info box
            case IDSTANG:
 					DestroyWindow(hwnd);
               break;

				// Avslutar applikationen
            case IDAVSLUTA: case WM_DESTROY: case WM_QUIT:
               PostQuitMessage(0);
               break;
            // Val av antalspelare 2, 3 eller 4
            case ID2: case ID3: case ID4:
               CheckRadioButton(hwnd,ID2,ID4,wparam);
               break;
            // Val att ge
            case IDGE:
               CheckRadioButton(hwnd,IDGE,IDANSLUT,IDGE);
               break;
            // Val att ansluta
            case IDANSLUT:
               CheckRadioButton(hwnd,IDGE,IDANSLUT,IDANSLUT);
               break;

         }
         break;
      default:
         return FALSE;//DefWindowProc(hwnd,msg,wparam,lparam);
   }
   return TRUE;
}
