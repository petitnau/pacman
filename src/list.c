#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

void list_init(MessageList *list)
{
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}
void list_push(MessageList* list, char msg[50])
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
void list_pop(MessageList* list)
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

int list_count(MessageList list)
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