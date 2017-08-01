//=============================================================================
// Projet : Winrell (server)
// Fichier : dialogBoxs.c (fenêtres secondaires)
//
//=============================================================================

#include "server.h"


//=============================================================================
//                       Structure FenInformationsParam
//
//=============================================================================

typedef struct {
    int clientIndex;
    char f_name_with_path[EXPLORE_MAX_PATH];
    char f_type[100];
    char f_ext[6];
} FenInformationsParam;

//=============================================================================
//                             AproposDialogBox
//
//=============================================================================

BOOL APIENTRY AproposDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg)
   {
       case WM_INITDIALOG:
            /* Affichage de l'icone */
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONE)));
            break;
       case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                /* Gestion des clics buttons (OK - FERMER) */
                case IDOK:
                case IDCANCEL:
                     EndDialog(hDlg, 0);
                     break;
            }
            break;
   }

   return 0;
}

//=============================================================================
//                            CommandeListeDialogBox
//
//=============================================================================

BOOL APIENTRY CommandeListeDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   static HWND hwListeCommandes;
   static int currentSelection;
   
   switch (uMsg)
   {
       case WM_INITDIALOG:
            /* Handle de la liste de commandes */
            hwListeCommandes = GetDlgItem(hDlg, LST_COMMANDES);
            /* Affichage de l'icone */
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONE)));
            /* Sélection par ligne */
            SendMessage(hwListeCommandes, LVM_SETEXTENDEDLISTVIEWSTYLE , 0, LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
            /* Ajout des colonnes */
            creerListViewColone(hwListeCommandes, 30, "N°", 0);
            creerListViewColone(hwListeCommandes, 120, "Commande prefixe", 1);
            creerListViewColone(hwListeCommandes, 100, "Parametre 1", 2);
            creerListViewColone(hwListeCommandes, 100, "Parametre 2", 3);
            creerListViewColone(hwListeCommandes, 100, "Parametre 3", 4);
            creerListViewColone(hwListeCommandes, 100, "Parametre 4", 5);
            creerListViewColone(hwListeCommandes, 100, "Parametre 5", 6);
            creerListViewColone(hwListeCommandes, 100, "Parametre 6", 7);
            /* Ajout des commandes */
            remplirListeDesCommandes(hwListeCommandes);    
            /* Initialisation */
            currentSelection = -1;
            break;
       case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                /* Gestion des clics buttons (FERMER...) */
                case ID_COPIER_SELECTION:
                     {
                         if (currentSelection != -1)
                         {
                             // On récupère la séléction
                             char selection[256];
                             ListView_GetItemText(hwListeCommandes, currentSelection, 1, selection, 256);
                             // On copie la séléction dans le clipboard/presse papier
                             size_t len = strlen(selection) + 1;
                             HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, len);
                             memcpy(GlobalLock(hMem), selection, len);
                             GlobalUnlock(hMem);
                             OpenClipboard(NULL);
                             EmptyClipboard();
                             SetClipboardData(CF_TEXT, hMem);
                             CloseClipboard();
                         }
                         else
                             MessageBox(hDlg, "Veuillez séléctionner une commande !", NOM_APP, MB_OK | MB_ICONWARNING);
                     }
                     break;
                case IDOK:
                case IDCANCEL:
                     EndDialog(hDlg, 0);
                     break;
            }
            break;
       case WM_NOTIFY:
            switch(LOWORD(wParam))
	        {
                case LST_COMMANDES: // si c'est notre liste de commande
                   switch (((LPNMHDR)lParam)->code)
                   {
                       case LVN_ITEMCHANGED: //NM_CLICK: // NM_CLICK - Single click on an item
                       {
                          NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)lParam;
                          if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED))
                             currentSelection = ((LPNM_LISTVIEW)lParam)->iItem;
                       }
                       break;
                   }
                   
                   // on change les couleurs des lignes/rows
                   return customdraw_handler(hDlg, wParam, lParam, RGB(242, 242, 242));
                break;
            }
            break;
   }

   return 0;
}

//=============================================================================
//                            ExploreDriveDialogBox
//
//=============================================================================

BOOL APIENTRY ExploreDriveDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   //static HWND hwListeExploration;
   static HWND hwBtBackToRoot, hwBtExploreBack, hwBtExploreDirectory, hwBtDownloadFile, hwBtDelete, hwBtRunFile, hwBtNewFolder, hwBtRename, hwBtCopy, hwBtCut, hwBtPaste, hwBtInfo;
   static HWND hwExplorePathEdit;
   static HWND hTTip;
   static char pathSave[EXPLORE_MAX_PATH], newPath[EXPLORE_MAX_PATH], fileType[100], fileExt[6], cmd[EXPLORE_MAX_PATH], toCopyCutPath[EXPLORE_MAX_PATH];
   static int clientIndex, clientId, clientsNumberSave;
   static BOOL isCopy = FALSE, isCut = FALSE;
   
   switch (uMsg)
   {
       case WM_INITDIALOG:
            /* Handle de tout les controls */
            hwListeExploration = GetDlgItem(hDlg, LST_EXPLORE_DRIVE);
            hwBtBackToRoot = GetDlgItem(hDlg, IDB_BACK_TO_ROOT);
            hwBtExploreBack = GetDlgItem(hDlg, IDB_EXPLORE_BACK);
            hwBtExploreDirectory = GetDlgItem(hDlg, IDB_EXPLORE_DIRECTORY);
            hwBtDownloadFile = GetDlgItem(hDlg, IDB_DOWNLOAD_FILE);
            hwExplorePathEdit = GetDlgItem(hDlg, IDE_EXPLORE_PATH);
            hwBtDelete = GetDlgItem(hDlg, IDB_DELETE);
            hwBtRunFile = GetDlgItem(hDlg, IDB_RUN_FILE);
            hwBtNewFolder = GetDlgItem(hDlg, IDB_NEW_FOLDER);
            hwBtRename = GetDlgItem(hDlg, IDB_RENAME);
            hwBtCopy = GetDlgItem(hDlg, IDB_COPY);
            hwBtCut = GetDlgItem(hDlg, IDB_CUT);
            hwBtPaste = GetDlgItem(hDlg, IDB_PASTE);
            hwBtInfo = GetDlgItem(hDlg, IDB_INFO);
            /* Affichage de l'icone */
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_SEARCH)));
            /* Récupération du titre de la fenêtre */
            char fenTitle[128];
            GetWindowText(hDlg, fenTitle, 128);
            /* Sauvegarde des données du client */
            clientIndex = lvItemActuel;
            clientId = clients[clientIndex].id;
            clientsNumberSave = actualClientsNumber;
            /* Modification du titre de la fenêtre */
            sprintf(fenTitle, "%s > %s", fenTitle, clients[clientIndex].name);
            SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)fenTitle);
            /* Création d'un tooltips (qui affiche des bulbes/bules d'informations) */
             hTTip = CreateWindow(TOOLTIPS_CLASS, NULL 
             , WS_POPUP | TTS_NOPREFIX | TTS_BALLOON 
             , CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT 
             , hDlg, NULL, hInst, NULL);
             
             //SendMessage(hTTip, TTM_SETTITLE, TTI_INFO, (LPARAM)"Informations"); /* Titre des infos bulbes */
            /* Affichage des icones des bouttons + Affectation des tooltips */
            // back to root
            SendMessage(hwBtBackToRoot, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_DRIVE), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtBackToRoot, hDlg, hTTip, "Back To Root");
            // explore back
            SendMessage(hwBtExploreBack, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_EXPLORE_BACK), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtExploreBack, hDlg, hTTip, "Explore Back");
            // explore directory
            SendMessage(hwBtExploreDirectory, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_EXPLORE), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtExploreDirectory, hDlg, hTTip, "Explore/Refresh Directory");
            // download file
            SendMessage(hwBtDownloadFile, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_DOWNLOAD), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtDownloadFile, hDlg, hTTip, "Download File");
            EnableWindow(hwBtDownloadFile, FALSE); /* Désactivation */
            // run file
            SendMessage(hwBtRunFile, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_RUN), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtRunFile, hDlg, hTTip, "Run File");
            EnableWindow(hwBtRunFile, FALSE); /* Désactivation */
            // delete
            SendMessage(hwBtDelete, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_DELETE), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtDelete, hDlg, hTTip, "Delete File/Folder");
            EnableWindow(hwBtDelete, FALSE); /* Désactivation */
            // new folder
            SendMessage(hwBtNewFolder, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_NEW_FOLDER), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtNewFolder, hDlg, hTTip, "New Folder");
            EnableWindow(hwBtNewFolder, FALSE); /* Désactivation */
            // rename
            SendMessage(hwBtRename, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_RENAME), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtRename, hDlg, hTTip, "Rename File/Folder");
            EnableWindow(hwBtRename, FALSE); /* Désactivation */
            // copy
            SendMessage(hwBtCopy, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_COPY), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtCopy, hDlg, hTTip, "Copy File/Folder");
            EnableWindow(hwBtCopy, FALSE); /* Désactivation */
            // cut
            SendMessage(hwBtCut, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_CUT), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtCut, hDlg, hTTip, "Cut File/Folder");
            EnableWindow(hwBtCut, FALSE); /* Désactivation */
            // paste
            SendMessage(hwBtPaste, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_PASTE), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtPaste, hDlg, hTTip, "Paste File/Folder");
            EnableWindow(hwBtPaste, FALSE); /* Désactivation */
            // file/folder info
            SendMessage(hwBtInfo, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_INFO), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtInfo, hDlg, hTTip, "File/Folder Informations");
            /* Sélection par ligne */
            SendMessage(hwListeExploration, LVM_SETEXTENDEDLISTVIEWSTYLE , 0, LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
            /* Ajout des colonnes */
            creerListViewColone(hwListeExploration, 400, "Nom", 0);
            creerListViewColone(hwListeExploration, SHOW_EXPLORE_TYPE ? 200 : 0, "Type", 1);
            creerListViewColone(hwListeExploration, SHOW_EXPLORE_EXT ? 80 : 0, "Extension", 2);
            /* Création de la bar de status */
            hwExploreStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "", hDlg, ID_EXPLORE_STATUSBAR);
            /* Initialisation */
            lstrcpy(pathSave, "");
            lstrcpy(newPath, "");
            /* Affichage des drives/partitions pour une première fois */
            write_client(&clients[clientIndex], "Explore_Drives", TRUE);
            break;
       case WM_COMMAND:
            
            if (clientIndex != -1) // si le client a déco pas la peine de revérifier
            {
                // S'il n'y a plus de client dont on est entrain d'explorer le drive (fichiers/dossiers)
                if (!checkClient(hDlg, &clientIndex, clientId, TRUE, FALSE, IDB_EXPLORE_DRIVE))
                    break; // obligatoire
            }
            else // pour sortir car dans ce cas le client s'est déco
                break; // obligatoire
            
            switch (LOWORD(wParam))
            {
                /* Gestion des clics buttons (FERMER...) */
                case IDB_BACK_TO_ROOT:
                     // vérifications
                     if (!checkClient(hDlg, &clientIndex, clientId, FALSE, TRUE, IDB_EXPLORE_DRIVE))
                         break;
                     else
                     {
                         // vidage du textBox
                         SendMessage(hwExplorePathEdit, WM_SETTEXT, 0, (LPARAM)"");
                         // Envoie de la commande
                         write_client(&clients[clientIndex], "Explore_Drives", TRUE);
                         /* Rénitialisation */
                         lstrcpy(pathSave, "");
                         lstrcpy(newPath, "");
                         /* Activation du boutton -> */
                         EnableWindow(hwBtExploreDirectory, TRUE);
                         /* Désactivation des bouttons new folder, download file, run file, delete/supprimer, rename, copy, cut, paste */
                         HWND hwButtonsTable[] = { hwBtNewFolder, hwBtDownloadFile, hwBtRunFile, hwBtDelete, hwBtRename, hwBtCopy, hwBtCut, hwBtPaste };
                         enableDisableButtons(hwButtonsTable, (sizeof hwButtonsTable / sizeof *hwButtonsTable), FALSE);
                     }
                     break;
                case IDB_EXPLORE_BACK:
                     if(pathBack(pathSave))
                     {
                         // vérifications
                         if (!checkClient(hDlg, &clientIndex, clientId, FALSE, TRUE, IDB_EXPLORE_DRIVE))
                             break;
                         else
                         {
                             // construction de la commande
                             sprintf(cmd, "Send_File explore->%s", pathSave);
                             // exécution de la commande
                             write_client(&clients[clientIndex], cmd, TRUE);
                             // on change le path atuel/newPath
                             lstrcpy(newPath, pathSave);
                             // On change/affiche le path actuel
                             SendMessage(hwExplorePathEdit, WM_SETTEXT, 0, (LPARAM)newPath);
                             /* Activation du boutton -> */
                             EnableWindow(hwBtExploreDirectory, TRUE);
                             /* Activation du boutton new folder */
                             EnableWindow(hwBtNewFolder, TRUE);
                             /* Désactivation des bouttons download file, run file, delete/supprimer, rename, copy, cut */
                             HWND hwButtonsTable[] = { hwBtDownloadFile, hwBtRunFile, hwBtDelete, hwBtRename, hwBtCopy, hwBtCut };
                             enableDisableButtons(hwButtonsTable, (sizeof hwButtonsTable / sizeof *hwButtonsTable), FALSE);
                             /* Activation du boutton paste */
                             EnableWindow(hwBtPaste, TRUE);
                         }
                     }
                     else
                         MessageBox(hDlg, "Retour en arrière impossible !", NOM_APP, MB_OK | MB_ICONWARNING);
                     break;
                case IDB_EXPLORE_DIRECTORY:
                case IDB_DOWNLOAD_FILE:
                  {
                     // On récupère la taille du path actuel
                     int pathLen = strlen(newPath);
                     if (pathLen == 0)
                         MessageBox(hDlg, "Aucun chemin spécifié !", NOM_APP, MB_OK | MB_ICONWARNING);
                     else
                     {
                         // Si c'est un fichier avec extension
                         if (strlen(fileExt) > 0)
                         {// c'est une requête de téléchargement
                             if (MessageBox(hDlg, "Voulez-vous vraiment télécharger ce fichier ?", NOM_APP, MB_YESNO | MB_ICONQUESTION) == IDYES)
                             {
                                 // vérifications (les 2 vérif. => TRUE, TRUE)
                                 if (!checkClient(hDlg, &clientIndex, clientId, TRUE, TRUE, IDB_EXPLORE_DRIVE))
                                     break;
                                 else
                                 {
                                     // construction de la commande
                                     sprintf(cmd, "Send_File %s", newPath);
                                     // exécution de la commande
                                     write_client(&clients[clientIndex], cmd, TRUE);
                                 }
                             }
                         }
                         else
                         {// c'est une requête d'exploration de dossier alors
                             // vérifications
                             if (!checkClient(hDlg, &clientIndex, clientId, FALSE, TRUE, IDB_EXPLORE_DRIVE))
                                 break;
                             else
                             {
                                 // on sauvegarde le path/chemin
                                 lstrcpy(pathSave, newPath);
                                 // construction de la commande
                                 sprintf(cmd, "Send_File explore->%s", newPath); // newPath ou savePath peu importe (on vien de sauvegarder)
                                 // exécution de la commande
                                 write_client(&clients[clientIndex], cmd, TRUE);
                                 /* Désactivation des bouttons delete/supprimer, rename, copy, cut */
                                 HWND hwButtonsTable[] = { hwBtDelete, hwBtRename, hwBtCopy, hwBtCut };
                                 enableDisableButtons(hwButtonsTable, (sizeof hwButtonsTable / sizeof *hwButtonsTable), FALSE);
                                 /* Activation du boutton new folder */
                                 EnableWindow(hwBtNewFolder, TRUE);
                                 /* Activation du boutton paste */
                                 EnableWindow(hwBtPaste, TRUE);
                             } // fin 3ème else
                         } // fin 2ème else
                     } // fin 1er else
                  }
                     break;
                case IDB_RUN_FILE:
                     /* Pour les vérifications les even. s'occupe d'activer/désactiver le boutton, donc nul besoin de vérifier */
                     if (MessageBox(hDlg, "Etes-vous sûr de vouloir exécuter/lancer ce fichier ?", NOM_APP, MB_YESNO | MB_ICONQUESTION) == IDYES)
                     {
                         // vérifications (les 2 vérif. => TRUE, TRUE)
                         if (!checkClient(hDlg, &clientIndex, clientId, TRUE, TRUE, IDB_EXPLORE_DRIVE))
                             break;
                         else
                         {
                             // construction de la commande
                             sprintf(cmd, "Shell_Exec %s", newPath);
                             // exécution de la commande
                             write_client(&clients[clientIndex], cmd, TRUE);
                         }
                     }
                     break;
                case IDB_NEW_FOLDER: // new folder
                case IDB_DELETE:     // delete
                case IDB_RENAME:     // rename
                case IDB_COPY:       // copy
                case IDB_CUT:        // cut
                case IDB_PASTE:      // paste
                  {
                     // stockage de l'id du boutton actuel
                     int currentBtn = LOWORD(wParam);
                     // On récupère la taille du path actuel
                     int pathLen = strlen(newPath);
                     // si path vide
                     if (pathLen == 0)
                         MessageBox(hDlg, "Aucun chemin spécifié !", NOM_APP, MB_OK | MB_ICONWARNING);
                     else if (currentBtn != IDB_NEW_FOLDER && currentBtn != IDB_PASTE && newPath[pathLen - 1] == '\\') // si nn si partition + pas la peine de vérifier ça si création d'un nouveau dossier + pareil si c'est un collage de dossier/fichier
                         MessageBox(hDlg, "Action impossible sur une partition !", NOM_APP, MB_OK | MB_ICONWARNING);
                     else
                     {
                         // si ce n'est pas une suppression pas la peine de demander une confirmation
                         if (isNotDelete(currentBtn) || MessageBox(hDlg, "Etes-vous sûr de vouloir supprimer ce fichier/dossier ?", NOM_APP, MB_YESNO | MB_ICONQUESTION) == IDYES)
                         {
                             // vérifications (les 2 vérif. => TRUE, TRUE)
                             if (!checkClient(hDlg, &clientIndex, clientId, TRUE, TRUE, IDB_EXPLORE_DRIVE))
                                 break;
                             else
                             {
                                // si suppression/delete
                                if (currentBtn == IDB_DELETE)
                                   // construction de la commande de suppression
                                   sprintf(cmd, "File_Opr DELETE %s|-", newPath);
                                // si nn si copy |or| cut
                                else if (currentBtn == IDB_COPY || currentBtn == IDB_CUT)
                                {
                                   lstrcpy(toCopyCutPath, newPath);
                                   isCopy = currentBtn == IDB_COPY ? TRUE : FALSE;
                                   isCut = !isCopy;
                                   MessageBox(hDlg, "Changez de répertoire pour pouvoir coller !", NOM_APP, MB_OK | MB_ICONINFORMATION);
                                   break; // on sort du switch
                                }
                                // si nn si paste
                                else if (currentBtn == IDB_PASTE)
                                {
                                   if (isCopy) // si copy
                                   {
                                       // construction de la commande de copy/copie
                                       sprintf(cmd, "File_Opr COPY %s|%s", toCopyCutPath, newPath);
                                   }
                                   else if (isCut) // si nn si cut
                                   {
                                       // construction de la commande de cut/déplacement
                                       sprintf(cmd, "File_Opr MOVE %s|%s", toCopyCutPath, newPath);
                                       // on rénitialise isCut, car après un déplacement/cut on ne peut plus coller la source
                                       isCut = FALSE;
                                   }
                                   else
                                   {
                                       MessageBox(hDlg, "Rien à coller !", NOM_APP, MB_OK | MB_ICONWARNING);
                                       break; // on sort du switch
                                   }
                                }
                                else // si nn, c'est un renommage ou une création de dossier
                                {
                                   // on récupère le nouveau nom/le nom du dossier
                                   DialogBox(hInst, "ENTER_NAME", hDlg, (DLGPROC)EnterNameDlgProc);
                                   // si l'utilisateur n'a pas rentré de nom
                                   if (strlen(EXPLORE_NEW_F_NAME) == 0)
                                       break; // on annule tout
                                   else
                                   {
                                       // vérifications (les 2 vérif. => TRUE, TRUE)
                                       if (!checkClient(hDlg, &clientIndex, clientId, TRUE, TRUE, IDB_EXPLORE_DRIVE))
                                           break;
                                       else
                                       {
                                           // si c'est un renommage
                                           if (currentBtn == IDB_RENAME)
                                               // construction de la commande de renommage
                                               sprintf(cmd, "File_Opr RENAME %s|%s%s%s", newPath, pathSave, pathSave[strlen(pathSave) - 1] == '\\' ? "" : "\\", EXPLORE_NEW_F_NAME);
                                           else
                                               // construction de la commande de création de dossier
                                               sprintf(cmd, "Create_Directory %s%s%s", newPath, newPath[strlen(newPath) - 1] == '\\' ? "" : "\\", EXPLORE_NEW_F_NAME);
                                       } // fin else (des vérifications)
                                   }
                                }
                                
                                // exécution de la commande
                                write_client(&clients[clientIndex], cmd, TRUE);
                                // On change/affiche le path actuel
                                lstrcpy(newPath, pathSave);
                                SendMessage(hwExplorePathEdit, WM_SETTEXT, 0, (LPARAM)newPath);
                                // On change/rénitialise l'extension
                                lstrcpy(fileExt, "");
                                // On active/désactive les bouttons (car on a changé le path actuel)
                                /* Activation du boutton -> */
                                EnableWindow(hwBtExploreDirectory, TRUE);
                                /* Désactivation des bouttons new folder, download file, run file, delete/supprimer, rename, copy, cut, paste */
                                HWND hwButtonsTable[] = { hwBtNewFolder, hwBtDownloadFile, hwBtRunFile, hwBtDelete, hwBtRename, hwBtCopy, hwBtCut, hwBtPaste };
                                enableDisableButtons(hwButtonsTable, (sizeof hwButtonsTable / sizeof *hwButtonsTable), FALSE);
                                // On demande si l'utilisateur veut raffraichir l'explorateur
                                if (MessageBox(hDlg, "Raffraichir l'explorateur ?", NOM_APP, MB_YESNO | MB_ICONQUESTION) == IDYES)
                                {
                                    // on simule l'appui sur Explore Directory pour raffraihir (cela permet d'éviter une erreur si le client déco)
                                    PostMessage(hDlg, WM_COMMAND, IDB_EXPLORE_DIRECTORY, 0);
                                }
                             }
                         }
                     }
                  }
                     break;
                case IDB_INFO: // file/folder info
                  {
                     // On récupère la taille du path actuel
                     int pathLen = strlen(newPath);
                     // si path vide
                     if (pathLen == 0)
                         MessageBox(hDlg, "Aucun chemin spécifié !", NOM_APP, MB_OK | MB_ICONWARNING);
                     else if (newPath[pathLen - 1] == '\\') // si nn si partition
                         MessageBox(hDlg, "Action impossible sur une partition !", NOM_APP, MB_OK | MB_ICONWARNING);
                     else
                     {
                         // vérifications
                         if (!checkClient(hDlg, &clientIndex, clientId, FALSE, TRUE, IDB_EXPLORE_DRIVE))
                             break;
                         else
                         {
                             // on prépare les parametres à passer à la fenêtre
                             FenInformationsParam fenInfoParam;
                             fenInfoParam.clientIndex = clientIndex;
                             lstrcpy(fenInfoParam.f_name_with_path, newPath);
                             lstrcpy(fenInfoParam.f_type, fileType);
                             lstrcpy(fenInfoParam.f_ext, fileExt);
                             // Création de la fenêtre + passage des parametres
                             DialogBoxParam(hInst, "INFO_DLG", hDlg, (DLGPROC)InfoDlgProc, (LPARAM) &fenInfoParam);
                         }
                     }
                  }
                     break;
                case IDCANCEL:
                     if (clientIndex != -1)
                        clients[clientIndex].explore_drive = FALSE;
                     /* Activation du boutton Explore drive à la fermeture */
                     EnableWindow(GetDlgItem(hwFenetre, IDB_EXPLORE_DRIVE), TRUE);
                     EndDialog(hDlg, 0);
                     break;
            }
            break;
       case WM_NOTIFY: // the message that is being sent always
           {
               switch(LOWORD(wParam)) // hit control
               {
                   case LST_EXPLORE_DRIVE: // did we hit our ListView contorl?
                       switch (((LPNMHDR)lParam)->code)
                       {
                           /*
                           case NM_CLICK: // NM_CLICK - Single click on an item
                               break;
                           */
                           case LVN_ITEMCHANGED: // LVN_ITEMCHANGED - new item selected
                           {
                             NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)lParam;
                             if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED))
                             {
                               // On récupère la position actuelle sur la ListView
                               int currentPos = ((LPNM_LISTVIEW)lParam)->iItem;
                               if (currentPos != -1) // si il y'a une séléction
                               {
                                   // On récupère la séléction (nom + type + extension)
                                   char nom[256];
                                   ListView_GetItemText(hwListeExploration, currentPos, 0, nom, 256);
                                   ListView_GetItemText(hwListeExploration, currentPos, 1, fileType, 100);
                                   ListView_GetItemText(hwListeExploration, currentPos, 2, fileExt, 6);
                                   // On prépare le nouveau path
                                   int pathLen = strlen(pathSave);
                                   if (pathLen == 0)
                                       sprintf(newPath, "%s", nom); // sans '\\' car les drives/partitions l'ont déjà, ex : 'C:\\'
                                   else if (pathSave[pathLen - 1] == '\\') // si jamais il y'a déjà les '\\'
                                       sprintf(newPath, "%s%s", pathSave, nom); // on ne les met pas
                                   else // si nn on les met
                                       sprintf(newPath, "%s\\%s", pathSave, nom);
                                   // On change/affiche le path actuel
                                   SendMessage(hwExplorePathEdit, WM_SETTEXT, 0, (LPARAM)newPath);
                                   // On active/désactive les bouttons de navigations/download, ...
                                   // On récupère l'extension du fichier, si c'est un dossier ext sera == ""
                                   char ext[6];
                                   getExtension(newPath, ext);
                                   // Si c'est un fichier avec extension
                                   if (strlen(ext) > 0)
                                   {
                                       /* Activation des bouttons download file, run file */
                                       EnableWindow(hwBtDownloadFile, TRUE);
                                       EnableWindow(hwBtRunFile, TRUE);
                                       /* Désactivation du boutton -> */
                                       EnableWindow(hwBtExploreDirectory, FALSE);                 
                                   }
                                   else // si nn (un dossier surement)
                                   {
                                       /* Désactivation des bouttons download file, run file */
                                       EnableWindow(hwBtDownloadFile, FALSE);
                                       EnableWindow(hwBtRunFile, FALSE);
                                       /* Activation du boutton -> */
                                       EnableWindow(hwBtExploreDirectory, TRUE);
                                   }
                                   /* Activation des bouttons delete/supprimer, rename, copy, cut */
                                   HWND hwButtonsTable[] = { hwBtDelete, hwBtRename, hwBtCopy, hwBtCut };
                                   enableDisableButtons(hwButtonsTable, (sizeof hwButtonsTable / sizeof *hwButtonsTable), TRUE);
                                   /* Désactivation du boutton new folder */
                                   EnableWindow(hwBtNewFolder, FALSE);
                                   /* Désactivation du boutton paste */
                                   EnableWindow(hwBtPaste, FALSE);
                               } // fin if currentPos..
                             } // fin 1er if
                           }
                               break;
                           case NM_DBLCLK: // Double Click
                               {
                                   // On récupère la position actuelle sur la ListView
                                   int currentPos = ((LPNM_LISTVIEW)lParam)->iItem;
                                   
                                   if (currentPos != -1) // s'il y'a un item séléctionné
                                   {
                                       // on simule l'appui sur Explore Directory (ou Download File peu importe les 2 cases utilisent le même code avec un if qui gère les 2 cas)
                                       PostMessage(hDlg, WM_COMMAND, IDB_EXPLORE_DIRECTORY, 0);
                                   }
                               }
                               break;
                       } // fin 2ème switch
                   break;
               } // fin 1er switch
           }
           break;
   }

   return 0;
}

//=============================================================================
//                             TaskListDialogBox
//
//=============================================================================

BOOL APIENTRY TaskListDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   //static HWND hwTaskList;
   static HWND hwBtStopTask, hwBtRefreshTaskList;
   static HWND hwSelectedTaskEdit;
   static HWND hTTip;
   static int clientIndex, clientId, clientsNumberSave;
   
   switch (uMsg)
   {
       case WM_INITDIALOG:
            /* Affichage de l'icone */
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_TASK)));
            /* Handle de tout les controls */
            hwTaskList = GetDlgItem(hDlg, LST_TASKS);
            hwBtStopTask = GetDlgItem(hDlg, IDB_STOP_TASK);
            hwSelectedTaskEdit = GetDlgItem(hDlg, IDE_SELECTED_TASK);
            hwBtRefreshTaskList = GetDlgItem(hDlg, IDB_REFRESH_TASK_LIST);
            /* Récupération du titre de la fenêtre */
            char fenTitle[128];
            GetWindowText(hDlg, fenTitle, 128);
            /* Sauvegarde des données du client */
            clientIndex = lvItemActuel;
            clientId = clients[clientIndex].id;
            clientsNumberSave = actualClientsNumber;
            /* Modification du titre de la fenêtre */
            sprintf(fenTitle, "%s > %s", fenTitle, clients[clientIndex].name);
            SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)fenTitle);
            /* Création d'un tooltips (qui affiche des bulbes/bules d'informations) */
             hTTip = CreateWindow(TOOLTIPS_CLASS, NULL 
             , WS_POPUP | TTS_NOPREFIX | TTS_BALLOON 
             , CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT 
             , hDlg, NULL, hInst, NULL);
             
             //SendMessage(hTTip, TTM_SETTITLE, TTI_INFO, (LPARAM)"Informations"); /* Titre des infos bulbes */
            /* Affichage des icones des bouttons + Affectation des tooltips */
            // refresh task list
            SendMessage(hwBtRefreshTaskList, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_REFRESH), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtRefreshTaskList, hDlg, hTTip, "Resfresh Task List");
            // stop task
            SendMessage(hwBtStopTask, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_STOP), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtStopTask, hDlg, hTTip, "Stop Task");
            /* Sélection par ligne dans la ListView */
            SendMessage(hwTaskList, LVM_SETEXTENDEDLISTVIEWSTYLE , 0, LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
            /* Ajout des colonnes */
            creerListViewColone(hwTaskList, 365, "Task", 0);
            creerListViewColone(hwTaskList, 100, "Type", 1);
            /* Création de la bar de status */
            hwTaskListStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "", hDlg, ID_TASK_LST_STATUSBAR);
            /* Récupération/Affichage des Tasks/Process */
            write_client(&clients[clientIndex], "Send_File tasks", TRUE);
            break;
       case WM_COMMAND:
            
            if (clientIndex != -1) // si le client a déco pas la peine de revérifier
            {
                // Si le client s'est déconnecté
                if (!checkClient(hDlg, &clientIndex, clientId, TRUE, FALSE, IDB_TASKS_LIST))
                    break; // obligatoire
            }
            else
                break; // obligatoire
            
            switch (LOWORD(wParam))
            {
                /* Gestion des clics buttons */
                case IDB_REFRESH_TASK_LIST:
                     // vérifications
                     if (!checkClient(hDlg, &clientIndex, clientId, FALSE, TRUE, IDB_TASKS_LIST))
                         break;
                     else
                     {
                         // on vide l'editbox
                         SendMessage(hwSelectedTaskEdit, WM_SETTEXT, 0, (LPARAM)"");
                         // exécution de la commande
                         write_client(&clients[clientIndex], "Send_File tasks", TRUE);
                     }
                     break;
                case IDB_STOP_TASK:
                     {
                         // Récupération du task contenu dans l'editbox
                         char task[128];
                         GetWindowText(hwSelectedTaskEdit, task, 128);
                         // S'il y'en a pas
                         if (strlen(task) == 0)
                             MessageBox(hDlg, "Veuillez séléctionner un Task !", NOM_APP, MB_OK | MB_ICONWARNING);
                         else // si nn s'il y'en a
                         {
                             if (MessageBox(hDlg, "Etes-vous sûr de vouloir arrêter ce Task ?", NOM_APP, MB_YESNO | MB_ICONQUESTION) == IDYES)
                             {
                                 // vérifications (les 2 vérif. => TRUE, TRUE)
                                 if (!checkClient(hDlg, &clientIndex, clientId, TRUE, TRUE, IDB_TASKS_LIST))
                                     break;
                                 else
                                 {
                                     char cmd[256];
                                     // construction de la commande
                                     sprintf(cmd, "Stop_Prg %s", task);
                                     // exécution de la commande
                                     write_client(&clients[clientIndex], cmd, TRUE);
                                     if (MessageBox(hDlg, "Rafraichir la liste ?", NOM_APP, MB_YESNO | MB_ICONQUESTION) == IDYES)
                                     {
                                         // on simule l'appui sur Refresh Task List (pour éviter un problème si jamais le client déco)
                                         PostMessage(hDlg, WM_COMMAND, IDB_REFRESH_TASK_LIST, 0);
                                     }
                                 }
                             }
                         }
                     }
                     break;
                case IDCANCEL:
                     if (clientIndex != -1)
                        clients[clientIndex].task_list_opened = FALSE;
                     /* Activation du boutton Task(s)/Process List à la fermeture */
                     EnableWindow(GetDlgItem(hwFenetre, IDB_TASKS_LIST), TRUE);
                     EndDialog(hDlg, 0);
                     break;
            }
            break;
         case WM_NOTIFY:
            switch(LOWORD(wParam))
	        {
                case LST_TASKS: // si c'est notre liste de commande
                   switch (((LPNMHDR)lParam)->code)
                   {
                       case LVN_ITEMCHANGED: // Changement de l'item actuel de la ListView
                       {
                          NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)lParam;
                          if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED))
                          {
                             // On récupère la séléction
                             char task[256];
                             ListView_GetItemText(hwTaskList, ((LPNM_LISTVIEW)lParam)->iItem, 0, task, 256);
                             // On change/affiche le task actuel/séléctionné
                             SendMessage(hwSelectedTaskEdit, WM_SETTEXT, 0, (LPARAM)task);
                          }
                       }
                       break;
                   }
                   
                   // on change les couleurs des lignes/rows
                   return customdraw_handler(hDlg, wParam, lParam, RGB(242, 242, 242));
                break;
            }
            break;
   }

   return 0;
}

//=============================================================================
//                             TurnOffDialogBox
//
//=============================================================================

BOOL APIENTRY TurnOffDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   static HWND hwTurnOffComboBox, hwTimeComboBox;
   
   switch (uMsg)
   {
       case WM_INITDIALOG:
            /* Affichage de l'icone */
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_TURN_OFF)));
            /* Récupération des handles */
            hwTurnOffComboBox = GetDlgItem(hDlg, IDC_TURN_OFF);
            hwTimeComboBox = GetDlgItem(hDlg, IDC_TIME);
            /* Remplissage de la combobox */
            SendMessage(hwTurnOffComboBox, CB_ADDSTRING, 0, (LONG)"Turn Off");
            SendMessage(hwTurnOffComboBox, CB_ADDSTRING, 0, (LONG)"Reboot");
            SendMessage(hwTurnOffComboBox, CB_ADDSTRING, 0, (LONG)"Sleep");
            SendMessage(hwTurnOffComboBox, CB_ADDSTRING, 0, (LONG)"Reset");
            /* Remplissage de la 2ème combobox */
            SendMessage(hwTimeComboBox, CB_ADDSTRING, 0, (LONG)"0");
            SendMessage(hwTimeComboBox, CB_ADDSTRING, 0, (LONG)"5");
            SendMessage(hwTimeComboBox, CB_ADDSTRING, 0, (LONG)"10");
            SendMessage(hwTimeComboBox, CB_ADDSTRING, 0, (LONG)"15");
            SendMessage(hwTimeComboBox, CB_ADDSTRING, 0, (LONG)"20");
            SendMessage(hwTimeComboBox, CB_ADDSTRING, 0, (LONG)"30");
            SendMessage(hwTimeComboBox, CB_ADDSTRING, 0, (LONG)"60");
            SendMessage(hwTimeComboBox, CB_ADDSTRING, 0, (LONG)"120");
            SendMessage(hwTimeComboBox, CB_ADDSTRING, 0, (LONG)"180");
            SendMessage(hwTimeComboBox, CB_ADDSTRING, 0, (LONG)"240");
            /* Séléction du premier item de la combobox */
            SendMessage(hwTurnOffComboBox, CB_SETCURSEL, 0, 0);
            SendMessage(hwTimeComboBox, CB_SETCURSEL, 0, 0);
            break;
       case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                /* Gestion des clics buttons */
                case IDOK:
                     if (actualClientsNumber > 0)
                     {
                         if (lvItemActuel != -1) // Si un client est séléctionné
                         {
                            if (clients[lvItemActuel].send_file)
                               MessageBox(hDlg, "Le client est en train d'envoyer un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
                            else if (clients[lvItemActuel].recv_file)
                               MessageBox(hDlg, "Le client reçoit un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
                            else
                            {
                                // On récupère la séléction
                                int cmd = SendMessage(hwTurnOffComboBox, CB_GETCURSEL, 0, 0);
                                char timeStr[5];
                                GetWindowText(hwTimeComboBox, timeStr, 5);
                                int time = strtol(timeStr, NULL, 10);
                                
                                // On exécute la commande voulu
                                if (time == 0)
                                {
                                   if (cmd == 0)
                                      write_client(&clients[lvItemActuel], "Turn_Off", TRUE);
                                   else if (cmd == 1)
                                      write_client(&clients[lvItemActuel], "Reboot", TRUE);
                                   else if (cmd == 2)
                                      write_client(&clients[lvItemActuel], "Menveille", TRUE);
                                   else
                                      write_client(&clients[lvItemActuel], "Reset", TRUE);
                                }
                                else
                                {
                                   char tmp[100];
                                   sprintf(tmp, "Add_ToDo %d|%s", time, (cmd == 0 ? "Turn_Off" : (cmd == 1 ? "Reboot" : (cmd == 2 ? "Menveille" : "Reset"))));
                                   write_client(&clients[lvItemActuel], tmp, TRUE);
                                }
                            }
                         }
                         else
                            MessageBox(hDlg, "Veuillez sélectionner un client !", NOM_APP, MB_OK | MB_ICONWARNING);
                      }
                      else
                         MessageBox(hDlg, "Aucun client en ligne !", NOM_APP, MB_OK | MB_ICONWARNING);
                     break;
                case IDCANCEL:
                     // Activation du boutton à la fermeture
                     EnableWindow(GetDlgItem(hwFenetre, IDB_TURN_OFF), TRUE);
                     EndDialog(hDlg, 0);
                     break;
            }
            break;
   }

   return 0;
}

//=============================================================================
//                             UpdateConfigDialogBox
//
//=============================================================================

BOOL APIENTRY UpdateConfigDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   static HWND hwBtUpdate;
   static HWND hwStopEspionCombo, hwStopSenderCombo, hwStopEspionMdpCombo, hwStopClientCombo, hwRunOnBootCombo;
   static HWND hwEmailEdit, hwServeurEdit, hwPortEdit, hwSenderSleepTimeEdit;
   static int clientIndex, clientId, clientsNumberSave;
   
   switch (uMsg)
   {
       case WM_INITDIALOG:
            /* Sauvegarde du handle */
            hwFenUpdateConfig = hDlg;
            /* Affichage de l'icone */
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_CONFIG)));
            /* Handle de tout les controls */
            hwBtUpdate = GetDlgItem(hDlg, IDOK);
            hwStopEspionCombo = GetDlgItem(hDlg, IDC_STOP_ESPION);
            hwStopSenderCombo = GetDlgItem(hDlg, IDC_STOP_SENDER);
            hwStopEspionMdpCombo = GetDlgItem(hDlg, IDC_STOP_ESPIONMDP);
            hwStopClientCombo = GetDlgItem(hDlg, IDC_STOP_CLIENT);
            hwRunOnBootCombo = GetDlgItem(hDlg, IDC_RUN_ON_BOOT);
            hwEmailEdit = GetDlgItem(hDlg, IDE_EMAIL);
            hwServeurEdit = GetDlgItem(hDlg, IDE_SERVEUR);
            hwPortEdit = GetDlgItem(hDlg, IDE_PORT);
            hwSenderSleepTimeEdit = GetDlgItem(hDlg, IDE_TEMP_RECEPTION);
            /* Récupération du titre de la fenêtre */
            char fenTitle[128];
            GetWindowText(hDlg, fenTitle, 128);
            /* Sauvegarde des données du client */
            clientIndex = lvItemActuel;
            clientId = clients[clientIndex].id;
            clientsNumberSave = actualClientsNumber;
            /* Modification du titre de la fenêtre */
            sprintf(fenTitle, "%s > %s", fenTitle, clients[clientIndex].name);
            SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)fenTitle);
            /* Affichage des icones des bouttons */
            // Update
            SendMessage(hwBtUpdate, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                        , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_REFRESH), IMAGE_ICON, 16, 16, 0));
            /* Limite de longueur des editboxs */
            PostMessage(hwEmailEdit, EM_LIMITTEXT, 127, 0);
            PostMessage(hwServeurEdit, EM_LIMITTEXT, 127, 0);
            PostMessage(hwPortEdit, EM_LIMITTEXT, 5, 0);
            PostMessage(hwSenderSleepTimeEdit, EM_LIMITTEXT, 5, 0);
            /* Remplissage des combobox */
            SendMessage(hwRunOnBootCombo, CB_ADDSTRING, 0, (LONG)"TRUE");
            SendMessage(hwRunOnBootCombo, CB_ADDSTRING, 0, (LONG)"FALSE");
            SendMessage(hwStopEspionCombo, CB_ADDSTRING, 0, (LONG)"TRUE");
            SendMessage(hwStopEspionCombo, CB_ADDSTRING, 0, (LONG)"FALSE");
            SendMessage(hwStopSenderCombo, CB_ADDSTRING, 0, (LONG)"TRUE");
            SendMessage(hwStopSenderCombo, CB_ADDSTRING, 0, (LONG)"FALSE");
            SendMessage(hwStopEspionMdpCombo, CB_ADDSTRING, 0, (LONG)"TRUE");
            SendMessage(hwStopEspionMdpCombo, CB_ADDSTRING, 0, (LONG)"FALSE");
            SendMessage(hwStopClientCombo, CB_ADDSTRING, 0, (LONG)"TRUE");
            SendMessage(hwStopClientCombo, CB_ADDSTRING, 0, (LONG)"FALSE");
            /* Désactivation des combobox */
            EnableWindow(hwRunOnBootCombo, FALSE);
            EnableWindow(hwStopClientCombo, FALSE);
            /* Récupération/Affichage des paramètres de configuration */
            write_client(&clients[clientIndex], "Send_File config", TRUE);
            break;
       case WM_COMMAND:
            switch (HIWORD(wParam))
            {
                case CBN_SELCHANGE: // si changement de séléction dans un combobox
                         switch (LOWORD(wParam))
                         {
                             case IDC_RUN_ON_BOOT:
                                  setComboBoxIcon(GetDlgItem(hDlg, IDI_RUN_ON_BOOT), SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0));
                                  break;
                             case IDC_STOP_ESPION:
                               {
                                  // pour éviter qu'Espion et EspionMdp soit tout les 2 activés
                                  // si l'EspionMdp n'est pas désactivé == FALSE => 1, on le désactive
                                  if (SendDlgItemMessage(hDlg, IDC_STOP_ESPIONMDP, CB_GETCURSEL, 0, 0) == 1)
                                  {
                                     // on change FALSE => 1 à TRUE => 0
                                     SendDlgItemMessage(hDlg, IDC_STOP_ESPIONMDP, CB_SETCURSEL, 0, 0);
                                     // on change son icône aussi
                                     setComboBoxIcon(GetDlgItem(hDlg, IDI_STOP_ESPIONMDP), 0);
                                  }
                                  
                                  setComboBoxIcon(GetDlgItem(hDlg, IDI_STOP_ESPION), SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0));
                               }
                                  break;
                             case IDC_STOP_SENDER:
                                  setComboBoxIcon(GetDlgItem(hDlg, IDI_STOP_SENDER), SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0));
                                  break;
                             case IDC_STOP_ESPIONMDP:
                               {
                                  // pour éviter qu'Espion et EspionMdp soit tout les 2 activés
                                  // si l'Espion n'est pas désactivé == FALSE => 1, on le désactive
                                  if (SendDlgItemMessage(hDlg, IDC_STOP_ESPION, CB_GETCURSEL, 0, 0) == 1)
                                  {
                                     // on change FALSE => 1 à TRUE => 0
                                     SendDlgItemMessage(hDlg, IDC_STOP_ESPION, CB_SETCURSEL, 0, 0);
                                     // on change son icône aussi
                                     setComboBoxIcon(GetDlgItem(hDlg, IDI_STOP_ESPION), 0);
                                  }
                                  
                                  setComboBoxIcon(GetDlgItem(hDlg, IDI_STOP_ESPIONMDP), SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0));
                               }
                                  break;
                             case IDC_STOP_CLIENT:
                                  setComboBoxIcon(GetDlgItem(hDlg, IDI_STOP_CLIENT), SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0));
                                  break;
                         }
                     break;
                default: // si nn
                
                     if (clientIndex != -1) // si le client a déco pas la peine de revérifier
                     {
                         // Si le client s'est déconnecté
                         if (!checkClient(hDlg, &clientIndex, clientId, TRUE, FALSE, IDB_UPDATE_CONFIG))
                             break; // obligatoire
                     }
                     else
                         break; // obligatoire
                     
                     switch (LOWORD(wParam))
                     {
                         /* Gestion des clics buttons (OK/Update - FERMER) */
                         case IDOK:
                         {
                             // vérifications (les 2 vérif. => TRUE, TRUE)
                             if (!checkClient(hDlg, &clientIndex, clientId, FALSE, TRUE, IDB_UPDATE_CONFIG))
                                 break;
                             else
                             {
                                 char email[128], serveur[128], port[6], senderSleepTime[6];
                                 // On récupère l'email
                                 GetWindowText(hwEmailEdit, email, 128);
                                 // On récupère le serveur
                                 GetWindowText(hwServeurEdit, serveur, 128);
                                 // On récupère le port
                                 GetWindowText(hwPortEdit, port, 6);
                                 int portInt = strtol(port, NULL, 10);
                                 // On récupère le temp de récéption/sender sleep time
                                 GetWindowText(hwSenderSleepTimeEdit, senderSleepTime, 6);
                                 // Début des vérification
                                 if (strlen(email) == 0 || strstr(email, "@") == NULL)
                                     MessageBox(hDlg, "Veuillez entrer un email correct !", NOM_APP, MB_OK | MB_ICONWARNING);
                                 else if (strlen(serveur) == 0)
                                     MessageBox(hDlg, "Veuillez entrer un serveur !", NOM_APP, MB_OK | MB_ICONWARNING);
                                 else if (portInt == 0 || portInt > 65535)
                                     MessageBox(hDlg, "Veuillez entrer un port (> 0 & <= 65535) !", NOM_APP, MB_OK | MB_ICONWARNING);
                                 else if (strtol(senderSleepTime, NULL, 10) == 0)
                                     MessageBox(hDlg, "Veuillez entrer un temp de récéption (> 0) !", NOM_APP, MB_OK | MB_ICONWARNING);
                                 else
                                 {// C'est bon on a tout vérifier
                             
                                     // Récupération du reste
                                     char cmd[1024], stopThread[4][5];
                                     // On récupère Stop Espion
                                     GetWindowText(hwStopEspionCombo, stopThread[0], 5);
                                     // On récupère Stop sender
                                     GetWindowText(hwStopSenderCombo, stopThread[1], 5);
                                     // On récupère Stop EspionMdp
                                     GetWindowText(hwStopEspionMdpCombo, stopThread[2], 5);
                                     // On récupère Stop Client
                                     GetWindowText(hwStopClientCombo, stopThread[3], 5);
                                     // Conversion en entier
                                     int i;
                                     for (i = 0; i < sizeof(stopThread); i++)
                                     {
                                         if (!strcmp(stopThread[i], "TRUE"))
                                             lstrcpy(stopThread[i], "1");
                                         else
                                             lstrcpy(stopThread[i], "0");
                                     }
                                     // construction de la commande
                                     sprintf(cmd, "Update_Config %s|%s|%s|%s|%s|%s|%s|%s", email, serveur, port, senderSleepTime, stopThread[0], stopThread[1], stopThread[2], stopThread[3]);
                                     // exécution de la commande
                                     write_client(&clients[clientIndex], cmd, TRUE);
                                     // on ferme la fenêtre
                                     PostMessage(hDlg, WM_COMMAND, IDCANCEL, 0);
                                 }
                             }
                         }
                         break;
                     case IDCANCEL:
                          // Si le client existe toujours/connecté
                          if (clientIndex != -1)
                             clients[clientIndex].update_config = FALSE;
                          // Activation du boutton à la fermeture
                          EnableWindow(GetDlgItem(hwFenetre, IDB_UPDATE_CONFIG), TRUE);
                          EndDialog(hDlg, 0);
                          break;
                } // fin 2ème switch
                break; // fin default
            }
            break;
   }

   return 0;
}

//=============================================================================
//                             EnterNameDialogBox
//
//=============================================================================

BOOL APIENTRY EnterNameDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   static HWND hwNameEdit;
   
   switch (uMsg)
   {
       case WM_INITDIALOG:
            /* Affichage de l'icone */
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_SEARCH)));
            /* Handle de l'editBox du nom */
            hwNameEdit = GetDlgItem(hDlg, IDE_NAME);
            /* Limite de longueur de l'editBox du nom */
            PostMessage(hwNameEdit, EM_LIMITTEXT, 200, 0);
            /* Initialisation du nom/la variable globale qui contiendra le nom */
            lstrcpy(EXPLORE_NEW_F_NAME, "");
            break;
       case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                /* Gestion des clics buttons (OK - FERMER) */
                case IDOK:
                     // on récupère le nom
                     GetWindowText(hwNameEdit, EXPLORE_NEW_F_NAME, 200);
                     // s'il est vide
                     if (strlen(EXPLORE_NEW_F_NAME) == 0)
                     {
                         MessageBox(hDlg, "Veuillez entrer un nom !", NOM_APP, MB_OK | MB_ICONWARNING);
                         break; // pour ne pas fermer la fenêtre
                     }
                     // si nn, c'est bon, on ferme la fenêtre
                case IDCANCEL:
                     EndDialog(hDlg, 0);
                     break;
            }
            break;
   }

   return 0;
}

//=============================================================================
//                             InfoDialogBox
//
//=============================================================================

BOOL APIENTRY InfoDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   static HWND hwBtAppliquer;
   static BOOL APPLIQUER;
   static char f_full_name[EXPLORE_MAX_PATH], cmd[EXPLORE_MAX_PATH];
   static int clientIndex, clientId, clientsNumberSave;
   
   switch (uMsg)
   {
       case WM_INITDIALOG:
            /* Affichage de l'icone */
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_INFO)));
            /* On récupère les parametres passé à la fenêtre */
            FenInformationsParam *fenInfoParam = (FenInformationsParam *) lParam;
            /* Sauvegarde des données du client */
            clientIndex = (*fenInfoParam).clientIndex;
            clientId = clients[clientIndex].id;
            clientsNumberSave = actualClientsNumber;
            /* Handle des controls */
            hwBtAppliquer = GetDlgItem(hDlg, IDOK);
            hwFenInformations = hDlg; // handle de la fenêtre/dialogbox
            /* Création des Tabs (onglets/volets) */
            TC_ITEM tie;
            tie.mask = TCIF_TEXT;
            /* Tab 1 */
            tie.pszText = "Général"; 
            TabCtrl_InsertItem(GetDlgItem(hDlg, INFO_TAB_CONTROL), 1, &tie);
            /* Désactivation du boutton 'Appliquer' */
            EnableWindow(hwBtAppliquer, FALSE);
            /* On récupère le chemin complet du fichier/dossier envoyé en parametre */
            lstrcpy(f_full_name, (*fenInfoParam).f_name_with_path);
            /* On affiche le nom seul */
            char *f_name = recupNomDuProgramme(f_full_name);
            SetDlgItemText(hDlg, IDE_INFO_F_NAME, f_name);
            free(f_name);
            /* on récupère l'id de l'icône et le type */
            char f_type_description[100];
            lstrcpy(f_type_description, (*fenInfoParam).f_type);
            int iconeId = getExploreIcon(f_type_description);
            /* On affiche l'icône */
            SendDlgItemMessage(hDlg, IDI_INFO_F_ICON, STM_SETIMAGE, (WPARAM)IMAGE_ICON
                                     , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(iconeId), IMAGE_ICON, 32, 32, 0));
            /* On affiche le type */
            SetDlgItemText(hDlg, TXT_INFO_F_TYPE, f_type_description);
            /* On affiche l'emplacement */
            char f_path[EXPLORE_MAX_PATH];
            lstrcpy(f_path, f_full_name);
            pathBack(f_path);
            SetDlgItemText(hDlg, TXT_INFO_F_PATH, f_path);
            /* On récupère la taille et les attributs */
            sprintf(cmd, "Get_F_Info %s", f_full_name);
            write_client(&clients[clientIndex], cmd, TRUE);
            /* Initialisation */
            APPLIQUER = FALSE;
            break;
       case WM_CTLCOLORSTATIC:
            {
            HBRUSH hbr = (HBRUSH) RGB(255, 255, 255); // Couleur blanche
            SetBkMode((HDC) wParam, TRANSPARENT); // On assure la transparence des Statics
            return (BOOL) hbr;
            }
       case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                /* Gestion des clics buttons (OK - FERMER - ...) */
                case IDC_INFO_F_SYSTEM:
                case IDC_INFO_F_HIDDEN:
                     if (!APPLIQUER)
                     {
                         EnableWindow(hwBtAppliquer, TRUE);
                         APPLIQUER = TRUE;
                     }
                     break;
                case IDOK:
                     {
                         // vérifications (les 2 vérif. => TRUE, TRUE)
                         if (!checkClient(hDlg, &clientIndex, clientId, TRUE, TRUE, -1))
                             break;
                         else
                         {
                             BOOL IS_SYSTEM = FALSE, IS_HIDDEN = FALSE;
                             // on récupère l'état des attributs (System, Caché)
                             if (IsDlgButtonChecked(hDlg, IDC_INFO_F_SYSTEM) == BST_CHECKED)
                                IS_SYSTEM = TRUE;
                             if (IsDlgButtonChecked(hDlg, IDC_INFO_F_HIDDEN) == BST_CHECKED)
                                IS_HIDDEN = TRUE;
                             // construction de la commande
                             sprintf(cmd, "Set_F_Info %s %s", IS_SYSTEM && IS_HIDDEN ? "SYSTEM&HIDDEN" : IS_SYSTEM ? "SYSTEM" : IS_HIDDEN ? "HIDDEN" : "NORMAL", f_full_name);
                             // exécution de la commande
                             write_client(&clients[clientIndex], cmd, TRUE);
                             // on désactive le boutton 'Appliquer' car on vien d'appliquer
                             EnableWindow(hwBtAppliquer, FALSE);
                             APPLIQUER = FALSE;
                         }
                     }
                     break;
                case IDCANCEL:
                     EndDialog(hDlg, 0);
                     break;
            }
            break;
   }

   return 0;
}

//=============================================================================
//                             SetSoundDialogBox
//
//=============================================================================

BOOL APIENTRY SetSoundDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   static HWND hwSoundIcon, hwSoundTrackBar;
   static int lastTrackBarPos;
   
   switch (uMsg)
   {
       case WM_INITDIALOG:
            /* Affichage de l'icone */
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_SOUND)));
            /* Handle des controls */
            hwSoundIcon = GetDlgItem(hDlg, IDI_SOUND_ICON);
            hwSoundTrackBar = GetDlgItem(hDlg, IDT_SOUND_TRACKBAR);
            /* On affiche l'icone du son (activé/désactivé), au début c'est bien évidament activé */
            SendMessage(hwSoundIcon, STM_SETIMAGE, (WPARAM)IMAGE_ICON
                                   , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_SOUND_ON), IMAGE_ICON, 24, 24, 0));
            /* Icone du boutton OK */
            SendDlgItemMessage(hDlg, IDOK, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                   , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_OK), IMAGE_ICON, 16, 16, 0));
            /* Gestion de la TrackBar */
            SendMessage(hwSoundTrackBar, TBM_SETRANGE, TRUE, MAKELONG(0, 100)); 
            SendMessage(hwSoundTrackBar, TBM_SETPOS, TRUE, 50); /* 50% */
            SendMessage(hwSoundTrackBar, TBM_SETPAGESIZE, 0, 10); /* step */
            /* Initialisation */
            lastTrackBarPos = 50;
            break;
       case WM_HSCROLL:
            switch (LOWORD(wParam))
            {
                /* Fin de mouvement de la TrackBar */              
                case TB_ENDTRACK:
                  {
                     // on récupère la position de la trackbar
                     int trackBarPos = SendMessage(hwSoundTrackBar, TBM_GETPOS, 0, 0);
                     // si position 0, donc => son désactivé, on affiche l'icone SOUND_OFF
                     if (trackBarPos == 0)
                         SendMessage(hwSoundIcon, STM_SETIMAGE, (WPARAM)IMAGE_ICON
                                                , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_SOUND_OFF), IMAGE_ICON, 24, 24, 0));
                     // si c'est une autre position, + position précédente était 0, donc => son activé, on affiche l'icone SOUND_ON
                     else if (lastTrackBarPos == 0)
                         SendMessage(hwSoundIcon, STM_SETIMAGE, (WPARAM)IMAGE_ICON
                                                , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_SOUND_ON), IMAGE_ICON, 24, 24, 0));
                     
                     lastTrackBarPos = trackBarPos; // sauvegarde (lastTrackBarPos sert juste à optimiser le changement d'icone pas plus)
                  }
                     break;
            }
            break;
       case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                /* Gestion des clics buttons (OK - FERMER) */
                case IDOK:
                     if (actualClientsNumber > 0)
                     {
                         if (lvItemActuel != -1) // Si un client est séléctionné
                         {
                            if (clients[lvItemActuel].send_file)
                               MessageBox(hDlg, "Le client est en train d'envoyer un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
                            else if (clients[lvItemActuel].recv_file)
                               MessageBox(hDlg, "Le client reçoit un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
                            else
                            {
                                // on récupère la position de la trackbar
                                int trackBarPos = SendMessage(hwSoundTrackBar, TBM_GETPOS, 0, 0);
                                // conversion en un nombre entre 0 et 65535
                                trackBarPos = (trackBarPos * 65535) / 100;
                                // on prépare la commande
                                char cmd[15];
                                sprintf(cmd, "Set_Volume %d", trackBarPos);
                                // on exécute la commande
                                write_client(&clients[lvItemActuel], cmd, TRUE);
                            }
                         }
                         else
                            MessageBox(hDlg, "Veuillez sélectionner un client !", NOM_APP, MB_OK | MB_ICONWARNING);
                      }
                      else
                         MessageBox(hDlg, "Aucun client en ligne !", NOM_APP, MB_OK | MB_ICONWARNING);
                     break;
                case IDCANCEL:
                     // Activation du boutton à la fermeture
                     EnableWindow(GetDlgItem(hwFenetre, IDB_SET_SOUND), TRUE);
                     EndDialog(hDlg, 0);
                     break;
            }
            break;
   }

   return 0;
}

//=============================================================================
//                           UpdateRegistryDialogBox
//
//=============================================================================

BOOL APIENTRY UpdateRegistryDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   static HWND hwActionCombo, hwPathCombo, hwRacineCombo, hwNomEdit, hwTypeCombo, hwValeurEdit, hwBtOk;
   static char cmd[BUF_SIZE];
   static BOOL WE_WAS_ON_DELETE;
   
   switch (uMsg)
   {
       case WM_INITDIALOG:
            /* Affichage de l'icone */
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_REGISTRY)));
            /* Handle des controls */
            hwActionCombo = GetDlgItem(hDlg, IDC_REG_ACTION);
            hwPathCombo = GetDlgItem(hDlg, IDC_REG_PATH);
            hwRacineCombo = GetDlgItem(hDlg, IDC_REG_RACINE);
            hwNomEdit = GetDlgItem(hDlg, IDE_REG_NOM);
            hwTypeCombo = GetDlgItem(hDlg, IDC_REG_TYPE);
            hwValeurEdit = GetDlgItem(hDlg, IDE_REG_VALEUR);
            hwBtOk = GetDlgItem(hDlg, IDOK);
            /* Remplissage des combobox */
            /* Action */
            SendMessage(hwActionCombo, CB_ADDSTRING, 0, (LONG)"CREER");
            SendMessage(hwActionCombo, CB_ADDSTRING, 0, (LONG)"OUVRIR");
            SendMessage(hwActionCombo, CB_ADDSTRING, 0, (LONG)"SUPPRIMER");
            SendMessage(hwActionCombo, CB_SETCURSEL, 0, 0); /* séléction du premier élément (0) */
            SetWindowText(hwBtOk, "Create"); /* changement du texte du boutton Ok */
            WE_WAS_ON_DELETE = FALSE; /* Initialisation */
            /* Path */
            SendMessage(hwPathCombo, CB_ADDSTRING, 0, (LONG)"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
            /* Racine */
            SendMessage(hwRacineCombo, CB_ADDSTRING, 0, (LONG)"HKEY_CURRENT_USER");
            SendMessage(hwRacineCombo, CB_ADDSTRING, 0, (LONG)"HKEY_CLASSES_ROOT");
            SendMessage(hwRacineCombo, CB_ADDSTRING, 0, (LONG)"HKEY_LOCAL_MACHINE");
            SendMessage(hwRacineCombo, CB_ADDSTRING, 0, (LONG)"HKEY_USERS");
            SendMessage(hwRacineCombo, CB_ADDSTRING, 0, (LONG)"HKEY_CURRENT_CONFIG");
            SendMessage(hwRacineCombo, CB_SETCURSEL, 0, 0); /* séléction du premier élément (0) */
            /* Type */
            SendMessage(hwTypeCombo, CB_ADDSTRING, 0, (LONG)"REG_BINARY");
            SendMessage(hwTypeCombo, CB_ADDSTRING, 0, (LONG)"REG_SZ");
            SendMessage(hwTypeCombo, CB_ADDSTRING, 0, (LONG)"REG_DWORD");
            SendMessage(hwTypeCombo, CB_SETCURSEL, 0, 0); /* séléction du premier élément (0) */
            /* Limite de longueur des editboxs/combobox */
            PostMessage(hwPathCombo, CB_LIMITTEXT, 511, 0);
            PostMessage(hwNomEdit, EM_LIMITTEXT, 127, 0);
            PostMessage(hwValeurEdit, EM_LIMITTEXT, 127, 0);
            break;
       case WM_COMMAND:
            switch (HIWORD(wParam))
            {
                case CBN_SELCHANGE: // si changement de séléction dans un combobox
                     if(LOWORD(wParam) == IDC_REG_ACTION)
                     {
                         switch (SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0))
                         {
                             case 0:
                                  SetWindowText(hwBtOk, "Create");
                                  if (WE_WAS_ON_DELETE)
                                  {
                                      EnableWindow(hwTypeCombo, TRUE);
                                      EnableWindow(hwValeurEdit, TRUE);
                                      WE_WAS_ON_DELETE = FALSE;
                                  }
                                  break;
                             case 1:
                                  SetWindowText(hwBtOk, "Open/Update");
                                  if (WE_WAS_ON_DELETE)
                                  {
                                      EnableWindow(hwTypeCombo, TRUE);
                                      EnableWindow(hwValeurEdit, TRUE);
                                      WE_WAS_ON_DELETE = FALSE;
                                  }
                                  break;
                             case 2:
                                  SetWindowText(hwBtOk, "Delete");
                                  EnableWindow(hwTypeCombo, FALSE);
                                  EnableWindow(hwValeurEdit, FALSE);
                                  WE_WAS_ON_DELETE = TRUE;
                                  break;
                         }
                     }
                     break;
                default:
                     switch (LOWORD(wParam))
                     {
                         /* Gestion des clics buttons (OK - FERMER) */
                         case IDOK:
                           {
                              // On récupère tout les parametres
                              char action[10], path[512], racine[20], nom[128], type[11], valeur[128];
                              // action
                              GetWindowText(hwActionCombo, action, 10);
                              // path
                              GetWindowText(hwPathCombo, path, 512);
                              // racine
                              GetWindowText(hwRacineCombo, racine, 20);
                              // nom
                              GetWindowText(hwNomEdit, nom, 128);
                              // type
                              GetWindowText(hwTypeCombo, type, 11);
                              // valeur
                              GetWindowText(hwValeurEdit, valeur, 128);
                              // Si le path/chemin est manquant
                              if (strlen(path) == 0)
                                  MessageBox(hDlg, "Veuillez entrer un path/chemin !", NOM_APP, MB_OK | MB_ICONWARNING);
                              else if (strlen(nom) == 0) // ou bien le nom
                                  MessageBox(hDlg, "Veuillez entrer un nom !", NOM_APP, MB_OK | MB_ICONWARNING);
                              else if (strlen(valeur) == 0) // ou bien la valeur
                                  MessageBox(hDlg, "Veuillez entrer une valeur !", NOM_APP, MB_OK | MB_ICONWARNING);
                              else // si nn, si tout est bon du coté des parametres
                              {
                                  // on commence les vérifications sur le client
                                  if (actualClientsNumber > 0)
                                  {
                                      if (lvItemActuel != -1) // Si un client est séléctionné
                                      {
                                          if (clients[lvItemActuel].send_file)
                                              MessageBox(hDlg, "Le client est en train d'envoyer un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
                                          else if (clients[lvItemActuel].recv_file)
                                              MessageBox(hDlg, "Le client reçoit un fichier !", NOM_APP, MB_OK | MB_ICONWARNING);
                                          else
                                          {
                                              // construction de la commande
                                              sprintf(cmd, "Reg_Set %s|%s|%s|%s|%s|%s", action, path, racine, nom, type, valeur);
                                              // exécution de la commande
                                              write_client(&clients[lvItemActuel], cmd, TRUE);
                                          }
                                      }
                                      else
                                          MessageBox(hDlg, "Veuillez sélectionner un client !", NOM_APP, MB_OK | MB_ICONWARNING);
                                  }
                                  else
                                      MessageBox(hDlg, "Aucun client en ligne !", NOM_APP, MB_OK | MB_ICONWARNING);
                              } // fin 1er else
                           }
                              break;
                         case IDCANCEL:
                              // Activation du boutton à la fermeture
                              EnableWindow(GetDlgItem(hwFenetre, IDB_UPDATE_REGISTRY), TRUE);
                              EndDialog(hDlg, 0);
                              break;
                     }
            }
            break;
   }

   return 0;
}

//=============================================================================
//                             ToDoListDialogBox
//
//=============================================================================

BOOL APIENTRY ToDoListDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   static HWND hwBtRefreshToDoList, hwBtAddToDo, hwBtDeleteToDo, hwBtFreeToDoList;
   static HWND hTTip;
   static int clientIndex, clientId, clientsNumberSave;
   
   switch (uMsg)
   {
       case WM_INITDIALOG:
            /* Affichage de l'icone */
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_TODO_LIST)));
            /* Handle de tout les controls */
            hwToDoList = GetDlgItem(hDlg, LST_TODO);
            hwBtRefreshToDoList = GetDlgItem(hDlg, IDB_REFRESH_TODO_LIST);
            hwBtAddToDo = GetDlgItem(hDlg, IDB_ADD_TODO);
            hwBtDeleteToDo = GetDlgItem(hDlg, IDB_DELETE_TODO);
            hwBtFreeToDoList = GetDlgItem(hDlg, IDB_FREE_TODO_LIST);
            /* Récupération du titre de la fenêtre */
            char fenTitle[128];
            GetWindowText(hDlg, fenTitle, 128);
            /* Sauvegarde des données du client */
            clientIndex = lvItemActuel;
            clientId = clients[clientIndex].id;
            clientsNumberSave = actualClientsNumber;
            /* Modification du titre de la fenêtre */
            sprintf(fenTitle, "%s > %s", fenTitle, clients[clientIndex].name);
            SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)fenTitle);
            /* Création d'un tooltips (qui affiche des bulbes/bules d'informations) */
             hTTip = CreateWindow(TOOLTIPS_CLASS, NULL 
             , WS_POPUP | TTS_NOPREFIX | TTS_BALLOON 
             , CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT 
             , hDlg, NULL, hInst, NULL);
             
             //SendMessage(hTTip, TTM_SETTITLE, TTI_INFO, (LPARAM)"Informations"); /* Titre des infos bulbes */
            /* Affichage des icones des bouttons + Affectation des tooltips */
            // Refresh ToDo List
            SendMessage(hwBtRefreshToDoList, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_REFRESH), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtRefreshToDoList, hDlg, hTTip, "Raffraîchir");
            // Add ToDo
            SendMessage(hwBtAddToDo, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_ADD), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtAddToDo, hDlg, hTTip, "Ajouter");
            // Delete ToDo
            SendMessage(hwBtDeleteToDo, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_STOP), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtDeleteToDo, hDlg, hTTip, "Supprimer");
            // Free ToDo List
            SendMessage(hwBtFreeToDoList, BM_SETIMAGE, (WPARAM)IMAGE_ICON
                                      , (LPARAM)LoadImage(hInst, MAKEINTRESOURCE(IDI_DELETE), IMAGE_ICON, 16, 16, 0));
            createTooltipFor(hwBtFreeToDoList, hDlg, hTTip, "Vider");
            /* Sélection par ligne dans la ListView */
            SendMessage(hwToDoList, LVM_SETEXTENDEDLISTVIEWSTYLE , 0, LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
            /* Ajout des colonnes */
            creerListViewColone(hwToDoList, 50, "ID", 0);
            creerListViewColone(hwToDoList, 250, "Commande", 1);
            creerListViewColone(hwToDoList, 150, "Temps restant avant execution (min.)", 2);
            /* Création de la bar de status */
            hwToDoListStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "", hDlg, ID_TODO_LST_STATUSBAR);
            /* Récupération/Affichage des ToDo */
            write_client(&clients[clientIndex], "List_ToDo", TRUE);
            break;
       case WM_COMMAND:
            
            if (clientIndex != -1) // si le client a déco pas la peine de revérifier
            {
                // Si le client s'est déconnecté
                if (!checkClient(hDlg, &clientIndex, clientId, TRUE, FALSE, IDB_TODO_LIST))
                    break; // obligatoire
            }
            else
                break; // obligatoire
            
            switch (LOWORD(wParam))
            {
                /* Gestion des clics buttons */
                case IDB_REFRESH_TODO_LIST:
                case IDB_ADD_TODO:
                case IDB_DELETE_TODO:
                case IDB_FREE_TODO_LIST:
                     // vérifications
                     if (!checkClient(hDlg, &clientIndex, clientId, FALSE, TRUE, IDB_TODO_LIST))
                         break;
                     else
                     {
                         if (LOWORD(wParam) == IDB_REFRESH_TODO_LIST)
                         {
                             // exécution de la commande
                             write_client(&clients[clientIndex], "List_ToDo", TRUE);
                         }
                         else if (LOWORD(wParam) == IDB_ADD_TODO)
                         {
                             // Création de la fenêtre + passage du parametre clientIndex
                             DialogBoxParam(hInst, "ADD_TODO", hDlg, (DLGPROC)AddToDoDlgProc, (LPARAM)clientIndex);
                         }
                         else if (LOWORD(wParam) == IDB_DELETE_TODO)
                         {
                             // on récupère l'index du ToDo séléctionné
                             int iSelected = SendMessage(hwToDoList, LVM_GETNEXTITEM, -1, LVNI_SELECTED); //LVNI_FOCUSED);
                             
                             // si aucun ToDo n'est séléctionné
                             if (iSelected == -1)
                             {
                                 MessageBox(hDlg, "Aucun ToDo séléctionné !", NOM_APP, MB_OK | MB_ICONWARNING);
                             }
                             else
                             {
                                 // on récupère l'id
                                 char id[10];
                                 ListView_GetItemText(hwToDoList, iSelected, 0, id, 10);
                                 // construction de la commande
                                 char cmd[256];
                                 sprintf(cmd, "Delete_ToDo %s", id);
                                 // exécution de la commande
                                 write_client(&clients[clientIndex], cmd, TRUE);
                                 // raffraîchissment de la liste
                                 if (MessageBox(hDlg, "Raffraîchir la liste ?", NOM_APP, MB_YESNO | MB_ICONQUESTION) == IDYES)
                                 {
                                     // on simule l'appui sur le boutton raffraihir (cela permet d'éviter une erreur si le client déco)
                                     PostMessage(hDlg, WM_COMMAND, IDB_REFRESH_TODO_LIST, 0);
                                 }
                             }
                         }
                         else if (LOWORD(wParam) == IDB_FREE_TODO_LIST)
                         {
                             // si la ToDo liste est vide
                             if (SendMessage(hwToDoList, LVM_GETITEMCOUNT, 0, 0) == 0)
                             {
                                 MessageBox(hDlg, "La liste est vide !", NOM_APP, MB_OK | MB_ICONWARNING);
                             }
                             else if (MessageBox(hDlg, "Voulez-vous vraiment supprimer tout les ToDo ?", NOM_APP, MB_YESNO | MB_ICONQUESTION) == IDYES)
                             {
                                 // exécution de la commande
                                 write_client(&clients[clientIndex], "Free_ToDo", TRUE);
                                 // raffraîchissment de la liste
                                 if (MessageBox(hDlg, "Raffraîchir la liste ?", NOM_APP, MB_YESNO | MB_ICONQUESTION) == IDYES)
                                 {
                                     // on simule l'appui sur le boutton raffraihir (cela permet d'éviter une erreur si le client déco)
                                     PostMessage(hDlg, WM_COMMAND, IDB_REFRESH_TODO_LIST, 0);
                                 }
                             }
                         }
                     }
                     break;
                case IDCANCEL:
                     // Si le client existe toujours/connecté
                     if (clientIndex != -1)
                        clients[clientIndex].todo_list_opened = FALSE;
                     // Activation du boutton à la fermeture
                     EnableWindow(GetDlgItem(hwFenetre, IDB_TODO_LIST), TRUE);
                     EndDialog(hDlg, 0);
                     break;
            }
            break;
        case WM_NOTIFY:
            switch(LOWORD(wParam))
	        {
                case LST_TODO: // si c'est notre listView
                   // on change les couleurs des lignes/rows
                   return customdraw_handler(hDlg, wParam, lParam, RGB(242, 242, 242));
                break;
            }
            break;
   }

   return 0;
}

//=============================================================================
//                             AddToDoDialogBox
//
//=============================================================================

BOOL APIENTRY AddToDoDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   static HWND hwBtAdd;
   static HWND hwCommandeCombo, hwTimeCombo;
   static int clientIndex, clientId, clientsNumberSave;
   
   switch (uMsg)
   {
       case WM_INITDIALOG:
            /* Affichage de l'icone */
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_TODO)));
            /* Handle de tout les controls */
            hwBtAdd = GetDlgItem(hDlg, IDOK);
            hwCommandeCombo = GetDlgItem(hDlg, IDC_TODO_COMMANDE);
            hwTimeCombo = GetDlgItem(hDlg, IDC_TODO_TIME);
            /* Récupération du titre de la fenêtre */
            char fenTitle[128];
            GetWindowText(hDlg, fenTitle, 128);
            /* Sauvegarde des données du client */
            clientIndex = lParam;
            clientId = clients[clientIndex].id;
            clientsNumberSave = actualClientsNumber;
            /* Modification du titre de la fenêtre */
            sprintf(fenTitle, "%s > %s", fenTitle, clients[clientIndex].name);
            SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)fenTitle);
            /* Remplissage des combobox */
            /* Commande */
            SendMessage(hwCommandeCombo, CB_ADDSTRING, 0, (LONG)"Turn_Off");
            SendMessage(hwCommandeCombo, CB_ADDSTRING, 0, (LONG)"Reboot");
            SendMessage(hwCommandeCombo, CB_ADDSTRING, 0, (LONG)"Black_Screen");
            SendMessage(hwCommandeCombo, CB_ADDSTRING, 0, (LONG)"Adsl_Disconnect");
            SendMessage(hwCommandeCombo, CB_ADDSTRING, 0, (LONG)"Shell_Exec notepad");
            SendMessage(hwCommandeCombo, CB_ADDSTRING, 0, (LONG)"Stop_Prg program.exe");
            SendMessage(hwCommandeCombo, CB_ADDSTRING, 0, (LONG)"Block_Site www.example.com");
            /* Time */
            SendMessage(hwTimeCombo, CB_ADDSTRING, 0, (LONG)"5");
            SendMessage(hwTimeCombo, CB_ADDSTRING, 0, (LONG)"10");
            SendMessage(hwTimeCombo, CB_ADDSTRING, 0, (LONG)"15");
            SendMessage(hwTimeCombo, CB_ADDSTRING, 0, (LONG)"20");
            SendMessage(hwTimeCombo, CB_ADDSTRING, 0, (LONG)"30");
            SendMessage(hwTimeCombo, CB_ADDSTRING, 0, (LONG)"60");
            SendMessage(hwTimeCombo, CB_ADDSTRING, 0, (LONG)"120");
            SendMessage(hwTimeCombo, CB_ADDSTRING, 0, (LONG)"180");
            SendMessage(hwTimeCombo, CB_ADDSTRING, 0, (LONG)"240");
            /* Limite de longueur des combobox */
            PostMessage(hwCommandeCombo, CB_LIMITTEXT, BUF_SIZE, 0);
            PostMessage(hwTimeCombo, CB_LIMITTEXT, 10, 0);
            break;
       case WM_COMMAND:
            
            if (clientIndex != -1) // si le client a déco pas la peine de revérifier
            {
                // Si le client s'est déconnecté
                if (!checkClient(hDlg, &clientIndex, clientId, TRUE, FALSE, -1))
                    break; // obligatoire
            }
            else
                break; // obligatoire
            
            switch (LOWORD(wParam))
            {
                /* Gestion des clics buttons (OK - FERMER) */
                case IDOK:
                      // vérifications
                      if (!checkClient(hDlg, &clientIndex, clientId, FALSE, TRUE, -1))
                          break;
                      else
                      {
                          // On récupère tout les parametres
                          char time[10], commande[BUF_SIZE], cmd[BUF_SIZE];
                          // time
                          GetWindowText(hwTimeCombo, time, 10);
                          // commande
                          GetWindowText(hwCommandeCombo, commande, BUF_SIZE);
                          // Si un parametre est manquant
                          if (strlen(commande) == 0) // ou bien le nom
                              MessageBox(hDlg, "Veuillez entrer une commande !", NOM_APP, MB_OK | MB_ICONWARNING);
                          else if (strlen(time) == 0)
                              MessageBox(hDlg, "Veuillez entrer un temps d'exécution !", NOM_APP, MB_OK | MB_ICONWARNING);
                          else if (strtol(time, NULL, 10) <= 0)
                              MessageBox(hDlg, "Temps d'exécution incorrect !", NOM_APP, MB_OK | MB_ICONWARNING);
                          else // si nn, si tout est bon du coté des parametres
                          {
                               // construction de la commande
                               sprintf(cmd, "Add_ToDo %s|%s", time, commande);
                               // exécution de la commande
                               write_client(&clients[lvItemActuel], cmd, TRUE);
                               // raffraîchissment de la liste
                               if (MessageBox(hDlg, "Raffraîchir la liste ?", NOM_APP, MB_YESNO | MB_ICONQUESTION) == IDYES)
                               {
                                   // exécution de la commande
                                   write_client(&clients[clientIndex], "List_ToDo", TRUE);
                               }
                               // fermeture de la fenêtre 
                               EndDialog(hDlg, 0);
                          }
                      }
                      break;
                case IDCANCEL:
                    EndDialog(hDlg, 0);
                    break;
            }
            break;
   }

   return 0;
}

//=============================================================================
//                             SettingsDialogBox
//
//=============================================================================

BOOL APIENTRY SettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   static HWND hwCommandeLine;
   
   switch (uMsg)
   {
       case WM_INITDIALOG:
            /* Affichage de l'icone */
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_CONFIG)));
            /* Handle des controls */
            hwCommandeLine = GetDlgItem(hwFenetre, IDC_COMMANDELINE);
            /* Création des Tabs (onglets/volets) */
            TC_ITEM tie;
            tie.mask = TCIF_TEXT;
            /* Tab 1 */
            tie.pszText = "Général"; 
            TabCtrl_InsertItem(GetDlgItem(hDlg, SETTINGS_TAB_CONTROL), 1, &tie);
            /* Tab 2 */
            tie.pszText = "Explore Drive"; 
            TabCtrl_InsertItem(GetDlgItem(hDlg, SETTINGS_TAB_CONTROL), 2, &tie);
            /* On cache les éléments des onglets non actif */
            setSettingsTab(hDlg, HIDE_TAB, EXPLORE_DRIVE_TAB);
            /* On coche les checkboxs s'il le faut */
            CheckDlgButton(hDlg, IDC_CRYPT_DECRYPT, CRYPT_DECRYPT ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hDlg, IDC_CHECK_COMMANDS, CHECK_COMMANDS ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hDlg, IDC_SHOW_EXPLORE_TYPE, SHOW_EXPLORE_TYPE ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(hDlg, IDC_SHOW_EXPLORE_EXT, SHOW_EXPLORE_EXT ? BST_CHECKED : BST_UNCHECKED);
            break;
       case WM_CTLCOLORSTATIC:
            {
            HBRUSH hbr = (HBRUSH) RGB(255, 255, 255); // Couleur blanche
            SetBkMode((HDC) wParam, TRANSPARENT); // On assure la transparence des Statics
            return (BOOL) hbr;
            }
       case WM_NOTIFY:
            {
            LPNMHDR pnmhdr = (LPNMHDR)lParam;
            switch (pnmhdr->code)
            {
                case TCN_SELCHANGE:
                     switch (TabCtrl_GetCurSel(GetDlgItem(hDlg, SETTINGS_TAB_CONTROL)))
                     {
                         case 0: /* Tab 1 */
                              /* On cache les éléments de l'onglet Explore Drive */
                              setSettingsTab(hDlg, HIDE_TAB, EXPLORE_DRIVE_TAB);
                              /* On affiche les éléments de l'onglet Général */
                              setSettingsTab(hDlg, SHOW_TAB, GENERAL_TAB);
                              break;
                         case 1: /* Tab 2 */
                              /* On cache les éléments de l'onglet Général */
                              setSettingsTab(hDlg, HIDE_TAB, GENERAL_TAB);
                              /* On affiche les éléments de l'onglet Explore Drive */
                              setSettingsTab(hDlg, SHOW_TAB, EXPLORE_DRIVE_TAB);
                              break;
                     }
                     break;              
            }
            }
            break;
       case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                /* Gestion des clics buttons (OK - FERMER - ...) */
                case IDC_CRYPT_DECRYPT:
                     CRYPT_DECRYPT = ! CRYPT_DECRYPT;
                     break;
                case IDC_CHECK_COMMANDS:
                     CHECK_COMMANDS = ! CHECK_COMMANDS;
                     break;
                case IDB_FREE_CMD_HISTORY:
                     // Vidage
                     SendMessage(hwCommandeLine, CB_RESETCONTENT, 0, 0);
                     // Fermeture de la fenêtre 
                     PostMessage(hDlg, WM_COMMAND, IDCANCEL, 0); //EndDialog(hDlg, 0);
                     break;
                case IDC_SHOW_EXPLORE_TYPE:
                     SHOW_EXPLORE_TYPE = ! SHOW_EXPLORE_TYPE;
                     break;
                case IDC_SHOW_EXPLORE_EXT:
                     SHOW_EXPLORE_EXT = ! SHOW_EXPLORE_EXT;
                     break;
                case IDOK:
                     CRYPT_DECRYPT = CRYPT_DECRYPT_DEFAULT;
                     CheckDlgButton(hDlg, IDC_CRYPT_DECRYPT, CRYPT_DECRYPT ? BST_CHECKED : BST_UNCHECKED);
                     CHECK_COMMANDS = CHECK_COMMANDS_DEFAULT;
                     CheckDlgButton(hDlg, IDC_CHECK_COMMANDS, CHECK_COMMANDS ? BST_CHECKED : BST_UNCHECKED);
                     SHOW_EXPLORE_TYPE = SHOW_EXPLORE_TYPE_DEFAULT;
                     CheckDlgButton(hDlg, IDC_SHOW_EXPLORE_TYPE, SHOW_EXPLORE_TYPE ? BST_CHECKED : BST_UNCHECKED);
                     SHOW_EXPLORE_EXT = SHOW_EXPLORE_EXT_DEFAULT;
                     CheckDlgButton(hDlg, IDC_SHOW_EXPLORE_EXT, SHOW_EXPLORE_EXT ? BST_CHECKED : BST_UNCHECKED);
                     break;
                case IDCANCEL:
                     EndDialog(hDlg, 0);
                     break;
            }
            break;
   }

   return 0;
}
