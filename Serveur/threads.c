//=============================================================================
// Projet : Winrell (server)
// Fichier : threads.c
//
//=============================================================================

#include "server.h"


//=============================================================================
//                           Thread : Serveur
//=============================================================================

DWORD  WINAPI Serveur(LPVOID lParam)
{
    init();
   
    app();

    end();
   
    return EXIT_SUCCESS;
}

//=============================================================================
//                           Thread : ShowScreen
//=============================================================================

DWORD  WINAPI ShowScreen(LPVOID lParam)
{
    int clientIndex, clientId, clientsNumberSave;
    char cmd[20]; // 20 caratères devraient suffir, ex: Send_Pict 70 (faut savoir optimisé quand c'est possible)
    
    // On récupère le paramètre du thread / l'indice/index du client
    int clientIndexParam = (int)lParam;
    
    // Sauvegarde des données du client (indice/index, id, clientsNumber)
    clientIndex = clientIndexParam;
    clientId = clients[clientIndex].id;
    clientsNumberSave = actualClientsNumber;
    
    // Tant que le client ne s'est pas déconnecté et qu'on peut toujours visionner l'écran du client
    while(clientIndex != -1 && clients[clientIndex].show_screen)
    {
        // Si stop == FALSE + le client ne reçoit rien + le client n'envoie rien + on ne controle pas la sourie, on peut y aller, si nn on ne fait rien à part attendre
        if (!clients[clientIndex].show_screen_infos.stop && !clients[clientIndex].recv_file && !clients[clientIndex].send_file && !clients[clientIndex].show_screen_infos.mouseControl)
        {
            // On prépare la commande
            sprintf(cmd, "Send_Pict %d", clients[clientIndex].show_screen_infos.quality);
            
            // On exécute la commande
            write_client(&clients[clientIndex], cmd, TRUE);
        }
        
        // On attend le temp donné
        Sleep(clients[clientIndex].show_screen_infos.speed * 1000);
            
        // mise à jour de l'indice du client (peut être que le client ou qu'un client avant nous s'est déconnecté)
        clientIndex = updateClientIndexById(clientId, clientIndex, &clientsNumberSave);
    }
    
    return EXIT_SUCCESS;
}

//=============================================================================
//                        Thread : CheckDoubleClic
//=============================================================================

DWORD  WINAPI CheckDoubleClic(LPVOID lParam)
{
    // Récupération du/des paramètres
    Clic *clic = (Clic *)lParam;
    
    // Petite pause (pour attendre la confirmation du double clic)
    Sleep(300);
    
    // On met à jour l'indice/index du client (peut être que le client ou qu'un client avant nous s'est déconnecté)
    (*clic).clientIndex = updateClientIndexById((*clic).clientId, (*clic).clientIndex, &(*clic).clientsNumberSave);
    
    // Si le client est toujours connecté
    if ((*clic).clientIndex != -1) // c'est juste pour la sécurité et éviter un plantage, vu que le thread ne dure que pour moin d'une seconde
    {
        // Si le nombre de clic est inférieur à 2 + la variable doubleClic == FALSE
        if (clients[(*clic).clientIndex].show_screen_infos.clicks < 2 && !clients[(*clic).clientIndex].show_screen_infos.doubleClic)
        {
            char cmd[50];
            
            // construction de la commande
            sprintf(cmd, "L_Mouse_Clic %d %d %d %d", (*clic).pt.x, (*clic).pt.y, (*clic).rc.right - (*clic).rc.left, (*clic).rc.bottom - (*clic).rc.top);
            
            // exécution de la commande
            write_client(&clients[(*clic).clientIndex], cmd, TRUE);
                    
            // on appel le thread qui va s'occuper de raffraichir l'écran
            DWORD dwThreadId;
            HANDLE hThread = CreateThread(NULL, 0, RefreshScreenShow, (LPVOID)clients[(*clic).clientIndex].id, 0, &dwThreadId);
        }
        
        // On rénitialise doubleClic et clicks
        clients[(*clic).clientIndex].show_screen_infos.doubleClic = FALSE;
        clients[(*clic).clientIndex].show_screen_infos.clicks = 0;
        
        // Si nn c'etait surement un double clic (il sera effectuer directement après la récéption du message WM_LBUTTONDBLCLK)
    }
    
    // On libère la structure allouée pour passer les paramètres
    free(clic);
    
    return EXIT_SUCCESS;
}

//=============================================================================
//                        Thread : RefreshScreenShow
//=============================================================================

DWORD  WINAPI RefreshScreenShow(LPVOID lParam)
{
    int clientId = (int)lParam;
    
    // On attend une demi-seconde
    Sleep(500);
    
    // On récupère l'index du client à partir de son id
    int clientIndex = getClientIndexById(clientId);
    
    // Si le client est toujorus connecté
    if (clientIndex != -1)
    {
        char cmd[20];
        
        // On prépare la commande pour raffraîchir l'écran/le screen show
        sprintf(cmd, "Send_Pict %d", clients[clientIndex].show_screen_infos.quality);
        // On exécute la commande
        write_client(&clients[clientIndex], cmd, TRUE);
    }
    
    return EXIT_SUCCESS;
}

//=============================================================================
//                           Thread : Telechargement
//=============================================================================

DWORD  WINAPI Telechargement(LPVOID lParam)
{
    Client *clientActuel = (Client *)lParam;
    int i = recv_file((*clientActuel).id);
    
    return EXIT_SUCCESS;
}

//=============================================================================
//                           Thread : Envoie
//=============================================================================

DWORD  WINAPI Envoie(LPVOID lParam)
{
    Client *clientActuel = (Client *)lParam;
    send_file((*clientActuel).id);
       
    return EXIT_SUCCESS;
}
