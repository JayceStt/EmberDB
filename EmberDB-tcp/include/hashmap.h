#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>

#define MAXKEY 256
#define MAXVAL 1026
#define BUCKETS 1000

#define BACKUP_FILE "backup/Backup.txt"
#define BACKUP_TMP  "backup/Backup.tmp"

typedef struct Node {
    char key[MAXKEY];
    char value[MAXVAL];
    struct Node *next;
} Node;

unsigned int hashfunction(const char *key);
Node *Hash_Find(const char *key);
bool Hash_Insert(Node *N);
bool Hash_Delete(const char *Del_key);
bool Hash_Clear(void);
Node *Create_Node(const char *input_key, const char *input_val);
bool Hash_Set(const char *input_key, const char *input_val);
bool create_backup(void); 
bool load_src(void);


#endif
