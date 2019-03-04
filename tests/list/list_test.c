#include <stdio.h>

#include "list.h"


int print_int_data(void* data)
{
    int* int_data = (int*) data;
    printf("%d\n", *int_data);
    return 0;
}


int main(int argc, char* argv[])
{
    int nums[] = {1, 2, 3, 4, 10, 20};

    List* list = create_new_list();

    int i;

    for(i = 0; i < sizeof(nums) / sizeof(int); i++)
        add_node(list, &nums[i]);

    traverse_list(list, print_int_data);

    destroy_list(list);

    return 0;
}