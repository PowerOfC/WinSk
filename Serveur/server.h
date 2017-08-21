//=============================================================================
// Projet : Winrell (server)
// Fichier : server.h
//
//=============================================================================

#ifndef SERVER_H
#define SERVER_H

#define _WIN32_IE 0x0500 // Pour la structure NOTIFYICONDATA
#define TTS_BALLOON 0x40 // Pour le tooltips
#define TTM_SETTITLE (WM_USER + 32)
#define TTI_INFO 1
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <windows.h>
#include <commctrl.h> // -lcomctl32
#include <richedit.h>
#include <winsock2.h> // -lws2_32
#include "ocidl.h"
#include "olectl.h" // -loleaut32 -luuid
#include <windowsx.h>
#include "client.h"

//=============================================================================
//                               Constantes
//=============================================================================

#define NOM_APP               "Winrell Server 3.0"
#define MY_WM_NOTIFYICON      WM_USER+1

#define CRYPT_DECRYPT_DEFAULT FALSE // CRYPT_DECRYPT est maintenant une variable BOOL qui prend par défaut la valeur de cette constante
#define CRYPT_KEY             956636 // @@ entier
//#define CRYPT_KEY             "" // si chaine vide, le mdp par défaut de la fonction crypt sera utilisé

#define CHECK_COMMANDS_DEFAULT      TRUE
#define ENABLE_COLORIZATION_DEFAULT TRUE
#define SHOW_EXPLORE_TYPE_DEFAULT   TRUE
#define SHOW_EXPLORE_EXT_DEFAULT    FALSE

#define IDI_ICONE             100
#define IDI_LOCAL             101
#define IDI_MACHINE           102
//#define IDI_BACKGROUND        103
//#define IDI_CMD               104
//#define IDI_FONT              105
#define IDI_SEARCH            106
#define IDI_STOP              107
#define IDI_VIDEO             108
#define IDI_DOWNLOAD          109
#define IDI_DRIVE             110
#define IDI_EXPLORE           111
#define IDI_EXPLORE_BACK      112
#define IDI_FOLDER            113
#define IDI_FILE              114
#define IDI_TEXT_FILE         115
#define IDI_IMAGE             116
#define IDI_SON               117
#define IDI_EXE               118
#define IDI_DELETE            119
#define IDI_RUN               120
#define IDI_UPLOAD            121
#define IDI_TASK              122
#define IDI_TURN_OFF          123
#define IDI_ADD               124
#define IDI_REFRESH           125
#define IDI_CONFIG            126
#define IDI_TRUE              127
#define IDI_FALSE             128
#define IDI_IN_OUT            129
#define IDI_COPY              130
#define IDI_CUT               131
#define IDI_PASTE             132
#define IDI_NEW_FOLDER        133
#define IDI_RENAME            134
#define IDI_INFO              135
#define IDI_DOC_WORD          136
#define IDI_DOC_EXCEL         137
#define IDI_DOC_POWERPOINT    138
#define IDI_DOC_ACCESS        139
#define IDI_ZIP_RAR           140
#define IDI_PDF               141
#define IDI_SOUND             142
#define IDI_SOUND_ON          143
#define IDI_SOUND_OFF         144
#define IDI_OK                145
#define IDI_REGISTRY          146
#define IDI_TODO              147
#define IDI_TODO_LIST         148
#define IDI_DISCONNECT        149

#define IDM_QUITTER           201
#define IDM_APROPOS           202

#define IDB_SEND              203
#define IDC_COMMANDELINE      204
#define IDB_TASKS_LIST        205
#define IDM_CLEAR_CONSOLE     206
#define IDM_CONSOLE_COULEUR   207
#define IDM_TEXTE_COULEUR     208
#define SETTINGS_TAB_CONTROL  209
#define LST_COMMANDES         210
#define IDB_LAUNCH_LST_CMD    211
#define IDB_SEND_TO_ALL       212
#define IDB_SHOW_SCREEN       213
#define IDC_ENREGISTRER       214
#define CB_WIDTH              215
#define CB_HEIGHT             216
#define IDE_FREQ_SECONDE      217
#define IDC_CRYPT_DECRYPT     218
#define IDB_EXPLORE_DRIVE     219
#define LST_EXPLORE_DRIVE     220
#define IDE_EXPLORE_PATH      221
#define IDB_BACK_TO_ROOT      222
#define IDB_EXPLORE_BACK      223
#define IDB_EXPLORE_DIRECTORY 224
#define IDB_DOWNLOAD_FILE     225
#define IDB_DELETE            226
#define ID_EXPLORE_STATUSBAR  227
#define ID_STATUSBAR          228
#define ID_COPIER_SELECTION   229
#define LST_CLIENTS           230
#define IDB_DISCONNECT        231
#define IDB_UPDATE_CONFIG     232
#define IDB_TURN_OFF          233
#define IDB_UPLOAD_FILE       234
#define IDB_RUN_FILE          235
#define IDE_SELECTED_TASK     236
#define IDB_STOP_TASK         237
#define LST_TASKS             238
#define IDB_REFRESH_TASK_LIST 239
#define ID_TASK_LST_STATUSBAR 240
#define IDC_TURN_OFF          241
#define IDE_EMAIL             242
#define IDE_SERVEUR           243
#define IDE_PORT              244
#define IDE_TEMP_RECEPTION    245
#define IDE_CURRENT_DIR       246
#define IDE_TASK_NAME         247
#define IDC_RUN_ON_BOOT       248
#define IDC_STOP_ESPION       249
#define IDC_STOP_SENDER       250
#define IDC_STOP_ESPIONMDP    251
#define IDC_STOP_CLIENT       252
#define IDE_VERSION           253
#define IDI_RUN_ON_BOOT       254
#define IDI_STOP_ESPION       255
#define IDI_STOP_SENDER       256
#define IDI_STOP_ESPIONMDP    257
#define IDI_STOP_CLIENT       258
#define IDB_DOWNS_UPLS_LIST   259
#define LST_DOWNLOADS_UPLOADS 260
#define IDC_ENABLE_COLORIZATION 261
#define IDB_NEW_FOLDER        262
#define IDB_COPY              263
#define IDB_CUT               264
#define IDB_PASTE             265
#define IDB_RENAME            266
#define IDE_NAME              267
#define IDB_INFO              268
#define INFO_TAB_CONTROL      269
#define IDI_INFO_F_ICON       270
#define IDE_INFO_F_NAME       271
#define TXT_INFO_F_TYPE       272
#define TXT_INFO_F_PATH       273
#define TXT_INFO_F_SIZE       274
#define IDC_INFO_F_SYSTEM     275
#define IDC_INFO_F_HIDDEN     276
#define IDB_SET_SOUND         277
#define IDI_SOUND_ICON        278
#define IDT_SOUND_TRACKBAR    279
#define IDB_UPDATE_REGISTRY   280
#define IDC_REG_ACTION        281
#define IDC_REG_PATH          282
#define IDC_REG_RACINE        283
#define IDE_REG_NOM           284
#define IDC_REG_TYPE          285
#define IDE_REG_VALEUR        286
#define IDC_TIME              287
#define IDB_TODO_LIST         288
#define ID_TODO_LST_STATUSBAR 289
#define LST_TODO              290
#define IDB_ADD_TODO          291
#define IDB_DELETE_TODO       292
#define IDB_FREE_TODO_LIST    293
#define IDB_REFRESH_TODO_LIST 294
#define IDC_TODO_COMMANDE     295
#define IDC_TODO_TIME         296
#define IDC_CHECK_COMMANDS    297
#define IDC_SHOW_EXPLORE_TYPE 298
#define IDC_SHOW_EXPLORE_EXT  299

#define IDM_OPEN_LOG          400
#define IDM_AUTOSHOW_D_U_LIST 401
#define IDM_SETTINGS          402
#define IDM_OPEN_CMD_LIST     403
#define IDM_REFRESH_CONSOLE   404
#define IDM_HIDE_CONSOLE_MSG  405
#define IDM_FREE_CMD_HISTORY  406


#define SCREEN_X              GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_Y              GetSystemMetrics(SM_CYSCREEN)

#define L_FENETRE             350
#define H_FENETRE             740

#define PX_ECRAN              SCREEN_X - L_FENETRE - 5
#define PY_ECRAN              5//SCREEN_Y - H_FENETRE

#define L_FENETRE_2           500
#define H_FENETRE_2           550

#define L_SC_FENETRE          300//SCREEN_X - L_FENETRE - 10
#define H_SC_FENETRE          200//H_FENETRE

#define PX_ECRAN_2            (SCREEN_X / 2) - (L_FENETRE_2 / 2)
#define PY_ECRAN_2            (SCREEN_Y / 2) - (H_FENETRE_2 / 2)

#define MAX_RECV_BUF          256 // pour la réception de fichier
#define MAX_SEND_BUF          256

#define LOG_FOLDER            "logs"
#define EXPLORE_MAX_PATH      2048

/* Couleurs des textes spéciaux de la RichConsole */
//#define TEXT_COLOR            RGB(255, 255, 255)
#define SEND_COLOR            RGB(0, 255, 0)
#define RECV_COLOR            RGB(0, 188, 242)
#define CANCEL_COLOR          RGB(255, 242, 0)
#define WARNING_COLOR         RGB(255, 127, 39)
#define ERROR_COLOR           RGB(255, 0, 0)

//=============================================================================
//                           Variables globales
//
// @ Initialiser dans la section : /* Initialisation des variables globales */
//=============================================================================

HINSTANCE hInst;

NOTIFYICONDATA TrayIcon;

BOOL REFRESH_CONSOLE, IS_MAIN_WINDOW_ACTIVE, SEND_TO_ALL, HIDE_CONSOLE_MSG, AUTOSHOW_DOWNLOAD_UPLOAD_LIST, ENABLE_COLORIZATION;

BOOL CRYPT_DECRYPT, CHECK_COMMANDS, SHOW_EXPLORE_TYPE, SHOW_EXPLORE_EXT;

HWND hwFenetre, hwFenUpdateConfig, hwFenInformations;

HWND hwFenDownloadsUploads, hwDownloadsUploadsListView;

HWND hwClientsList, hwRichConsole, hwStatusBar, hwListeExploration, hwExploreStatusBar, hwTaskList, hwTaskListStatusBar;

HWND hwToDoList, hwToDoListStatusBar;

int lvItemActuel;

char fichierLog[200], APP_PATH[MAX_PATH], EXPLORE_NEW_F_NAME[200];

/* an array for all clients */
Client clients[MAX_CLIENTS];

int actualClientsNumber;

WNDPROC oldProc;

COLORREF TextColor;

enum{GENERAL_TAB = 1, EXPLORE_DRIVE_TAB};

enum{SHOW_TAB, HIDE_TAB};

//=============================================================================
//                       Prototypes des fonctions
//
//=============================================================================

/* main.c */
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK editProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

/* dialogBoxs.c */
BOOL APIENTRY AproposDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY CommandeListeDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY ExploreDriveDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY TaskListDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY TurnOffDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY UpdateConfigDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY EnterNameDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY InfoDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY SetSoundDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY UpdateRegistryDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY ToDoListDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY AddToDoDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL APIENTRY SettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

/* server.c */
void init();
void end();
void app();
int init_connection();
void end_connection(int sock);
int read_socket(SOCKET sock, char *buffer);
int read_client(Client *client, char *buffer);
void write_client(Client *client, char *buffer, BOOL appendToConsole);
void send_message_to_all_clients(Client *clientsTab, int actualClientsNumber, char *buffer);
void remove_client(Client *clients, int to_remove, int *actualClientsNumber);
void clear_clients(Client *clients, int actualClientsNumber);
int recv_file(int clientId);
int send_file(int clientId);
void disconnect_client(int clientIndice, BOOL closeSocket);

/* fct.c */
void appendToRichConsole(char *title, const char *buffer);
void ajouterALaListView(char *texte, int tailleTexte, char *ip);
char *get_time(short format);
BOOL enregistrerDansLog(char *log, char *fichier, BOOL ecraserAncien);
BOOL lireLogEtAfficherSurRichConsole(char *fichier);
void afficherSurRichConsole(char *texteAafficher);
void resetRichConsoleTextColor();
void richConsoleColorization(char *texte, int len);
void changerCouleurRichEdit(HWND hEdit, COLORREF couleur);
void changerCouleurSelectionRichEdit(HWND hEdit, CHARRANGE Selection, COLORREF couleur);
BOOL verifierOsWindowsXPorLater();
void afficherTrayIconBallon(char *titre, char *contenu, unsigned int temp, BOOL icone);
char *recupNomDuProgramme(char *path);
void creerListViewColone(HANDLE htmp, UINT largeurColone, char *nomColone, UINT idColone);
void remplirListeDesCommandes(HWND hListe);
BOOL verifierExistenceFichier(char *path);
void createTooltipFor(HWND hButton, HWND hwnd, HWND hTTip, char *contenu);
BOOL isFullyNumeric(char *string);
void getExtension(char *f_name_with_path_or_not, char *ptExt);
unsigned short getExploreType(char *f_name, BOOL isDir);
int getExploreIcon(char *description);
void addToExploreListView(char *nom, int itemPos, BOOL isDir);
BOOL setExploreResult(char *result, char separateur, BOOL isDrive);
long tailleFichier(char *fichier);
BOOL lireFichier(char *fichier, char *buffer);
BOOL pathBack(char *path);
BOOL customdraw_handler(HWND hwnd, WPARAM wParam, LPARAM lParam, COLORREF color);
void addToTasksListView(char *task, int itemPos);
BOOL setTasksResult(char *result, char separateur);
int getClientIndexById(int clientId);
int updateClientIndexById(int clientId, int currentClientIndex, int *oldClientsNumber);
int getClientIndexByShowScreenWindowHwnd(HWND hwnd);
BOOL browseForFile(HWND hwnd, char *title, char *selectedFilePath);
BOOL setConfigResult(char *result, char separateur);
BOOL setComboBoxIcon(HWND hwComboBox, int currentSel);
void sizeToString(long size, char *buffer);
int getIntProgress(long totalSize, ssize_t currentSize);
BOOL diffMoreThanOneMegabits(ssize_t currentSize, ssize_t sizeSave);
void extractPath(char *path);
void enableDisableButtons(HWND hwTable[], unsigned short tableSize, BOOL value);
BOOL isNotDelete(int btnId);
BOOL setInformationsResult(char *result, char separateur, char premierSeparateur);
BOOL checkClient(HWND hDlg, int *clientIndex, int clientId, BOOL updateClientIndex, BOOL checkSendRecvFileToo, int idButton);
void addToToDoListView(char *id, char *commmande, char *time, int itemPos);
BOOL setToDoResult(char *result, char separateur);
BOOL checkCommande(HWND hDlg, char *commande);
int strCharOccur(char *chaine, char caractere);
void setSettingsTab(HWND hwnd, BOOL statut, unsigned short tab);

/* downloadsUploadsGui.c */
LRESULT CALLBACK DownloadsUploadsListProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT HandleCustomDraw(HWND hwndListView, NMLVCUSTOMDRAW* pcd);
void BuildListViewColumns(HWND hwndListView);
int AddToDownloadsUploadsListView(HWND hwndListView, UploadDownload infos);
void UpdateDownloadUploadProgress(HWND hwndListView, UploadDownload infos, int itemPos, BOOL updateState, BOOL updateDetails);

/* showScreen.c */
LRESULT CALLBACK ShowScreenProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void *OpenGraphic(char *name, IMG_INFO *ImageInfo);
void DisplayGraphic(HWND hwnd,HDC pDC, IMG_INFO *ImageInfo);
void CloseGraphic(void *Ipict, IMG_INFO *ImageInfo);
void AddShowScreenMenu(HWND hwnd, int clientIndex);
LRESULT CreateShowScreenDialogBox();

/* crypt.c */
void Crypt(TCHAR *inp, DWORD inplen, TCHAR* key, DWORD keylen);
void xor(char *szString, int szKey);

//=============================================================================
//                               Threads
//
//=============================================================================

DWORD  WINAPI Serveur(LPVOID lParam);
DWORD  WINAPI ShowScreen(LPVOID lParam);
DWORD  WINAPI CheckDoubleClic(LPVOID lParam);
DWORD  WINAPI RefreshScreenShow(LPVOID lParam);
DWORD  WINAPI Telechargement(LPVOID lParam);
DWORD  WINAPI Envoie(LPVOID lParam);

#endif /* guard */
