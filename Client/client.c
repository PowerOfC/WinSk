//=============================================================================
// Projet : WinSk
// Fichier : client.c
//
//=============================================================================

#include "client.h"


//=============================================================================
//                   Fonction de gestion du client
//=============================================================================

void app(char *name, char *adress, int port)
{
   SOCKET sock = init_connection(adress, port);
   char buffer[BUF_SIZE];

   fd_set rdfs;
   
   // initialisation
   WE_RECV_FILE = FALSE;

   /* send our name */
   write_server(sock, name);
   free(name); // name is a pointer to another pointer (char*)

   while(1)
   {
      FD_ZERO(&rdfs);

      /* add the socket */
      FD_SET(sock, &rdfs);

      if(select(sock + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         //perror("select()");
         return;//exit(errno);
      }

      if(FD_ISSET(sock, &rdfs))
      {
         int n = read_server(sock, buffer);
         /* server down */
         if(n == 0)
         {
            CONNECTED_TO_SERVER = FALSE;
            //printf("Server disconnected !\n");
            #if NO_GUI == FALSE
            appendToRichConsole("Server disconnected !", buffer);
            #endif
            break;
         }
         //puts(buffer);
      }
   }

   end_connection(sock);
}

//=============================================================================
//               Fonctions qui effectue différents traitements
//     (initialisation socket, envoie, reception, fermeture socket, ...)
//=============================================================================

void init(void)
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

void end(void)
{
    #ifdef WIN32
       WSACleanup();
    #endif
}

int init_connection(char *address, int port)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };
   struct hostent *hostinfo;

   if(sock == INVALID_SOCKET)
   {
      //perror("socket()");
      return;//exit(errno);
   }
   
   #if NO_GUI == FALSE
   appendToRichConsole("CONNECTING TO SERVER...", "");
   char tmp[BUF_SIZE];
   sprintf(tmp, "ADDRESS : %s, PORT : %d", address, port);
   appendToRichConsole(tmp, "");
   #endif

   hostinfo = gethostbyname(address);
   if (hostinfo == NULL)
   {
      //fprintf (stderr, "Unknown host %s.\n", address);
      #if NO_GUI == FALSE
      appendToRichConsole("Unknown host", address);
      #endif
      return;//exit(EXIT_FAILURE);
   }

   sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr;
   sin.sin_port = htons(port);
   sin.sin_family = AF_INET;

   if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
   {
      //perror("connect()");
      #if NO_GUI == FALSE
      appendToRichConsole("CONNECTED TO SERVER", "KO");
      #endif
      return;//exit(errno);
   }
   else
   {
      CONNECTED_TO_SERVER = TRUE;
      serverSock = sock;
      #if NO_GUI == FALSE
      appendToRichConsole("CONNECTED TO SERVER", "OK");
      #endif
   }

   return sock;
}

void end_connection(int sock)
{
   closesocket(sock);
}

//=============================================================================
//             Fonctions d'envoie/réception des données/commandes
//=============================================================================

int read_server(SOCKET sock, char *buffer)
{
   int n = 0;
   
   if (!WE_RECV_FILE)
   {
       if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
       {
           //perror("recv()");
           return 0;//exit(errno);
       }

       buffer[n] = 0;
       
       #if CRYPT_DECRYPT == TRUE
       //Crypt(buffer, strlen(buffer), CRYPT_KEY, strlen(CRYPT_KEY)); // décryptage
       xor(buffer, CRYPT_KEY); // décryptage
       #endif
       
       //printf("recu : %s\n", buffer);
       #if NO_GUI == FALSE
       appendToRichConsole("recv", buffer);
       #endif
       effectuerCommande(sock, buffer, TRUE);
   }
   else
   {
      n = recv_file(sock, fileRecvName);
      WE_RECV_FILE = FALSE;
   }

   return n;
}

void write_server(SOCKET sock, char *buffer)
{
   char msg[BUF_SIZE]; // msg sert à éviter les erreurs lorsque buffer pointe sur une constante(vu que le cryptage modifie la valeur passée)
   lstrcpy(msg, buffer);
   
   #if CRYPT_DECRYPT == TRUE
   //Crypt(msg, strlen(msg), CRYPT_KEY, strlen(CRYPT_KEY)); // cryptage
   xor(msg, CRYPT_KEY); // cryptage
   #endif
   
   if(send(sock, msg, strlen(msg), 0) < 0)
   {
      //perror("send()");
      return;//exit(errno);
   }
   //printf("envoyee : %s\n", buffer);
   #if NO_GUI == FALSE
   appendToRichConsole("send", buffer);
   #endif
}

//=============================================================================
//             Fonctions d'envoie/réception de fichiers
//=============================================================================

int send_file(SOCKET sock, char *file_name)
{
   int sent_count; /* how many sending chunks, for debugging */
   ssize_t read_bytes, /* bytes read from local file */
   sent_bytes, /* bytes sent to connected socket */
   sent_file_size; 
   char send_buf[MAX_SEND_BUF]; /* max chunk size for sending file */

   FILE *f = NULL; /* file handle for reading local file*/
 
   sent_count = 0;
   sent_file_size = 0;
 
   /* attempt to open requested file for reading */
   if( (f = fopen(file_name, "rb")) == NULL) /* can't open requested file */
   {
      //perror(file_name);
      char tmp[500];
      sprintf(tmp, "%s [No such File or Directory]", file_name);
      //write_server(sock, tmp);
      send(sock, tmp, strlen(tmp), 0);
      return -1;
   }
   else /* open file successful */
   {
      // send file size
      long file_size = tailleFichier(file_name, "", TRUE);
      sprintf(send_buf, "%d", file_size);
      send(sock, send_buf, strlen(send_buf), 0);
      // if file_size == 0
      if (file_size == 0)
      {
         #if NO_GUI == FALSE
         appendToRichConsole("file_size = 0, Sending file", "KO");
         #endif
         fclose(f); // close file (because we have open it)
         return -1;
      }
      // wait for/get response
      recv(sock, send_buf, BUF_SIZE - 1, 0);
      
      //printf("Sending file: %s\n", file_name);
      #if NO_GUI == FALSE
      appendToRichConsole("Sending file", file_name);
      lockGUI();
      #endif
      while( (read_bytes = fread(send_buf, sizeof(char), MAX_RECV_BUF, f)) > 0 )
      {
         if( (sent_bytes = send(sock, send_buf, read_bytes, 0)) < read_bytes )
         {
            //perror("send error");
            return -1;
         }
         
         sent_count++;
         sent_file_size += sent_bytes;
      }
      
      fclose(f);
   } /* end else */
   
   //printf("Done with this file. Sent %d bytes in %d send(s)\n", sent_file_size, sent_count);
   #if NO_GUI == FALSE
   char tmp[BUF_SIZE];
   sprintf(tmp, "Sent %d bytes in %d send(s)", sent_file_size, sent_count);
   appendToRichConsole("Done with this file", tmp);
   unlockGUI();
   #endif
   return sent_count;
}

int recv_file(SOCKET sock, char* file_name)
{
    char send_str [MAX_SEND_BUF]; /* message to be sent to server*/
    FILE * f = NULL; /* file handle for receiving file*/
    ssize_t sent_bytes, rcvd_bytes, rcvd_file_size;
    int recv_count; /* count of recv() calls*/
    char recv_str[MAX_RECV_BUF]; /* buffer to hold received data */
    size_t send_strlen; /* length of transmitted string */
 
    /* attempt to create file to save received data. 0644 = rw-r--r-- */
    if ( (f = fopen(file_name, "wb")) == NULL )
    {
       //perror("error creating file");
       return -1;
    }
 
    recv_count = 0; /* number of recv() calls required to receive the file */
    rcvd_file_size = 0; /* size of received file */
 
    // On récupère la taille du fichier
    int n = recv(sock, recv_str, MAX_RECV_BUF, 0);
    recv_str[n] = 0; // fin de chaine
    long file_size = atol(recv_str);
    /* continue receiving until ? (data or close) */
    int end = 0;
    //printf("start downloading...\n");
    #if NO_GUI == FALSE
    appendToRichConsole("Start downloading...", "");
    lockGUI();
    #endif
    
    // On renvoie une reponse (pour le début de la récéption)
    char reponse[] = "OK"; // peut importe la réponse, elle sert juste à synchroniser/garder l'équilibre entre le serveur et le client
    //send(sock, reponse, strlen(reponse), 0);
    write_server(sock, reponse); // on l'envoie cryptée, car le serveur va la décrypter (vu qu'il utilise des threads)
    
    while(!end)
    {
       if ((rcvd_bytes = recv(sock, recv_str, MAX_RECV_BUF, 0)) < 0 )
       {
          //perror("recv error");
          return 0;
       }
       
       recv_count++;
       rcvd_file_size += rcvd_bytes;
       
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
    //printf("Received: %d bytes in %d recv(s)\n", rcvd_file_size, recv_count);
    #if NO_GUI == FALSE
    sprintf(send_str, "%d bytes received in %d recv(s)", rcvd_file_size, recv_count);
    sprintf(recv_str, "%s %s", file_name, send_str);
    appendToRichConsole("Finish downloading", recv_str);
    unlockGUI();
    #endif
    return rcvd_file_size;
}
