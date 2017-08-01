//=============================================================================
// Projet : Winrell (server)
// Fichier : exploreTypes.h
//
// @ Les préfixes des commandes ne devraient pas/jamais contenir des espaces.
//=============================================================================

#ifndef EXPLORE_TYPES_H
#define EXPLORE_TYPES_H

#include "server.h" // car on utilise les constantes IDI_FOLDER, ...


//=============================================================================
//                          Structure exploreType
//
//=============================================================================

typedef struct
{
   char description[100];
   char extension[6];
   unsigned int iconeId;
}exploreType;

exploreType exploreTypes[] = { {"Dossier", "", IDI_FOLDER},
                               {"Fichier inconnu (?)", "?", IDI_FILE},
                               {"Fichier texte (.txt)", "txt", IDI_TEXT_FILE},
                               {"Fichier image (.jpg)", "jpg", IDI_IMAGE},
                               {"Fichier image (.jpeg)", "jpeg", IDI_IMAGE},
                               {"Fichier image (.png)", "png", IDI_IMAGE},
                               {"Fichier image (.gif)", "gif", IDI_IMAGE},
                               {"Fichier image (.bmp)", "bmp", IDI_IMAGE},
                               {"Fichier son (.mp3)", "mp3", IDI_SON},
                               {"Fichier son (.wav)", "wav", IDI_SON},
                               {"Fichier vidéo (.mp4)", "mp4", IDI_VIDEO},
                               {"Fichier vidéo (.flv)", "flv", IDI_VIDEO},
                               {"Fichier vidéo (.mkv)", "mkv", IDI_VIDEO},
                               {"Fichier vidéo (.avi)", "avi", IDI_VIDEO},
                               {"Fichier vidéo (.3gp)", "3gp", IDI_VIDEO},
                               {"Fichier exécutable (.exe)", "exe", IDI_EXE},
                               {"Document Microsoft Office Word 97 - 2003 (.doc)", "doc", IDI_DOC_WORD},
                               {"Document Microsoft Office Word 2007 (.docx)", "docx", IDI_DOC_WORD},
                               {"Document Microsoft Office Excel 97 - 2003 (.xls)", "xls", IDI_DOC_EXCEL},
                               {"Document Microsoft Office Excel 2007 (.xlsx)", "xlsx", IDI_DOC_EXCEL},
                               {"Document Microsoft Office PowerPoint 97 - 2003 (.ppt)", "ppt", IDI_DOC_POWERPOINT},
                               {"Document Microsoft Office PowerPoint 2007 (.pptx)", "pptx", IDI_DOC_POWERPOINT},
                               {"Document Microsoft Office Access 97 - 2003 (.mdb)", "mdb", IDI_DOC_ACCESS},
                               {"Document Microsoft Office Access 2007 (.accdb)", "accdb", IDI_DOC_ACCESS},
                               {"Archive WinRAR ZIP (.zip)", "zip", IDI_ZIP_RAR},
                               {"Archive WinRAR RAR (.rar)", "rar", IDI_ZIP_RAR},
                               {"Adobe Acrobat Document (.pdf)", "pdf", IDI_PDF}
                             };

#endif
