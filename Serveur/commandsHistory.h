//=============================================================================
// Projet : Winrell (server)
// Fichier : commandsHistory.h
//
// @ Les préfixes des commandes ne devraient pas/jamais contenir des espaces.
//=============================================================================

#ifndef COMMANDS_HISTORY_H
#define COMMANDS_HISTORY_H

#define CMD_BUF_SIZE    256

//=============================================================================
//               Historique des commandes/commandes sauvegardées
//
//=============================================================================

char cmdHistory[][CMD_BUF_SIZE] = {"Send_File log",
                                   "Reset_Log 1",
                                   "Current_Version",
                                   "New_Version_Update D:\\WinSk.exe",
                                   "File_Opr DELETE D:\\WinSk.exe|-",
                                   "Shell_Exec www.google.com",
                                   "Set_Task_Manager ON",
                                   "Set_Registry_Editor ON"
                                   };

#endif
