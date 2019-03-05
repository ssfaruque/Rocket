#include "list.h"
#include <stdio.h>
int main() {
  LinkedList *List;
  char *ptrs[5];
  List = new_linked_list();
  set_all_string_data(List,false);

  insert(List,add_element(1111.000000));
  insert(List,add_element("This is a string"));
  insert(List,add_element(10));
  insert(List,add_element('A'));
  insert(List,add_element(-2034));

  printf("%f\n",*(double *)get_element(List,1));
  printf("%s\n",(char *)get_element(List,2));
  printf("%d\n",*(int *)get_element(List,3));
  printf("%c\n",*(char *)get_element(List,4));
  printf("%d\n",*(int  *)get_element(List,5));

  return 0;
}

int compare(void *p,void *q){
  return *(int *)p==*(int *)q;
}

