#include <windows.h>
#include "grafik.h"
#include "cards.h"

int Grafik::ritakort(HWND hwnd,HINSTANCE hinst, POINT p, int baksida,int farg, int valor) {
   HDC hdc = GetDC(hwnd);
   // Kopierar text från minnet
   HDC hdcmem;
   HBITMAP hbm;
   DWORD re = MAKEWORD(valor,farg+1);
   if ( !baksida )
      hbm = LoadImage(hinst,MAKEINTRESOURCE(re),IMAGE_BITMAP,75,90,NULL);
   else
      hbm = LoadImage(hinst,MAKEINTRESOURCE(BAKSIDA),IMAGE_BITMAP,75,90,NULL);

   hdcmem = CreateCompatibleDC( hdc );
   SetMapMode( hdcmem, GetMapMode( hdc ) );
   SelectObject(hdcmem, hbm);

   POINT org;
   org.x=0;
   org.y=0;
   DPtoLP(hdcmem,&org,1);
   BitBlt(hdc,p.x,p.y,75,90,hdcmem,org.x,org.y, SRCCOPY);
   // raden ovanför
   DeleteDC(hdcmem);
   DeleteObject(hbm);
   ReleaseDC(hwnd,hdc);
   return 1;
}

