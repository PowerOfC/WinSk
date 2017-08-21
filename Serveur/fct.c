//=============================================================================
// Projet : Winrell (server)
// Fichier : fct.c (fonctions)
//
//=============================================================================

//#include "server.h" // déja inclus dans "exploreTypes.h"
#include "exploreTypes.h"
#include "commands.h"


//=============================================================================
//                    Fonction d'affichage sur la RichConsole
//
//=============================================================================

void appendToRichConsole(char *title, const char *buffer)
{
     char *temp = get_time(2), chaine[strlen(temp)+strlen(title)+strlen(buffer)+8];
     
     sprintf(chaine, "[%s] %s : %s\n", temp, title, buffer);
     free(temp);
     /* Sauvegarde + affichage */
     enregistrerDansLog(chaine, fichierLog, FALSE);
     if (!HIDE_CONSOLE_MSG)
        afficherSurRichConsole(chaine);
     //if ((!enregistrerDansLog(chaine, fichierLog, FALSE) || !lireLogEtAfficherSurRichConsole(fichierLog)))
     //{
        //MessageBox(hwFenetre, "Erreur de sauvegarde/affichage !", "Fermeture de " NOM_APP, MB_OK | MB_ICONWARNING);
        //exit(EXIT_FAILURE);
     //}
}

//=============================================================================
//         Fonction qui enregistre les données dans le fichier spécifié
//
//=============================================================================

BOOL enregistrerDansLog(char *log, char *fichier, BOOL ecraserAncien)
{
   HANDLE hFichier;
   DWORD FileSize, nbcharRead;
   
   FileSize = lstrlen(log);
   if (ecraserAncien)
      hFichier = CreateFile(fichier, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   else
      hFichier = CreateFile(fichier, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if(hFichier != INVALID_HANDLE_VALUE)
   {
      /* Mettre le pointeur du fichier à la fin */
      if (!ecraserAncien)
         SetFilePointer(hFichier, 0, NULL, FILE_END);
      /* Ecrire la lettre dans le fihier log */
      WriteFile(hFichier, log, FileSize, &nbcharRead, NULL);
      /* Fermer le fichier */
      CloseHandle(hFichier);
      return TRUE;
   }
   else
      return FALSE;
}

//=============================================================================
//             Fonction qui lit les données du fichier spécifié
//                     et affiche sur la RichConsole
//
//=============================================================================

BOOL lireLogEtAfficherSurRichConsole(char *fichier)
{
   HANDLE hFichier;
   DWORD FileSize, nbcharRead;
   CHAR *buffer;

   hFichier = CreateFile(fichier, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if(hFichier != INVALID_HANDLE_VALUE) /* Si fichier trouvé */
   {
      FileSize = GetFileSize(hFichier, NULL);
      buffer = (PCHAR)LocalAlloc(LMEM_FIXED, FileSize+1);
      ReadFile(hFichier, buffer, FileSize, &nbcharRead, NULL) ; /* Lecture */
      buffer[FileSize] = 0;
      SendMessage(hwRichConsole, WM_SETTEXT, 0, (LPARAM)buffer); /* Affichage à l'écran */
      if (REFRESH_CONSOLE)
      {
         changerCouleurRichEdit(hwRichConsole, TextColor); // couleur du texte
         //SendMessage(hwRichConsole, WM_HSCROLL, SB_LEFT, 0); /* On se positionne à gauchhe */
      }
      SendMessage(hwRichConsole, WM_VSCROLL, SB_BOTTOM, 0); /* On descend à la dernière ligne */
      LocalFree(buffer);
      CloseHandle(hFichier);
      return TRUE;
   }
   else
      return FALSE;
}

//=============================================================================
//      Fonction qui affiche les données spécifiées sur la RichConsole
//
//=============================================================================

void afficherSurRichConsole(char *texteAafficher)
{
   int old_len = GetWindowTextLength(hwRichConsole), len = old_len + lstrlen(texteAafficher) + 1;
   char *texte = malloc(len);
   
   lstrcpy(texte, ""); /* Initialisation */
   if (old_len > 0)
      SendMessage(hwRichConsole, WM_GETTEXT, old_len+1, (LPARAM)texte); /* On récupère le texte existant dans la RichConsole */
   lstrcat(texte, texteAafficher); /* On lui ajoute le nouveau texte */
   
   SendMessage(hwRichConsole, WM_SETTEXT, 0, (LPARAM)texte); /* Affichage à l'écran */
   
   /* On raffraichie la couleur du texte (pour que ça fonctionne sur Wine) */
   if (REFRESH_CONSOLE)
   {
      changerCouleurRichEdit(hwRichConsole, TextColor); // couleur du texte
      //SendMessage(hwRichConsole, WM_HSCROLL, SB_LEFT, 0); /* On se positionne à gauchhe */
   }
   
   /* Colorisation */
   richConsoleColorization(texte, len);
   
   free(texte);
   
   SendMessage(hwRichConsole, WM_VSCROLL, SB_BOTTOM, 0); /* On descend à la dernière ligne */
}

//=============================================================================
//      Fonction qui rénitialise la couleur du texte de la RichConsole
//
//=============================================================================

void resetRichConsoleTextColor()
{
   int len = GetWindowTextLength(hwRichConsole) + 1;
   char *texte = malloc(len);
   
   GetWindowText(hwRichConsole, texte, len);
   
   /* On raffraichie la couleur du texte (pour que ça fonctionne sur Wine) */
   changerCouleurRichEdit(hwRichConsole, TextColor); // couleur du texte
   
   /* Colorisation */
   richConsoleColorization(texte, len);
   
   free(texte);
   
   SendMessage(hwRichConsole, WM_VSCROLL, SB_BOTTOM, 0); /* On descend à la dernière ligne */
}

//=============================================================================
//          Fonction qui s'occupe de colorier la RichConsole
//
//=============================================================================

void richConsoleColorization(char *texte, int len)
{
   if (ENABLE_COLORIZATION)
   {
      /* Colorisation */
      CHARRANGE CurrentSelection;
      CHARRANGE Selection;
      int i, lines = 0;
   
      /* On récupère la sélection courante afin de la rétablir après le traitement */
      SendMessage(hwRichConsole, EM_EXGETSEL, 0, (LPARAM) &CurrentSelection);
   
      for (i = 0; i < len - 1; i++)
      {
         // send | recv
         if ((texte[i] == 's' && texte[i+1] == 'e' && texte[i+2] == 'n' && texte[i+3] == 'd') || (texte[i] == 'r' && texte[i+1] == 'e' && texte[i+2] == 'c' && texte[i+3] == 'v'))
         {
            // Indice de début (cpMin) et un indice de fin (cpMax) de la sélection
            Selection.cpMin = i - lines;
            Selection.cpMax = Selection.cpMin+4; // 4 lettres > send | recv
            // On marque le mot avec sa couleur
            changerCouleurSelectionRichEdit(hwRichConsole, Selection, texte[i] == 's' ? SEND_COLOR : RECV_COLOR);
         }
         // error
         /*else if (texte[i] == 'e' && texte[i+1] == 'r' && texte[i+2] == 'r' && texte[i+3] == 'o' && texte[i+4] == 'r')
         {
            // Indice de début (cpMin) et un indice de fin (cpMax) de la sélection
            Selection.cpMin = i - lines;
            Selection.cpMax = Selection.cpMin+5; // 5 lettres > error
            // On marque le mot avec sa couleur
            changerCouleurSelectionRichEdit(hwRichConsole, Selection, ERROR_COLOR);
         }*/
         else if (texte[i] == '\n')
            lines++;
      }
      
      /* Restauration de la sélection */
      SendMessage(hwRichConsole, EM_EXSETSEL, 0, (LPARAM) &CurrentSelection);
   }
}

//=============================================================================
//                 Fonction d'ajout de client(s) à la listView
//
//=============================================================================

void ajouterALaListView(char *texte, int tailleTexte, char *ip)
{
   int i = 0, j = 0, itemPos;
   char user[100], machine[100];
   LVITEM lvi;
   
   ZeroMemory(&lvi, sizeof(LV_ITEM));
   lvi.mask = LVIF_TEXT | LVIF_IMAGE; //LVIF_PARAM
   
   while (texte[i] != '-' && i < tailleTexte) /* Si le caractère actuel n'est pas un '-' */
   {
      machine[j] = texte[i]; /* On récupère le nom de la machine */
      i++;
      j++;
   }
   machine[j] = '\0'; /* Fin du nom (machine) */
   j = 0;
   for (i++; i < tailleTexte; i++)
   {
      user[j] = texte[i]; /* On récupère le sujet du rappel */
      j++;
   }
   user[j] = '\0'; /* Fin du nom (machine) */
   
   /* On remplie la listView */
   
   /* user */
   if (lstrlen(user) <= 0) lstrcpy(user, "-");
   lvi.iItem = actualClientsNumber; /* Emplacement d'insersion (0 == tout en haut de la liste) */
   lvi.iSubItem = 0;
   //lvi.lParam = LVM_SORTITEMS;
   if (!strcmp(ip, "127.0.0.1")) // si c'est un client local
       lvi.iImage = 0;
   else
       lvi.iImage = 1;
   lvi.pszText = user;
   itemPos = ListView_InsertItem(hwClientsList, &lvi); /* Position actuelle dans la ListView */
   
   /* machine */
   if (lstrlen(machine) <= 0) lstrcpy(machine, "-");
   lvi.iItem = itemPos;
   lvi.iSubItem = 1;
   lvi.pszText = machine;
   ListView_SetItem(hwClientsList, &lvi);
   
   /* ip */
   if (lstrlen(ip) <= 0) lstrcpy(ip, "-");
   lvi.iItem = itemPos;
   lvi.iSubItem = 2;
   lvi.pszText = ip;
   ListView_SetItem(hwClientsList, &lvi);

   appendToRichConsole("ip", ip); // on sauvegarde et affiche l'ip aussi
   
   /* connexion time */
   char *time = get_time(2);
   lvi.iItem = itemPos;
   lvi.iSubItem = 3;
   lvi.pszText = time;
   ListView_SetItem(hwClientsList, &lvi);
   free(time);
   
   /* Réorganise la taille des colones. */
   //ListView_SetColumnWidth(hwClientsList, 0, LVSCW_AUTOSIZE);
   //ListView_SetColumnWidth(hwClientsList, 1, LVSCW_AUTOSIZE);
}

//=============================================================================
//                  Fonction qui récupère le temp actuel
//
//=============================================================================

char *get_time(short format)
{
   char *temp;
   temp = malloc(32);
   SYSTEMTIME Time;
   GetSystemTime(&Time);
   if (format == 2)
      sprintf(temp, "%02d:%02d:%02d", Time.wHour, Time.wMinute, Time.wSecond);
   else
      sprintf(temp, "[%02d-%02d-%d](%02d-%02d-%02d)", Time.wDay, Time.wMonth, Time.wYear, Time.wHour, Time.wMinute, Time.wSecond);
   return temp;
}

//=============================================================================
//     Fonction qui change la couleur de la selection dans l'edit spécifié
//
//=============================================================================

void changerCouleurRichEdit(HWND hEdit, COLORREF couleur)
{
   /* Format du texte */
   CHARFORMAT2 Format;
   ZeroMemory(&Format, sizeof(CHARFORMAT2));
   Format.cbSize = sizeof(CHARFORMAT2);
   Format.crTextColor = couleur; /* La couleur a utilisé */
   Format.dwEffects = CFE_BOLD; /* Gras */
   Format.dwMask = CFM_BOLD | CFM_COLOR;
   /* On indique au Rich Edit que l'on va utiliser le format : Format */
   SendMessage(hEdit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&Format);
}

//=============================================================================
//     Fonction qui change la couleur de la selection dans l'edit spécifié
//
//=============================================================================

void changerCouleurSelectionRichEdit(HWND hEdit, CHARRANGE Selection, COLORREF couleur)
{
   /* Formatage du texte */
   CHARFORMAT2 Format;
   ZeroMemory(&Format, sizeof(CHARFORMAT2));
   Format.cbSize = sizeof(CHARFORMAT2);
   Format.crTextColor = couleur; /* La couleur a utilisé */
   Format.dwMask = CFM_COLOR;	/* Le formatage va concerner la couleur du texte */
   /* On indique au Rich Edit que l'on va utiliser la sélection : Selection */
   SendMessage(hEdit, EM_EXSETSEL, 0, (LPARAM) &Selection);
   /* On indique au Rich Edit que l'on va utiliser le format : Format */
   SendMessage(hEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &Format);
}

//=============================================================================
//                Fonction qui vérifie la version de l'os
//
//=============================================================================

BOOL verifierOsWindowsXPorLater()
{
    OSVERSIONINFO osvi;
    BOOL bIsWindowsXPorLater;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);

    bIsWindowsXPorLater = ( (osvi.dwMajorVersion > 5) || ( (osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1) ));

    if(bIsWindowsXPorLater)
        return TRUE;
    else
       return FALSE;
}

//=============================================================================
//      Fonction qui affiche un ballon dans la TrayIcon (la notification)
//
//=============================================================================

void afficherTrayIconBallon(char *titre, char *contenu, unsigned int temp, BOOL icone)
{
   lstrcpy(TrayIcon.szInfoTitle, titre); /* Titre */
   lstrcpy(TrayIcon.szInfo, contenu); /* Contenu du ballon */
   TrayIcon.uTimeout = temp;  /* Temp d'attente */
   if (icone)
      TrayIcon.dwInfoFlags = NIIF_INFO;
   else
      TrayIcon.dwInfoFlags = NIIF_NONE;
   Shell_NotifyIcon(NIM_MODIFY, &TrayIcon); /* Modification */
}

//=============================================================================
//       Fonction qui récupère le nom du programme depuis son chemin
//
//=============================================================================

char *recupNomDuProgramme(char *path)
{
   char *nomDuProgramme, nomInverser[200];
   nomDuProgramme = malloc(strlen(path));

   /* On récupère le nom (inversé) */
   int i, j;
   for (i = 0, j = strlen(path) - 1; j >= 0; i++, j--)
   {
      if (path[j] != '\\')
         nomInverser[i] = path[j];
      else
         break;
   }
   nomInverser[i] = '\0';
   /* On inverse le nom */
   for (i = 0, j = strlen(nomInverser) - 1; j >= 0; i++, j--)
   {
       nomDuProgramme[i] = nomInverser[j];
   }
   nomDuProgramme[i] = '\0';
   
   return nomDuProgramme;
}

//=============================================================================
//              Fonction qui crée les colones de la ListView
//=============================================================================

void creerListViewColone(HANDLE htmp, UINT largeurColone, char *nomColone, UINT idColone)
{
   LVCOLUMN lvc;
   lvc.mask = LVCF_TEXT | LVCF_WIDTH;
   lvc.cx = largeurColone;
   lvc.pszText = nomColone;
   ListView_InsertColumn(htmp, idColone, &lvc);         
}

//=============================================================================
//          Fonction de remplissage de la liste des commandes
//
//=============================================================================

void remplirListeDesCommandes(HWND hListe)
{
   int i, itemPos;
   char cmdNbr[3];
   
   for (i = 0; i < (sizeof cmd / sizeof *cmd); i++)
   {
      LVITEM lvi;
      ZeroMemory(&lvi, sizeof(LV_ITEM));
      lvi.mask = LVIF_TEXT | LVIF_PARAM;
      lvi.iItem = i;
      lvi.lParam = LVM_SORTITEMS;
      
      itemPos = ListView_InsertItem(hListe, &lvi); /* Position actuelle dans la ListView */
      
      sprintf(cmdNbr, "%d", i + 1);
      ListView_SetItemText(hListe, itemPos, 0, cmdNbr); /* Numéro de commande */
      ListView_SetItemText(hListe, itemPos, 1, cmd[i].nom); /* cmdPrefixe/nom */
      ListView_SetItemText(hListe, itemPos, 2, cmd[i].param1); /* param1 */
      ListView_SetItemText(hListe, itemPos, 3, cmd[i].param2); /* param2 */
      ListView_SetItemText(hListe, itemPos, 4, cmd[i].param3); /* param3 */
      ListView_SetItemText(hListe, itemPos, 5, cmd[i].param4); /* param4 */
      ListView_SetItemText(hListe, itemPos, 6, cmd[i].param5); /* param5 */
      ListView_SetItemText(hListe, itemPos, 7, cmd[i].param6); /* param6 */
   }
}

//=============================================================================
//    Fonction qui se charge de vérifier l'existence du fichier spécifié 
//
//=============================================================================

BOOL verifierExistenceFichier(char *path)
{
   FILE *Fichier = NULL;
   
   Fichier = fopen(path, "r");
   
   if (Fichier != NULL)
   {
      fclose(Fichier);
      return TRUE;
   }
   else
      return FALSE;     
}

//=============================================================================
//               Fonction qui crée des tooltips/bules d'infos
//=============================================================================

void createTooltipFor(HWND hButton, HWND hwnd, HWND hTTip, char *contenu)
{ 
   RECT rect;
   TOOLINFO ti;
   
   GetClientRect(hwnd, &rect); 
   ti.cbSize = sizeof(TOOLINFO); 
   ti.uFlags = TTF_SUBCLASS; 
   ti.hwnd = hButton;
   ti.hinst = hInst; 
   ti.uId = 0; 
   ti.lpszText = contenu; 
   ti.rect.left = rect.left; 
   ti.rect.top = rect.top; 
   ti.rect.right = rect.right; 
   ti.rect.bottom = rect.bottom; 
   SendMessage(hTTip, TTM_ADDTOOL, 0, (LPARAM) &ti);
}

//=============================================================================
// Fonction qui retourne TRUE si la chaine passé en parametre est numérique, si nn FALSE
//=============================================================================

BOOL isFullyNumeric(char *string)
{
    BOOL isNumeric = TRUE; // on suppose que la chaine contient/est un nombre numérique
    int i;
    
    // on parcourt la chaine
    for (i = 0; i < strlen(string); i++)
    {
        // si on trouve un seul caractère non numérique
        if (string[i] != '0' && string[i] != '1' && string[i] != '2' && string[i] != '3' && string[i] != '4' && string[i] != '5' &&
            string[i] != '6' && string[i] != '7' && string[i] != '8' && string[i] != '9')
        {
           isNumeric = FALSE; // la chaine n'est pas numérique
           break; // on sort de la boucle
        }
    }
    
    return isNumeric;
}

//=============================================================================
//      Fonction qui renvoie l'extension du fichier passé en parametre
//
// @ si c'est un dossier la valeur "" est affecté à ptExt
//=============================================================================

void getExtension(char *f_name_with_path_or_not, char *ptExt)
{
     char f_name[200];
     
     // si le nom du fichier fourni contient son chemin aussi, on extrait le nom
     if (strstr(f_name_with_path_or_not, "\\") != NULL)
     {
         char *name = recupNomDuProgramme(f_name_with_path_or_not); // extraction du nom du fichier
         lstrcpy(f_name, name); // affectation du nouveau nom
         free(name); // libération du pointeur name
     }
     else // si nn, le nom du fichier fourni ne contient pas son chemin
         lstrcpy(f_name, f_name_with_path_or_not); // affectation du nouveau nom
     
     // si c'est un dossier (son nom ne contient aucun point)
     // @@ il se peut qu'un nom de dossier contient un point, mais ce n'est pas un problème, on traite ce cas après en bas
     // @@ le dernier petit conflit qui reste içi, c'est que si le fichier n'a pas d'extension, il sera considéré comme un dossier
     // , et on peux rien y faire, à part reconstruire tout le système qu'on vien de bâtir, et c'est pas de nôtre faute, mais pour
     // s'assurer que c'est un fichier non pas un dossier c'est simple, on ouvre la fenêtre d'informations et on trouvera qu'il a bien
     // une taille (nôtre fichier masqué en dossier) :pp, et si jamais vous voulez le télécharger utilisez la ligne de commande ;))
     if (strstr(f_name, ".") == NULL)
     {
         lstrcpy(ptExt, ""); // on affecte une chaine vide
         return; // on sort de la fonction
     }
     
     // si nn
     int i, j = 0;
     char extension[6] = ""; // extension .xxxxxx maximum, 6 caractère maxi (osef du point, on ne l'inclus pas)
     
     // on récupère l'extension
     for (i = strlen(f_name) - 1; i >=0 && j < 6; i--)
     {
        if (f_name[i] == '.')
           break; // on sort de la boucle
        else
           extension[j] = f_name[i];
           
        j++;
     }
     
     extension[j] = 0; // ou '\0', fin de chaine
     
     if (i < 0) i = 0; // si on a parcouru tout le nom (f_name), i sera == -1 (il ne faut pas utiliser un indice de tableau négatif)
     // si on n'a pas trouvé le point qui indique que c'est une extension, c'est plutôt un dossier alors
     // ou bien si l'extension ne contient que des chiffres, ce n'est pas une extension alors
     // c'est plutot un nom de dossier qui contient un/des points
     if (f_name[i] != '.' || isFullyNumeric(extension))
     {
         lstrcpy(ptExt, ""); // on affecte une chaine vide
         return; // on sort de la fonction
     }
     
     // maintenant on doit invérssé ptExt car l'extension a été écrite à l'envers
     // et c'est normal, on a commencé de la fin quand on a parcouru f_name
     i = 0;
     for (j = strlen(extension) - 1; j >= 0; j--)
     {
         ptExt[i] = extension[j];
         i++;
     }
     
     ptExt[i] = 0; // fin de chaine
}

//=============================================================================
// Fonction qui renvoie l'indice de l'exploreType du fichier/dossier passé en parametre
//=============================================================================

unsigned short getExploreType(char *f_name, BOOL isDir)
{
    int i, j = 0;
    
    char ext[6] = ""; // si c'est un dossier => ext == ""
    
    // si ce n'est pas un dossier
    if (! isDir)
    {
        // on récupère l'extension
        getExtension(f_name, ext);
    }
    
    // @ si c'est un dossier ext sera == "" (à une chaine vide)
    // et ça coincide exactement avec ce qu'on a dans le tableau exploreTypes
    
    // on parcourt le tableau exploreTypes
    for (i = 0; i < (sizeof exploreTypes / sizeof *exploreTypes); i++)
    {
        if (!strcmp(ext, exploreTypes[i].extension)) // si on identifie l'extension
           return i;
    }
    
    // si on arrive içi c'est que l'extension n'a pa été identifié => c'est un fichier inconnu alors, on renvoie son indice
    return 1;
}

//=============================================================================
// Fonction qui retourne l'id de l'icône du type de fichier passé en parametre
//=============================================================================

int getExploreIcon(char *description)
{
    int i;
    
    // on parcourt le tableau exploreTypes
    for (i = 0; i < (sizeof exploreTypes / sizeof *exploreTypes); i++)
    {
        if (!strcmp(description, exploreTypes[i].description)) // si on identifie le type/description
           return exploreTypes[i].iconeId;
    }
    
    // si jamais on ne trouve pas le type (même si ça ne devrais pas arriver) => on renvoie l'icone d'un fichier inconnu alors
    return exploreTypes[1].iconeId;
}

//=============================================================================
//                 Fonction d'ajout à l'explore listView
//
//=============================================================================

void addToExploreListView(char *nom, int itemPos, BOOL isDir)
{
   LVITEM lvi;
   ZeroMemory(&lvi, sizeof(LV_ITEM));
   lvi.mask = LVIF_TEXT | LVIF_IMAGE; //LVIF_PARAM
   /* Création de l'image liste */
   HICON hIcon;
   HIMAGELIST hExploreImgList = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 , 14, 14); // 14 == max icones
   ImageList_SetBkColor(hExploreImgList, GetSysColor(COLOR_WINDOW));
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DRIVE));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_FOLDER));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_FILE));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_TEXT_FILE));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_IMAGE));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SON));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_VIDEO));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_EXE));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DOC_WORD));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DOC_EXCEL));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DOC_POWERPOINT));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DOC_ACCESS));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ZIP_RAR));
   ImageList_AddIcon(hExploreImgList, hIcon);
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PDF));
   ImageList_AddIcon(hExploreImgList, hIcon);
   // @@ si vous ajoutez des icônes à l'imageList, changez le nombre max d'image en haut (6ème ligne après l'accolade de la fonction)
   // on lui associe l'image liste
   ListView_SetImageList(hwListeExploration, hExploreImgList, LVSIL_SMALL);
   
   /* nom */
   int tailleNom = strlen(nom);
   short indice = -1;
   
   lvi.iItem = itemPos; /* Emplacement d'insersion (0 == tout en haut de la liste) */
   lvi.iSubItem = 0;
   if (nom[tailleNom - 1] == '\\') // si c'est un drive/partition
      lvi.iImage = 0;
   else // c'est un fichier ou dossier alors
   {
      // on récupère l'exploreType (l'indice de l'extension "qu'on va extraire du nom" dans le tableau exploreTypes)
      indice = getExploreType(nom, isDir);
      
      if (exploreTypes[indice].iconeId == IDI_FOLDER)
         lvi.iImage = 1; // dossier => indice 1
      else if (exploreTypes[indice].iconeId == IDI_FILE)
         lvi.iImage = 2; // fichier (inconnu) => indice 2
      else if (exploreTypes[indice].iconeId == IDI_TEXT_FILE)
         lvi.iImage = 3; // fichier texte => indice 3
      else if (exploreTypes[indice].iconeId == IDI_IMAGE)
         lvi.iImage = 4; // image => indice 4
      else if (exploreTypes[indice].iconeId == IDI_SON)
         lvi.iImage = 5; // son => indice 5
      else if (exploreTypes[indice].iconeId == IDI_VIDEO)
         lvi.iImage = 6; // vidéo => indice 6
      else if (exploreTypes[indice].iconeId == IDI_EXE)
         lvi.iImage = 7; // exé => indice 7
      else if (exploreTypes[indice].iconeId == IDI_DOC_WORD)
         lvi.iImage = 8; // exé => indice 8
      else if (exploreTypes[indice].iconeId == IDI_DOC_EXCEL)
         lvi.iImage = 9; // exé => indice 9
      else if (exploreTypes[indice].iconeId == IDI_DOC_POWERPOINT)
         lvi.iImage = 10; // exé => indice 10
      else if (exploreTypes[indice].iconeId == IDI_DOC_ACCESS)
         lvi.iImage = 11; // exé => indice 11
      else if (exploreTypes[indice].iconeId == IDI_ZIP_RAR)
         lvi.iImage = 12; // exé => indice 12
      else if (exploreTypes[indice].iconeId == IDI_PDF)
         lvi.iImage = 13; // exé => indice 13
   }
   lvi.pszText = nom;
   ListView_InsertItem(hwListeExploration, &lvi);
   
   /* type */
   lvi.iItem = itemPos;
   lvi.iSubItem = 1;
   lvi.pszText = indice == -1 ? "Partition" : exploreTypes[indice].description;
   ListView_SetItem(hwListeExploration, &lvi);
   
   /* extension */
   char ext[6] = ""; // si c'est une partition => ext == ""
   lvi.iItem = itemPos;
   lvi.iSubItem = 2;
   if (indice != -1)
   {
      // si c'est un fichier inconnu (?)
      if (! lstrcmp(exploreTypes[indice].extension, "?"))
         getExtension(nom, ext); // on récupère son extension réelle
      else
         lstrcpy(ext, exploreTypes[indice].extension);
   }
   lvi.pszText = ext;
   ListView_SetItem(hwListeExploration, &lvi);
}

//=============================================================================
//        Fonction qui découpe le résultat selon le separateur donné
//                  et le rajoute dans l'exploreListView
//=============================================================================

BOOL setExploreResult(char *result, char separateur, BOOL isDrive)
{
   int len = strlen(result);
   /*
   if (len == 0)
      return FALSE;
   */
   
   int i, j = 0, count = 0;
   char nom[256];
   
   // on vide la listeView (si elle est déjà vide aucun prob.)
   ListView_DeleteAllItems(hwListeExploration);
   
   // on découpe le résultat
   for (i = 0; i < len; i++)
   {
       if (result[i] == separateur)
       {
           nom[j] = 0; // ou '\0'
           if (isDrive)
           {
               addToExploreListView(nom, count, FALSE);
           }
           else
           {
               addToExploreListView(nom, count, result[i+1] == '1' ? TRUE : FALSE);
               i += 2; // on saute les caractères : 0| ou 1|
           }
           j = 0; // pour un nouveau nom
           count++;
       }
       else
       {
           nom[j] = result[i];
           j++;
       }
   }
   
   // On affiche le nombre de résultat sur la StatusBar
   sprintf(nom, "%d élément(s) trouvé(s)", count); // on utilise 'nom' temporairement içi
   SetWindowText(hwExploreStatusBar, nom);
   
   if (count > 0)
      return TRUE;
   else
      return FALSE;
}

//=============================================================================
//            Fonction qui retourne la taille du fichier spécifé
//=============================================================================

long tailleFichier(char *fichier)
{
   HANDLE hFichier;
   DWORD FileSize;
   
   hFichier = CreateFile(fichier, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if(hFichier != INVALID_HANDLE_VALUE) /* Si fichier trouvé */
   {
       FileSize = GetFileSize(hFichier, NULL);
       CloseHandle(hFichier);
       return FileSize;
   }
   else
       return 0;
}

//=============================================================================
//  Fonction qui se charge de vérifier l'existence et lire le fichier spécifié 
//=============================================================================

BOOL lireFichier(char *fichier, char *buffer)
{
   HANDLE hFichier;
   DWORD FileSize, nbcharRead;
   //CHAR *buffer;
   
   hFichier = CreateFile(fichier, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if(hFichier != INVALID_HANDLE_VALUE) /* Si fichier trouvé */
   {
      FileSize = GetFileSize(hFichier, NULL);
      //buffer = (PCHAR)LocalAlloc(LMEM_FIXED, FileSize+1);
      ReadFile(hFichier, buffer, FileSize, &nbcharRead, NULL) ; /* Lecture */
      buffer[FileSize] = 0;
      //LocalFree(buffer);
      CloseHandle(hFichier);
      return TRUE;
   }
   else
      return FALSE;
}

//=============================================================================
//     Fonction qui tranche la dernière partie d'un path à chaque appel 
//=============================================================================

BOOL pathBack(char *path)
{
     int i, pathLen = strlen(path);
     
     for (i = pathLen - 1; i >= 0; i--)
     {
         if (path[i] == '\\')
         {
            // si 'C:\\xxxxxx' ou 'C:\\'
            if (path[i - 1] == ':')
            {
               if (i == pathLen - 1) // si juste 'C:\\'
                  return FALSE; // on ne peux rien trancher (pour ne pas avoir d'erreur)
               else // si nn si 'C:\\xxxxxx'
                  path[i + 1] = 0; // on enlève juste les 'xxxxxx'
            }
            else // si nn si 'C:\\xxxx\\xxxxx'
               path[i] = 0; // ou '\0' (on tranche le path, on enlève les derniers '\\xxxxx')
            
            return TRUE;
         }
     }
     
     return FALSE;
}

//=============================================================================
//      Fonction qui change la couleur des lignes/rows d'une listView
//=============================================================================

BOOL customdraw_handler(HWND hwnd, WPARAM wParam, LPARAM lParam, COLORREF color)
{
    LPNMLISTVIEW pnm = (LPNMLISTVIEW)lParam;

    switch (pnm->hdr.code)
    {
        case NM_CUSTOMDRAW:
        {
            LPNMLVCUSTOMDRAW  lplvcd;
            lplvcd = (LPNMLVCUSTOMDRAW)lParam;

            switch(lplvcd->nmcd.dwDrawStage)
            {
                case CDDS_PREPAINT :
                        SetWindowLong(hwnd, DWL_MSGRESULT, CDRF_NOTIFYITEMDRAW); // Comment this line out if this is not a dialog box
                        return CDRF_NOTIFYITEMDRAW;
                    break;
                case CDDS_ITEMPREPAINT:
                    {
                        int row; 
                        row = lplvcd->nmcd.dwItemSpec;
               
                        if((row + 1) % 2 == 0)
                            lplvcd->clrTextBk = color; // change the text background color
                    
                        return CDRF_NEWFONT;
                    }
                    break;
            } // fin 2eme switch
        }
        break;
    }
    
return FALSE;
}

//=============================================================================
//                 Fonction d'ajout à la tasks listView
//
//=============================================================================

void addToTasksListView(char *task, int itemPos)
{
   LVITEM lvi;
   ZeroMemory(&lvi, sizeof(LV_ITEM));
   lvi.mask = LVIF_TEXT | LVIF_IMAGE; //LVIF_PARAM
   /* Création de l'image liste */
   HICON hIcon;
   HIMAGELIST hTasksImgList = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 , 1, 1); // 1 == max icones
   ImageList_SetBkColor(hTasksImgList, GetSysColor(COLOR_WINDOW));
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_EXE));
   ImageList_AddIcon(hTasksImgList, hIcon);
   // on lui associe l'image liste
   ListView_SetImageList(hwTaskList, hTasksImgList, LVSIL_SMALL);
   
   /* task */
   lvi.iItem = itemPos; // Emplacement d'insersion (0 == tout en haut de la liste)
   lvi.iSubItem = 0;
   lvi.iImage = 0;
   lvi.pszText = task;
   ListView_InsertItem(hwTaskList, &lvi);
   
   /* type */
   char type[26] = "-";
   
   if (!lstrcmp(task, "System") || !lstrcmp(task, "svchost.exe") ||
       !lstrcmp(task, "explorer.exe") || !lstrcmp(task, "taskhost.exe") ||
       !lstrcmp(task, "taskmgr.exe") || !lstrcmp(task, "winlogon.exe") ||
       !lstrcmp(task, "dwm.exe"))
       lstrcpy(type, "System");
   
   lvi.iItem = itemPos;
   lvi.iSubItem = 1;
   lvi.pszText = type;
   ListView_SetItem(hwTaskList, &lvi);
}

//=============================================================================
//        Fonction qui découpe le résultat selon le separateur donné
//                  et le rajoute dans la TasksListView
//=============================================================================

BOOL setTasksResult(char *result, char separateur)
{
   int len = strlen(result);
   /*
   if (len == 0)
      return FALSE;
   */
   
   int i, j = 0, count = 0;
   char task[256];
   
   // on vide la listeView (si elle est déjà vide aucun prob.)
   ListView_DeleteAllItems(hwTaskList);
   
   // on découpe le résultat
   for (i = 0; i < len; i++)
   {
       if (result[i] == separateur)
       {
           task[j] = 0; // ou '\0'
           addToTasksListView(task, count);
           j = 0; // pour un nouveau nom
           count++;
       }
       else
       {
           task[j] = result[i];
           j++;
       }
   }
   
   // On affiche le nombre de résultat sur la StatusBar
   sprintf(task, "%d task(s) trouvé(s)", count); // on utilise 'task' temporairement içi
   SetWindowText(hwTaskListStatusBar, task);
   
   if (count > 0)
      return TRUE;
   else
      return FALSE;
}

//=============================================================================
//     Fonction qui retourne l'index du client dont l'id est spécifié
//
// @ retourne -1 si client non trouvé
//=============================================================================

int getClientIndexById(int clientId)
{
   int i;
   
   for (i = 0; i < actualClientsNumber; i++)
   {
       if (clients[i].id == clientId) // si notre client est toujours connecté on trouvera son id
           return i; // on retourne l'index
   }
       
   // si nn c vraiment notre client qui s'est déconnecté
   return -1;
}

//=============================================================================
//     Fonction qui met à jour l'index du client dont l'id est spécifié
//=============================================================================

int updateClientIndexById(int clientId, int currentClientIndex, int *oldClientsNumber)
{
   /*
   // Si un client vient de se déconnecter (le nombre de clients à changer)
   if ((*oldClientsNumber) > actualClientsNumber)
   {
      (*oldClientsNumber) = actualClientsNumber; // on met à jour le nombre de clients aussi
      return getClientIndexById(clientId);
   }
   else if ((*oldClientsNumber) < actualClientsNumber) // Si nn si un client vient de se connecter
   {
       (*oldClientsNumber) = actualClientsNumber; // on met à jour le nombre de clients
       return currentClientIndex;
   }
   else // si nn si le nombre de clients n'a pas changé
      return currentClientIndex;
   */
   
   // par contre dans le else, si jamais le client déco et qu'un autre reco sur place ou lui même 
   // avant d'appeler cette fonction/avant de faire l'update, l'ancien nombre de client sera
   // égal au nombre de client actuel et du coup on aura un faux id, c vrai que l'index pourra
   // être juste si on a un seul client mais l'id nan, ce qui va générer une erreur au prochain update.
   // De toute façon la possibilité que ça arrive n'est pas trop grande, et puis la plus simple des solutions
   // serait d'utiliser un return getClientIndexById(clientId); au lieu de tout ce code, ce qui veux dire
   // que cette fonction ne sera plus nécessaire, on pourra la supprimer, mais bon, j'ai pas envie de faire ça
   // pour l'instant, si jamais quelqu'un lit ce que j'ai écrit, garde en tête cette possibilité, voila ;))
   
   return getClientIndexById(clientId);
}

//=============================================================================
//     Fonction qui retourne l'index du client dont le handle est spécifié
//
// @ retourne -1 si client non trouvé
//=============================================================================

int getClientIndexByShowScreenWindowHwnd(HWND hwnd)
{
    int i;
   
    for (i = 0; i < actualClientsNumber; i++)
    {
        if (clients[i].show_screen_infos.hwnd == hwnd) // si on trouve le handle
            return i; // on retourne l'index
    }
    
    // si nn si le client s'est déconnecté
    return -1;
}

//=============================================================================
//                   Fonction d'exploration des fichiers
//=============================================================================

BOOL browseForFile(HWND hwnd, char *title, char *selectedFilePath)
{
   OPENFILENAME ofn;
   CHAR szFile[MAX_PATH]={0};

   ZeroMemory(&ofn, sizeof(OPENFILENAME));
   ofn.lStructSize = sizeof(OPENFILENAME);
   ofn.hwndOwner = hwnd;
   ofn.lpstrFile = szFile;
   ofn.nMaxFile = MAX_PATH;
   ofn.lpstrFilter = "Tout les fichiers (*.*)\0*.*\0";
   ofn.nFilterIndex = 1;
   ofn.lpstrInitialDir = APP_PATH;
   ofn.lpstrTitle = title;
   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

   if (GetOpenFileName(&ofn) == TRUE)
   {
      lstrcpy(selectedFilePath, szFile);
      SetCurrentDirectory(APP_PATH); // on rend le répertoire par défaut à celui de départ (pour éviter que les fichiers reçus soit mit ailleur)
      return TRUE;
   }
   
   return FALSE;
}

//=============================================================================
//        Fonction qui découpe le résultat selon le separateur donné
//          et le rajoute au control de la fenêtre Update Config
//=============================================================================

BOOL setConfigResult(char *result, char separateur)
{
   int len = strlen(result);
   /*
   if (len == 0)
      return FALSE;
   */
   
   int i, j = 0, count = 0;
   char parametre[256];
   
   // on découpe le résultat
   for (i = 0; i <= len; i++)
   {
       if (result[i] == separateur || i == len)
       {
           parametre[j] = 0; // ou '\0'
           j = 0; // pour un nouveau nom
           i++; // pour dépasser le '\n' qui vient après un '\r'
           
           // On remplie les controls de la fenêtre
           switch (count)
           {
               case 0:
                    SetWindowText(GetDlgItem(hwFenUpdateConfig, IDE_EMAIL), parametre);
                    break;
               case 1:
                    SetWindowText(GetDlgItem(hwFenUpdateConfig, IDE_SERVEUR), parametre);
                    break;
               case 2:
                    SetWindowText(GetDlgItem(hwFenUpdateConfig, IDE_PORT), parametre);
                    break;
               case 3:
                    SetWindowText(GetDlgItem(hwFenUpdateConfig, IDE_TEMP_RECEPTION), parametre);
                    break;
               case 4:
                    SetWindowText(GetDlgItem(hwFenUpdateConfig, IDE_CURRENT_DIR), parametre);
                    break;
               case 5:
                    SetWindowText(GetDlgItem(hwFenUpdateConfig, IDE_TASK_NAME), parametre);
                    break;
               case 6:
                    if (parametre[0] == '1')
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_RUN_ON_BOOT), CB_SETCURSEL, 0, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_RUN_ON_BOOT), 1);
                       // 1 içi est non 0 (le vrai indice) car Run On Boot n'est pas précédé d'un Stop qui indique la négation comme
                       // les autres options qui suivent
                    }
                    else
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_RUN_ON_BOOT), CB_SETCURSEL, 1, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_RUN_ON_BOOT), 0);
                    }
                    break;
               case 7:
                    if (parametre[0] == '1')
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_STOP_ESPION), CB_SETCURSEL, 0, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_STOP_ESPION), 0);
                    }
                    else
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_STOP_ESPION), CB_SETCURSEL, 1, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_STOP_ESPION), 1);
                    }
                    break;
               case 8:
                    if (parametre[0] == '1')
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_STOP_SENDER), CB_SETCURSEL, 0, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_STOP_SENDER), 0);
                    }
                    else
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_STOP_SENDER), CB_SETCURSEL, 1, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_STOP_SENDER), 1);
                    }
                    break;
               case 9:
                    if (parametre[0] == '1')
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_STOP_ESPIONMDP), CB_SETCURSEL, 0, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_STOP_ESPIONMDP), 0);
                    }
                    else
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_STOP_ESPIONMDP), CB_SETCURSEL, 1, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_STOP_ESPIONMDP), 1);
                    }
                    break;
               case 10:
                    if (parametre[0] == '1')
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_STOP_CLIENT), CB_SETCURSEL, 0, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_STOP_CLIENT), 0);
                    }
                    else
                    {
                       SendMessage(GetDlgItem(hwFenUpdateConfig, IDC_STOP_CLIENT), CB_SETCURSEL, 1, 0);
                       setComboBoxIcon(GetDlgItem(hwFenUpdateConfig, IDI_STOP_CLIENT), 1);
                    }
                    break;
               case 11:
                    SetWindowText(GetDlgItem(hwFenUpdateConfig, IDE_VERSION), parametre);
                    break;       
           }
           count++;
       }
       else
       {
           parametre[j] = result[i];
           j++;
       }
   }
   
   if (count > 0)
      return TRUE;
   else
      return FALSE;
}


//=============================================================================
// Fonction qui affiche l'image correspandante à la séléction de la combobox spécifié
//=============================================================================

BOOL setComboBoxIcon(HWND hwComboBox, int currentSel)
{
    if (currentSel == 1)
    {
       SendMessage(hwComboBox, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_TRUE), IMAGE_ICON, 12, 12, 0));
       return TRUE;
    }
    else
    {
       SendMessage(hwComboBox, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_FALSE), IMAGE_ICON, 12, 12, 0));
       return FALSE;
    }
}

//=============================================================================
// Fonction qui convertie une taille/size en chaine de caractère significatif
//=============================================================================

void sizeToString(long size, char *buffer)
{
    char stringSize[100];
    
    if (size / 1024 > 0)
    {
        if (size / 1048576 > 0)
            sprintf(stringSize, "%.2f Mo", (double)size / 1048576);
        else
            sprintf(stringSize, "%.2f Ko", (double)size / 1024);
    }
    else
        sprintf(stringSize, "%d byte", size); // byte en anglais == octet
        
    lstrcpy(buffer, stringSize);
}

//=============================================================================
//   Fonction qui retourne le pourcentage de progression (p = x * 100 / max)
//=============================================================================

int getIntProgress(long totalSize, ssize_t currentSize)
{
    return (double)currentSize * 100 / totalSize;
}

//=============================================================================
//   Fonction qui calcule la différence entre 2 size donnée et retourne TRUE si la diff. == 1 Mo
//=============================================================================

BOOL diffMoreThanOneMegabits(ssize_t currentSize, ssize_t sizeSave)
{
    // if the difference equal or more than 1 mégabits
    if ((currentSize - sizeSave) / 1048576 >= 1) // conversion octet/byte => Mo
       return TRUE;
    else
       return FALSE;
}

//=============================================================================
// Fonction qui enlève le nom du program et son extension pour extraire son path
//=============================================================================

void extractPath(char *path)
{
     int i, pathLen = strlen(path);
     
     for (i = pathLen - 1; i >= 0; i--)
     {
         if (path[i] == '\\')
         {
             path[i] = 0; // ou '\0' (on tranche le path, on enlève les derniers '\\xxxxx.exe')
             return;
         }
     }
}

//=============================================================================
// Fonction qui active/désactive les controls/buttons passé en tableau selon la valeur spécifiée
//=============================================================================

void enableDisableButtons(HWND hwTable[], unsigned short tableSize, BOOL value) // ou bien HWND *hwTable
{
     int i;
     
     for (i = 0; i < tableSize; i++)
         EnableWindow(hwTable[i], value);
}

//=============================================================================
// Fonction qui retourne TRUE si le boutton n'est pas le boutton Delete, si nn FALSE
//=============================================================================

BOOL isNotDelete(int btnId)
{
     if (btnId == IDB_NEW_FOLDER || btnId == IDB_RENAME || btnId == IDB_COPY || btnId == IDB_CUT || btnId == IDB_PASTE)
         return TRUE;
     else
         return FALSE;
}

//=============================================================================
//        Fonction qui découpe le résultat selon le separateur donné
//              et le rajoute dans la fenêtre d'informations
//=============================================================================

BOOL setInformationsResult(char *result, char separateur, char premierSeparateur)
{
   int len = strlen(result);
   /*
   if (len == 0)
      return FALSE;
   */
   
   int i = 0, j = 0, count = 0;
   char param[100];
   
   // on dépasse le premier séparateur
   while (result[i] != premierSeparateur)
       i++;
   
   // on découpe le résultat
   i++; // pour sauter/commencer après le premier séparateur
   for (i; i <= len; i++)
   {
       if (i == len || result[i] == separateur)
       {
           param[j] = 0; // ou '\0'
           // Traitement des parametres
           switch (count)
           {
               case 0: // Taille
                 {
                    long f_size = atol(param);
                    if (f_size != -1) // si ce n'est pas : un dossier ou bien le chemin est introuvable
                    {
                       sizeToString(f_size, param);
                       SetDlgItemText(hwFenInformations, TXT_INFO_F_SIZE, param);
                    }
                 }
                    break;
               case 1: // Attribut System
                    if (param[0] == '1')
                       CheckDlgButton(hwFenInformations, IDC_INFO_F_SYSTEM, BST_CHECKED);
                    break;
               case 2: // Attribut Caché
                    if (param[0] == '1')
                       CheckDlgButton(hwFenInformations, IDC_INFO_F_HIDDEN, BST_CHECKED);
                    break;
           }   
           j = 0; // pour un nouveau parametre
           count++;
       }
       else
       {
           param[j] = result[i];
           j++;
       }
   }
   
   if (count > 0)
      return TRUE;
   else
      return FALSE;
}

//=============================================================================
// Fonction qui vérifie le client spécifié et affiche les messages d'erreur appropriés
//=============================================================================

BOOL checkClient(HWND hDlg, int *clientIndex, int clientId, BOOL updateClientIndex, BOOL checkSendRecvFileToo, int idButton)
{
    // si on doit mettre à jour l'index du client
    if (updateClientIndex)
    {
        // On met à jour l'index de notre client, @@ car la fenêtre/ou un MessageBox peut rester ouvert longtemp, et le client pourra déco pendant ce temp
        //clientIndex = updateClientIndexById(clientId, clientIndex, &clientsNumberSave);
        (*clientIndex) = getClientIndexById(clientId);
        
        // S'il n'y a plus de client dont on est entrain d'explorer le drive (fichiers/dossiers)
        if ((*clientIndex) == -1)
        {
            MessageBox(hDlg, "Le client s'est déconnecté !", NOM_APP, MB_OK | MB_ICONWARNING);
            /* Activation du boutton associé à la fenêtre à la fermeture */
            if (idButton > 0) EnableWindow(GetDlgItem(hwFenetre, idButton), TRUE);
            EndDialog(hDlg, 0); // fermeture de la fenêtre 
            return FALSE;
        }
    }
    
    // si on doit vérifier si le client est en train d'envoyer ou recevoir un fichier
    if (checkSendRecvFileToo)
    {
       // vérifications
       if (clients[(*clientIndex)].send_file)
       {
           MessageBox(hDlg, "Le client est en train d'envoyer un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
           return FALSE;
       }
       else if (clients[(*clientIndex)].recv_file)
       {
           MessageBox(hDlg, "Le client reçoit un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
           return FALSE;
       }
    }
    
    return TRUE; // aucune erreur lors des vérifications
}

//=============================================================================
//                 Fonction d'ajout à la ToDo listView
//
//=============================================================================

void addToToDoListView(char *id, char *commmande, char *time, int itemPos)
{
   LVITEM lvi;
   ZeroMemory(&lvi, sizeof(LV_ITEM));
   lvi.mask = LVIF_TEXT | LVIF_IMAGE; //LVIF_PARAM
   /* Création de l'image liste */
   HICON hIcon;
   HIMAGELIST hToDoImgList = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 , 1, 1); // 1 == max icones
   ImageList_SetBkColor(hToDoImgList, GetSysColor(COLOR_WINDOW));
   hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_TODO));
   ImageList_AddIcon(hToDoImgList, hIcon);
   // on lui associe l'image liste
   ListView_SetImageList(hwToDoList, hToDoImgList, LVSIL_SMALL);
   
   /* id */
   lvi.iItem = itemPos; // Emplacement d'insersion (0 == tout en haut de la liste)
   lvi.iSubItem = 0;
   lvi.iImage = 0;
   lvi.pszText = id;
   ListView_InsertItem(hwToDoList, &lvi);
   
   /* commande */
   lvi.iItem = itemPos;
   lvi.iSubItem = 1;
   lvi.pszText = commmande;
   ListView_SetItem(hwToDoList, &lvi);
   
   /* time */
   lvi.iItem = itemPos;
   lvi.iSubItem = 2;
   lvi.pszText = time;
   ListView_SetItem(hwToDoList, &lvi);
}

//=============================================================================
//        Fonction qui découpe le résultat selon le separateur donné
//                  et le rajoute dans la ToDoListView
//=============================================================================

BOOL setToDoResult(char *result, char separateur)
{
   int len = strlen(result);
   /*
   if (len == 0)
      return FALSE;
   */
   
   int i, j = 0, count = 0;
   char id[10];
   char commande[BUF_SIZE];
   char time[10];
   BOOL GET_ID = FALSE, GET_TIME = FALSE, GET_COMMANDE = FALSE;
   
   // on vide la listeView (si elle est déjà vide aucun prob.)
   ListView_DeleteAllItems(hwToDoList);
   
   // on découpe le résultat
   for (i = 0; i <= len; i++)
   {
       // si c'est le début (séparateur d'id '[') ou la fin
       if (result[i] == '[' || i == len)
       {
           if (GET_COMMANDE)
           {
               GET_COMMANDE = FALSE;
               commande[j] = 0; // ou '\0'
               addToToDoListView(id, commande, time, count);
               j = 0;
               count++;
           }
           
           GET_ID = TRUE;
       }
       // si nn, si c'est la fin de l'id ']'
       else if (result[i] == ']' && GET_ID) // && GET_ID == TRUE)
       {
           id[j] = 0; // ou '\0'
           j = 0;
           GET_ID = FALSE;
           GET_TIME = TRUE;
       }
       // si nn, si c'est la fin du temps restant avant exécution/time
       else if (result[i] == separateur && GET_TIME) // == '|' && GET_TIME == TRUE)
       {
           time[j] = 0; // ou '\0'
           j = 0;
           GET_TIME = FALSE;
           GET_COMMANDE = TRUE;
       }
       else
       {
           if (GET_ID)
               id[j] = result[i];
           else if (GET_TIME)
               time[j] = result[i];
           else //else if (GET_COMMANDE)
               commande[j] = result[i];
           j++;
       }
   }
   
   // On affiche le nombre de résultat sur la StatusBar
   sprintf(commande, "%d ToDo(s) trouvé(s)", count); // on utilise 'commande' temporairement içi
   SetWindowText(hwToDoListStatusBar, commande);
   
   if (count > 0)
      return TRUE;
   else
      return FALSE;
}

//=============================================================================
// Fonction qui vérifie les commandes et affiche les messages d'erreur appropriés
//=============================================================================

BOOL checkCommande(HWND hDlg, char *commande)
{
    // Si la vérification est désactivée => return TRUE
    if (! CHECK_COMMANDS) return TRUE;
    
    int i = 0;
    char cmdPrefixe[100];
    
    // On récupère le prefixe/nom de la commande
    sscanf(commande, "%s", cmdPrefixe);
    
    for (i = 0; i < (sizeof cmd / sizeof *cmd); i++)
    {
        // On vérifie le préfixe de la commande d'abord
        if (!lstrcmp(cmdPrefixe, cmd[i].nom)) // si trouvé
        {
            // On récupère le nombre de paramètre par défaut de la commande
            int paramNbr = (!lstrcmp(cmd[i].param1, "NULL") ? 0 : 
                           ((!lstrcmp(cmd[i].param2, "NULL") || cmd[i].param2[0] == '|') ? 1 : 
                           ((!lstrcmp(cmd[i].param3, "NULL") || cmd[i].param3[0] == '|') ? 2 : 
                           ((!lstrcmp(cmd[i].param4, "NULL") || cmd[i].param4[0] == '|') ? 3 : 
                           ((!lstrcmp(cmd[i].param5, "NULL") || cmd[i].param5[0] == '|') ? 4 : 
                           ((!lstrcmp(cmd[i].param6, "NULL") || cmd[i].param6[0] == '|') ? 5 : 6
                           ))))));
            // On compte un paramètre tout ce qui est différent de NULL ou ne commence pas par un séparateur '|'
            // NB: le 1er paramètre ne peut pas commencer par un séparateur
            
            // On vérifie le nombre de paramètres
            if (strCharOccur(commande, ' ') == paramNbr) // si c'est bon
            {
                return TRUE;
            }
            else
            {
                //MessageBox(hDlg, "Le nombre de paramètre de cette commande est incorrect !", NOM_APP, MB_OK | MB_ICONWARNING);
                appendToRichConsole("erreur", "Le nombre de paramètre de cette commande est incorrect !");
                return FALSE;
            }
        }
    }
    
    //MessageBox(hDlg, "Cette commande n'existe pas !", NOM_APP, MB_OK | MB_ICONWARNING);
    appendToRichConsole("erreur", "Cette commande n'existe pas !");
    
    return FALSE;
}

//=============================================================================
//  Fonction qui renvoie le nombre d'occurence d'un caractere dans une chaine
//=============================================================================

int strCharOccur(char *chaine, char caractere)
{
    int occurence = 0, i;
    
    for (i = 0; i < strlen(chaine); i++)
    {
        if (chaine[i] == caractere)
           occurence++;
    }
    
    return occurence;
}

//=============================================================================
//             Fonction qui active l'onglet (Tab) spécifié
//=============================================================================

void setSettingsTab(HWND hwnd, BOOL statut, unsigned short tab)
{
   if (tab == GENERAL_TAB) /* Onglet Général (Tab 1) */
   {
      if (statut == SHOW_TAB)
      {
         /* On affiche les éléments de l'onglet Général */
         ShowWindow(GetDlgItem(hwnd, IDC_CRYPT_DECRYPT), SW_SHOW);
         ShowWindow(GetDlgItem(hwnd, IDC_CHECK_COMMANDS), SW_SHOW);
         ShowWindow(GetDlgItem(hwnd, IDC_ENABLE_COLORIZATION), SW_SHOW);
      }
      else
      {
         /* On cache les éléments de l'onglet Général */
         ShowWindow(GetDlgItem(hwnd, IDC_CRYPT_DECRYPT), SW_HIDE);
         ShowWindow(GetDlgItem(hwnd, IDC_CHECK_COMMANDS), SW_HIDE);
         ShowWindow(GetDlgItem(hwnd, IDC_ENABLE_COLORIZATION), SW_HIDE);
      }
   }
   else if (tab == EXPLORE_DRIVE_TAB) /* Explore Drive (Tab 2) */
   {
      if (statut == SHOW_TAB)
      {
         /* On affiche les éléments de l'onglet Explore Drive */
         ShowWindow(GetDlgItem(hwnd, IDC_SHOW_EXPLORE_TYPE), SW_SHOW);
         ShowWindow(GetDlgItem(hwnd, IDC_SHOW_EXPLORE_EXT), SW_SHOW);
      }
      else
      {
         /* On cache les éléments de l'onglet Explore Drive */
         ShowWindow(GetDlgItem(hwnd, IDC_SHOW_EXPLORE_TYPE), SW_HIDE);
         ShowWindow(GetDlgItem(hwnd, IDC_SHOW_EXPLORE_EXT), SW_HIDE);
      }    
   }
}
