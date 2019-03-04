#ifndef LIST_H
#define LIST_H


typedef struct ListNode
{
    void* data;
    struct Listnode* next;
} ListNode;


typedef struct
{
    int num_nodes;
    ListNode* head;
} List;


typedef int (*process_data)(void* data);


List* create_new_list();

void destroy_list(List* list);

void add_node(List* list, void* data);

void remove_node(List* list, void* data);

void traverse_list(List* list, process_data process);



#endif  // LIST_H