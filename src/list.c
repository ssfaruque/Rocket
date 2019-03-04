#include <stdlib.h>

#include "list.h"


#if 0

List* create_new_list()
{
    List* list      = (List*) malloc(sizeof(List));
    list->head      = NULL;
    list->num_nodes = 0;
    return list;
}


void destroy_list(List* list)
{
    if(list)
        free(list);
}


void add_node(List* list, void* data)
{
    ListNode* newNode = malloc(sizeof(ListNode));
    newNode->data     = data;
    newNode->next     = NULL;

    ListNode* ptr = list->head;

    if(ptr == NULL)
        list->head = newNode;

    else
    {
        while(ptr->next) {ptr = ptr->next;}
        ptr->next = newNode;
    }
    
    list->num_nodes++;
}


void remove_node(List* list, void* data)
{
    ListNode* prev = list->head;
    ListNode* curr = list->head;

    while(curr && (((char*)curr->data) != (((char*)data))))
    {
        prev = curr;
        curr = curr->next;
    }

    if(curr == list->head)
        list->head = curr->next;
    
    else
        prev->next = curr->next;
    
    free(curr);
    list->num_nodes--;
}


void traverse_list(List* list, process_data process)
{
    ListNode* ptr = list->head;

    while(ptr) 
    {
        process(ptr->data);
        ptr = ptr->next;
    }
}

#endif