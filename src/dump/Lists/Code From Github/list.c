#include "list.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct node {
  void *dataptr;
  struct node *link;
  char *type;
}node;

struct LinkedList {
  size_t count;
  bool all_string;
  Node *start;
  Node *end;
  int (*compare)(void *, void *);
};

LinkedList* new_linked_list() {
  LinkedList *link;
  link = malloc(sizeof( LinkedList));
  if(link) {
    link->start = link->end = NULL;
    link->count=0;
    link->compare = compare;
    link->all_string=false;
  }
  return link;
}

void delete_linked_list(  LinkedList* list) {
  Node *trav, *temp;
  if(list->count!=0) {
    trav = list->start;
    while(trav!=NULL) {
      temp = trav;
      trav =trav->link;
      free(temp);
    }
  }
  free(list);
}

int insert( LinkedList *list,void *data) {
  int pos;
  Node *ptr;
  ptr = malloc(sizeof(Node));
  if(ptr==NULL) return 0;

  ptr->dataptr = data;
  ptr->link=NULL;
  pos = list->count;
  pos++;
  if(list->count==0)
    list->start = list->end=ptr;

  else if (pos<=1) {
    ptr->link = list->start;
    list->start = ptr;
  }

  else if (pos>list->count) {
    list->end->link = ptr;
    list->end = ptr;
  }
  list->count++;
  return 1;
}

int empty( LinkedList *list) {
  if(list->count==0)return 1;
  else return 0;
}

int full( LinkedList *vect) {
  Node *ptr;
  ptr = malloc(sizeof(Node));
  if(ptr)return 0;
  else {
    free(ptr);
    return 1;
  }
}

int length ( LinkedList *list) {
  return list->count;
}

void* get_element( LinkedList *list,int position) {
  int i;
  Node *trav;
  if(list->count==0 || position<1)   return NULL;
  else if(position>list->count) return NULL;
  else {
    trav = list->start;
    for(i=1; i<=position-1; i++)
    trav = trav->link;

    if(list->all_string==true)
    return (char *)trav->dataptr;
    else
    return trav->dataptr;
  }

}

int* add_int(int data) {
  int *ptr;
  ptr = malloc(sizeof (int));
  *ptr = data;
  return ptr;
}

float* add_float(float data) {
  float *ptr;
  ptr = malloc(sizeof (float));
  *ptr = data;
  return ptr;
}

double* add_double(double data) {
  double *ptr;
  ptr = malloc(sizeof(double));
  *ptr = data;
  return ptr;
}

char* add_char(char data) {
  char *ptr;
  ptr = malloc(sizeof(char));
  *ptr = data;
  return ptr;
}

char* String(char *data) {
  char *ptr;
  ptr = malloc(sizeof(char)+strlen(data));
  ptr = data;
  return ptr;
}

int* int_ptr(int *data) {
  return data;
}

float* float_ptr(float *data) {
  return data;
}

double* double_ptr(double *data) {
  return data;
}

void* Default( void *ptr) {
  return ptr;
}

void set_all_string_data( LinkedList *list,bool val) {
  list->all_string = val;
}

void to_string_set(  LinkedList* list,char *ptr[]) {
  int j=0;
  Node *trav;
  if(list->all_string==false) {printf("all data should be of string type\n"); exit(EXIT_FAILURE);};
  trav = list->start;
  while(trav!=NULL) {
    ptr[j++] = (char *)trav->dataptr;
    trav = trav->link;
  }
}

void copy_into( LinkedList *list, void *ptr[]) {
  int j=0;
  Node *trav;
  if(list->count==0)return;
  trav  = list->start;
  while(trav!=NULL) {
    ptr[j++] = trav->dataptr;
    trav = trav->link;
  }
}

int insert_at( LinkedList *list,void *data,int index) {
  Node *ptr,*trav;
  int iterator;
  ptr = malloc(sizeof(Node));
  if(ptr==NULL) return 0;

  ptr->dataptr = data;
  ptr->link=NULL;

  if(list->count==0)
  list->start = list->end=ptr;

  else if (index<=1) {
    ptr->link = list->start;
    list->start = ptr;
  }

  else if (index>list->count) {
    list->end->link = ptr;
    list->end = ptr;
  }
  else {
    trav = list->start;
    for(iterator=1; iterator<=index-2; iterator++)
    trav  = trav->link;

    ptr->link = trav->link;
    trav->link = ptr;
  }
  list->count++;
  return 1;
}

void* delete(LinkedList *list, void *datap) {
  Node *prev, *nxt;
  void *temp;
  prev = NULL;
  nxt =  list->start;

  while(nxt!=NULL && list->compare(nxt->dataptr,datap)==0) {
    prev = nxt;
    nxt = nxt->link;
  }

  if(nxt==NULL)
  return NULL;
  else if(prev==NULL)
  list->start = list->start->link;
  else if(nxt==list->end) {
    prev->link = NULL;
    list->end = prev;
  }
  else
    prev->link = nxt->link;

  temp = nxt->dataptr;
  free(nxt);
  list->count--;
  return temp;
}

int  search_liked_list(LinkedList *list, void *datap) {
  Node *trav;
  trav  = list->start;
  while(trav!=NULL && list->compare(trav->dataptr,datap)!=1)
  trav = trav->link;

  if(trav==NULL)
  return 0;
  else
  return 1;
}

