//Including hashtables for data storage/ret 

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdbool.h>

#include "hashmap.h" 


static Node *hashtable[BUCKETS] = { NULL }; //Only Hash-set function has access to this mem for Security reasons 

/*
useful funciton for debugging linked list 

unsigned int hashfunction(const char *key){
    unsigned int hash_val = 0; 
    return hash_val; 
}
*/

unsigned int hashfunction(const char *key){
    int len = strlen(key); 
    unsigned int hash_val = 0; 
    for (int i=0; i<len; i++){
        hash_val += key[i]; 
        hash_val = (hash_val*key[i]) % BUCKETS; 
    }
    return hash_val; 
}


Node *Hash_Find(const char *key){
    int bucket = hashfunction(key); 
    Node *prime = hashtable[bucket]; 
    
    while (prime != NULL ){
        if(strcmp(prime->key, key)== 0) return prime; 
        else prime = prime->next;   
        
    }   
    return NULL;  

}

bool Hash_Insert(Node *N){
    if (N == NULL) return false;  
    int hash_key = hashfunction(N->key); 
    if (hashtable[hash_key] != NULL)  {
        N->next = hashtable[hash_key];
        hashtable[hash_key] = N; 
        return true; 
    }
    hashtable[hash_key] = N;
    return true; 
}

bool Hash_Delete(const char *Del_key) {
    if (Del_key == NULL) {
        return false;
    }
    int hash_key = hashfunction(Del_key);

    Node *prime = hashtable[hash_key];
    Node *previous = NULL;

    while (prime != NULL) {
        if (strcmp(prime->key, Del_key) == 0) {
            if (previous == NULL) {
                hashtable[hash_key] = prime->next;
            } else {
                previous->next = prime->next;
            }
            free(prime);
            return true;
        }

        previous = prime;
        prime = prime->next;
    }

    return false;
}

bool Hash_Clear() {
    Node *next = NULL; 
    Node *current = NULL; 

    for (int i = 0; i < BUCKETS; i++) {
        current = hashtable[i]; 

        while (current != NULL) {
            next = current->next; 
            free(current); 
            current = next;  
       }

        hashtable[i] = NULL; 
    }

    return true; 
}

Node *Create_Node(const char *input_key, const char *input_val) {
    Node *new_node = malloc(sizeof(Node)); //set aside heap mem 

    if (new_node == NULL) {
        return NULL;
    }

    strncpy(new_node->key, input_key, MAXKEY - 1);
    new_node->key[MAXKEY - 1] = '\0';

    strncpy(new_node->value, input_val, MAXVAL - 1);
    new_node->value[MAXVAL - 1] = '\0';

    new_node->next = NULL;

    return new_node;
}

bool Hash_Set(const char *input_key, const char *input_val) {
    if (input_key == NULL || input_val == NULL) {
        return false;
    }

    Node *prime = Hash_Find(input_key);

    if (prime != NULL) {
        strncpy(prime->value, input_val, MAXVAL - 1);
        prime->value[MAXVAL - 1] = '\0';
        return true;
    }
    else {
        Node *Insert_node = Create_Node(input_key, input_val);

        if (Insert_node == NULL) {
            return false;
        }

        return Hash_Insert(Insert_node);
    }
}

bool create_backup(void) {
    FILE *backup_tmp = fopen(BACKUP_TMP, "w");

    if (backup_tmp == NULL) {
        return false;
    }

    Node *temp; 
    int check; 

    for (int i = 0; i < BUCKETS; i++) {
        temp = hashtable[i]; 

        while (temp != NULL) {
            fprintf(backup_tmp, "%s~%s\n", temp->key, temp->value);
            temp = temp->next; 
        }
    }

    fclose(backup_tmp);
    
    FILE *src = fopen(BACKUP_FILE, "w");
    FILE *tmp = fopen(BACKUP_TMP, "r");
   
    if (src == NULL || tmp == NULL) {
        if (src != NULL) {
            fclose(src);
        }

        if (tmp != NULL) {
            fclose(tmp);
        }

        remove(BACKUP_TMP); 
        return false; 
    }
        
    while ((check = fgetc(tmp)) != EOF) {
        fputc(check, src); 
    }
    
    fclose(src);
    fclose(tmp);

    remove(BACKUP_TMP); 

    return true; 
}

bool load_src(void) {
    FILE *src = fopen(BACKUP_FILE, "r");

    if (src == NULL) {
        return false;
    }

    char temp_buffer[MAXKEY + MAXVAL + 4];

    while (fgets(temp_buffer, sizeof(temp_buffer), src) != NULL) {
        temp_buffer[strcspn(temp_buffer, "\r\n")] = '\0';

        char *delimiter = strchr(temp_buffer, '~');

        if (delimiter == NULL) {
            continue;
        }

        *delimiter = '\0';

        char *key = temp_buffer;
        char *value = delimiter + 1;

        if (key[0] == '\0') {
            continue;
        }

        if (!Hash_Set(key, value)) {
            fclose(src);
            return false;
        }
    }

    fclose(src);
    return true;
}
