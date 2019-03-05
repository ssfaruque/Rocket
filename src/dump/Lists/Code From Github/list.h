#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define add_element(args) _Generic((args),\
  int:add_int, int*:int_ptr,\
  char: add_char, char*: String,\
  float:add_float, float*:float_ptr,\
  double:add_double, double*: double_ptr,\
  default:Default\
)(args)

typedef struct node Node;
typedef struct LinkedList LinkedList;

LinkedList* new_linked_list();

int*		add_int(int );
float*		add_float(float );
double*		add_double(double );
char*		add_char(char );
int*		int_ptr(int *);
float*		float_ptr(float *);
double*		double_ptr(double *);
int			insert(struct LinkedList*,void *);
void*		get_element(struct LinkedList*,int);
int			insert_at(struct LinkedList*,void*,int);
void 		set_all_string_data(struct LinkedList*,bool);
void*		Default(void *);
char*		String(char *);
void* 		delete(struct LinkedList*, void *);
extern inline  int     compare(void *,void *);

#endif


