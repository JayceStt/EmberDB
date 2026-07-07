//Persistance Logic 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "hashmap.h"

void init_database(void) {
    if (!load_src()) {
        printf("No Source File Detected Check 'Backup' file\n");
        printf("Initializing Empty Database\n");
    }
    else {
        printf("Database Initialized from 'Backup.txt'\n");
    }
}

bool database_wipe(void) {
    return Hash_Clear();
}

bool database_snap(void) {
    return create_backup(); 
}

bool backup_wipe(void) {
    if(!remove(BACKUP_FILE)) return true; 
    else return false; 
}
