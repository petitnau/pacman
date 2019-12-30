#ifndef LIST_H
#define LIST_H

#include "bullet.h"

typedef struct nodo_message
{
    char msg[50];
    struct nodo_message* next;
    struct nodo_message* prev;
} MessageNode;

typedef struct
{
    MessageNode *head;
    MessageNode *tail;
    int count;
} MessageList;

///////////////

typedef struct nodo_bullet
{
    Bullet bullet;
    struct nodo_bullet* next;
    struct nodo_bullet* prev;
} BulletNode;

typedef struct
{
    BulletNode *head;
    BulletNode *tail;
    int count;
} BulletList;

void m_list_init(MessageList*);
void m_list_push(MessageList*, char[50]);
void m_list_pop(MessageList*);
int m_list_count(MessageList);

void b_list_init(BulletList*);
BulletNode* b_list_push(BulletList*, Bullet);
void b_list_remove(BulletList*, BulletNode*);
int b_list_count(BulletList);
BulletNode* b_list_search(BulletList, Bullet);
void b_list_update(BulletList*, Bullet);
void b_list_empty(BulletList*);

#endif