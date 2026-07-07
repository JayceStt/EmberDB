#ifndef PERSISTANCE_H
#define PERSISTANCE_H

#include <stdbool.h>

void init_database(void);
bool database_wipe(void);
bool database_snap(void);
bool backup_wipe(void);

 
#endif
