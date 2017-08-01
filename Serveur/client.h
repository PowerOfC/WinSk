//=============================================================================
// Projet : Winrell (server)
// Fichier : client.h
//
//=============================================================================

#ifndef CLIENT_H
#define CLIENT_H

//=============================================================================
//                               Constantes
//=============================================================================

#define CRLF		          "\r\n"
#define PORT                  50999
#define MAX_CLIENTS 	      100

#define BUF_SIZE	          1024

//=============================================================================
//                          Structure ScreenShowBtnId
//
//=============================================================================

typedef struct {
    int stopContinue;
    int save;
    int close;
    int speed_fast; // 1
    int speed_normal; // 3
    int speed_slow; // 5
    int quality_high; // 100
    int quality_normal; // 70
    int quality_low; // 5
    int mouse_control_on;
    int mouse_control_off;
} ScreenShowBtnId;

//=============================================================================
//                          Structure IMG_INFO
//
//=============================================================================

typedef struct tagImgInfo {
  IPicture *Ipic;
  SIZE sizeInHiMetric;
  SIZE sizeInPix;
  char *Path;
} IMG_INFO;

//=============================================================================
//                          Structure ScreenShow
//
//=============================================================================

typedef struct {
    HWND hwnd;
    HMENU hMenu;
    HANDLE hThread;
    IMG_INFO ImageInfo;
    char imagePath[2048];
    BOOL stop;
    BOOL saveImages;
    int speed;
    int quality;
    BOOL mouseControl;
    BOOL doubleClic;
    int clicks;
    ScreenShowBtnId btn_id;
} ScreenShow;

//=============================================================================
//                              Structure Client
//
//=============================================================================

typedef struct
{
   SOCKET sock; // il vaut mieu laisser sock en premier
   int id;
   char name[BUF_SIZE]; // la taille doit être == à BUF_SIZE, à ne pas changer
   BOOL recv_file, send_file;
   char file_name[BUF_SIZE];
   BOOL explore_drive;
   BOOL task_list_opened;
   BOOL update_config;
   BOOL show_screen;
   BOOL todo_list_opened;
   ScreenShow show_screen_infos;
} Client;

//=============================================================================
//                         Structure UploadDownload
//
//=============================================================================

typedef struct {
    char fileName[BUF_SIZE];
    char fileSize[100];
    char state[26];
    int iProgress;
    char client[BUF_SIZE];
    char details[200];
    char startAt[32];
    char endAt[32];
    BOOL isDownload;
    BOOL isUpload;
} UploadDownload;

//=============================================================================
//                              Structure Clic
//
//=============================================================================

typedef struct {
    POINT pt;
    RECT rc;
    int clientIndex;
    int clientId;
    int clientsNumberSave;
} Clic;

#endif
