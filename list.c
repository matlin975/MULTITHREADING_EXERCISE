#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/*
 * ===== Assignment OU1 for "Systemnära Programmering 5DV088 ht19" =======
 *
 * Description:
 * An implementation of a linked list based on chapter 4
 * of "Datatyper och algoritmer, Lars-Erik Janlert, Torbjörn Wiberg"
 *
 * Author: Mattias Lindström
 * Mail: hed17mlm@cs.umu.se
 *
 * Version information:
 * v1.0  2019-09-13: First version.
 *  Comment: -
 *
*/

// ============================  TYPEDEF  ================================

typedef struct node{
	void *val;
	struct node *next;
}node;

typedef struct list{
	struct node *head;
}list;

// ======================  FUNCTION DEFINITIONS  ===========================

/**
* list_empty() - Create an empty list.
*
* Returns: The list.
*/
list *list_empty(void){
	//Allocate list structure
	list *l = calloc(1, sizeof(list));
	if (l == NULL){
		perror("list_empty calloc");
		exit(EXIT_FAILURE);
	}

	//Allocate list-head
	l->head = calloc(1, sizeof(node));
	if (l->head == NULL){
		perror("list_empty calloc");
		exit(EXIT_FAILURE);
	}
	l->head->val = NULL;
	l->head->next = NULL;

	return l;
}



/**
* list_isempty() - Check if a list is empty.
* @l: A list.
*
* Returns: True if the list is empty, else false. Returns -1 on failure.
*/
bool list_isempty(list *l){
	if (l != NULL){
		return l->head->next == NULL;
	}
	else{
		return -1;
	}
}



/**
* list_isend() - Checks if a position in a list is the last position of
* the list.
* @l: A list.
* @p: The position to check
*
* Returns: True if p is the last position of l, otherwise false. Returns -1
* on failure.
*/
bool list_isend(list *l, pos p){
	if (l != NULL){
		return p->next == NULL;
	}
	else{
		return -1;
	}
}



/**
* list_first() - Returns the first element of the list.
* @l: A list.
*
* Returns: First element of l. Returns NULL if list doesn't exist.
*/
pos list_first(list *l){
	if (l != NULL){
		return l->head;
	}
	else {
		return NULL;
	}
}



/**
* list_next() - Returns the next position after p, in list l.
* @l: A list
* @p: A position in the list.
*
* Returns: The next position after p. Returns NULL if list doesn't exist.
*/
pos list_next(list *l, pos p){
	if (l != NULL){
		return p->next;
	}
	else {
		return NULL;
	}
}



/**
* list_insert() - Inserts a new element in the list after position p.
* @l: A list.
* @p: Position to be inserted at.
* @v: Value to be inserted.
*
* Returns: The position after the inserted element. Returns NULL if list
* doesn't exist.
*/
pos list_insert(list *l, pos p, void *v){
	if (l != NULL){
		node *n = calloc(1, sizeof(node));
		if (n == NULL){
			perror("list_insert calloc");
			exit(EXIT_FAILURE);
		}
		n->val = v;
		n->next = p->next;
		p->next = n;

		return n->next;
	}
	else{
		return NULL;
	}
}



/**
* list_inspect() - Inspects the value at pos p in list l.
* @l: A list.
* @p: Position to inspect at.
*
* Returns: The value of the element at pos p. Returns NULL if list doesn't
* exist.
*/
void *list_inspect(list *l, pos p){
	if (l != NULL){
		return p->val;
	}
	else {
		return NULL;
	}
}



/**
* list_remove - Removes an element from list l at pos p.
* @l: A list.
* @p: Position to remove element at.
*
* Returns: The position of the element after the removed element. Returns NULL
* if list doesn't exist.
*/
pos list_remove(list *l, pos p){
	if (l != NULL){
		node *n = p->next;
		p->next = n->next;

		free(n->val);
		free(n);
		return p;
	}
	else{
		return NULL;
	}
}



/**
* list_swap() - Swaps the values of two elements in a list.
* @n1: First node element to be swapped.
* @n2: Second node element to be swapped.
*
* Returns: Nothing.
*/
void list_swap(node *n1, node *n2){
	void *temp = n1->val;
	n1->val = n2->val;
	n2->val = temp;
}



/**
* list_print() - Prints the contents of a list to stdout, starting at
* the first element in the list.
* @l: A list.
*
* Returns: Nothing.
*/
void list_print(list *l){
	node *n = list_first(l);

	while(!list_isend(l, n)){
		printf("(%d)->", *(int*)n->val);
		n = list_next(l, n);
	}
	printf(".\n");
}



/**
* list_kill() - Frees the memory used by a list.
* @l: List to kill.
*
* Returns: Nothing.
*/
void list_kill(list *l){
	pos p = list_first(l);

	while(!list_isempty(l)){
		p = list_remove(l, p);
	}

	free(l->head->val);
	free(l->head);
	free(l);
}
