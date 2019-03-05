#include <stdlib.h>
#include <stdio.h>
#include <string.h>



struct ListNode
{
    void* data;
    struct Listnode* next;
} ListNode;


struct List
{
    int num_nodes;
    struct ListNode* head;
} List;


typedef int (*process_data)(void* data);


struct List* create_new_list();

void destroy_list(struct List* list);

void add_node(struct List* list, void* data);

void remove_node(struct List* list, void* data);

void traverse_list(struct List* list, process_data process);

int print_int_data(void* data);
struct List* create_new_list()
{
    struct List* list      = (struct List*) malloc(sizeof(struct List*));
    list->head      = NULL;
    list->num_nodes = 0;
    return list;
}


void destroy_list(struct List* list)
{
    if(list)
        free(list);
}


void add_node(struct List* list, void* data)
{
    struct ListNode* newNode = malloc(sizeof(ListNode));
    newNode->data     = data;
    newNode->next     = NULL;

    struct ListNode* ptr = list->head;

    if(ptr == NULL)
        list->head = newNode;

    else
    {
        while(ptr->next) {ptr = ptr->next;}
        ptr->next = newNode;

     /*  if(__builtin_types_compatible_p(typeof(ptr), struct ListNode*)) {
        printf("ptr is of type ListNode*\n");
       }
       if(__builtin_types_compatible_p(typeof(ptr->next), struct ListNode*)) {
        printf("ptr->next is of type ListNode*\n");
       }*/
 
    }
    
    list->num_nodes++;
}


void remove_node(struct List* list, void* data)
{
    struct ListNode* prev = list->head;
    struct ListNode* curr = list->head;

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


void traverse_list(struct List* list, process_data process)
{
    struct ListNode* ptr = list->head;

    while(ptr) 
    {
        process(ptr->data);
        ptr = ptr->next;
    }
}

int print_int_data(void* data)
{
    int* int_data = (int*) data;
    printf("%d\n", *int_data);
    return 0;
}


int main(int argc, char* argv[])
{
    int nums[] = {1, 2, 3, 4, 10, 20};

    struct List* list = create_new_list();

    int i;

    for(i = 0; i < sizeof(nums) / sizeof(int); i++)
        add_node(list, &nums[i]);

    traverse_list(list, print_int_data);

    destroy_list(list);

    return 0;
}
