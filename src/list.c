#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "bullet.h"

void m_list_init(MessageList *list)
{
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}
void m_list_push(MessageList* list, char msg[50])
{
    MessageNode *new_node = malloc(sizeof(MessageNode));
    strcpy(new_node->msg, msg);
    new_node->next = NULL;
    new_node->prev = list->tail;

    if(list->head == NULL)
        list->head = new_node;
    else
        list->tail->next = new_node;
    
    list->tail = new_node;
    list->count++;
}
void m_list_pop(MessageList* list)
{
    MessageNode* aux = list->head;

    if(list->head != NULL)
        list->head = list->head->next;

    if(list->head == NULL)
        list->tail = NULL;
    else
        list->head->prev = NULL;

    free(aux);
    list->count--;
}
int m_list_count(MessageList list)
{
    MessageNode* aux = list.head;
    int c=0;

    while(aux != NULL)
    {
        c++;
        aux = aux->next;
    }
    return c;
}

///////////////

void b_list_init(BulletList *list)
{
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}
BulletNode* b_list_push(BulletList* list, Bullet bullet)
{
    BulletNode *new_node = malloc(sizeof(BulletNode));
    new_node->bullet = bullet;
    new_node->next = NULL;
    new_node->prev = list->tail;

    if(list->head == NULL)
        list->head = new_node;
    else
        list->tail->next = new_node;
    
    list->tail = new_node;
    list->count++;

    return new_node;
}
void b_list_remove(BulletList* list, BulletNode* node)
{
    if(node == NULL)
        return;
        
    if(node->prev != NULL)
        node->prev->next = node->next;
    else
        list->head = node->next;

    if(node->next != NULL)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;

    free(node);
    list->count--;
}

int b_list_count(BulletList list)
{
    BulletNode* aux = list.head;
    int c=0;

    while(aux != NULL)
    {
        c++;
        aux = aux->next;
    }
    return c;
}

BulletNode* b_list_search(BulletList list, Bullet bullet)
{
    BulletNode* aux = list.head;

    while(aux != NULL)
    {
        if(aux->bullet.id == bullet.id)
            break;
        aux = aux->next;
    }

    return aux;
}

void b_list_update(BulletList* list, Bullet bullet)
{
    BulletNode* node = b_list_search(*list, bullet);
    if(node == NULL)
    {
        b_list_push(list, bullet);
    }
    else
    {
        node->bullet = bullet;
    }
}