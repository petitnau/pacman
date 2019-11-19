#ifndef LIST_H
#define LIST_H

typedef struct nodo_messaggio
{
    char msg[50];
    struct nodo_messaggio* next;
    struct nodo_messaggio* prev;
} MessageNode;

typedef struct
{
    MessageNode *head;
    MessageNode *tail;
    int count;
} MessageList;

void list_init(MessageList *list);
void list_push(MessageList*, char[50]);
void list_pop(MessageList*);
int list_count(MessageList);

#endif