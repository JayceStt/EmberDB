#include <stdio.h>
#include <stdlib.h>

#include "server.h"
#include "persistance.h"
#include <general.h>

int main(void) {
    
    read_print(NAME_FILE); 
    printf("\n\n"); 
    init_database(); 
    Init_Server(); 
    run_server(); 

    return 0;
}