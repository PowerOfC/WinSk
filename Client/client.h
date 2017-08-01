//=============================================================================
// Projet : WinSk
// Fichier : client.h
//
//=============================================================================

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "resources.h" // important


//=============================================================================
//                              Constantes
//=============================================================================

/*
#if (TEST_VERSION == 1)
   #define SERVER_IP_ADRESS         "127.0.0.1"//"192.168.1.30"
#else
   #define SERVER_IP_ADRESS         "127.0.0.1"
#endif

#define SERVER_PORT	                50999
*/

#define RECEIVED_RESPONSE           "Okay !"
#define UPDATE_RESPONSE             "Going Update Now, see u !"
#define SUCCESS_RESPONSE            "Done !"
#define ERROR_RESPONSE              "I don't understand.."

#define CRLF	      "\r\n"
#define BUF_SIZE      1024
#define MAX_RECV_BUF  256
#define MAX_SEND_BUF  256

//=============================================================================
//                          Variables globales
//=============================================================================

BOOL WE_RECV_FILE;
char fileRecvName[200];

//=============================================================================
//                  Prototypes des fonctions internes
//=============================================================================

int init_connection(char *address, int port);
void end_connection(int sock);
int read_server(SOCKET sock, char *buffer);
void write_server(SOCKET sock, char *buffer);
int send_file(SOCKET sock, char *file_name);
int recv_file(SOCKET sock, char* file_name);
void effectuerCommande(SOCKET sock, char *buffer, BOOL reply);

#endif
