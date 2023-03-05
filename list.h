#ifndef __LIST_H
#define __LIST_H

#include <stdbool.h>

/*
 * ===== Assignment OU1 for "Systemnära Programmering 5DV088 ht19" =======
 *
 * Description:
 * Declaration of a linked list based on chapter 4
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

typedef struct list list;   //List structure.
typedef struct node node;   //Node structure.
typedef struct node* pos;   //List position structure.

/**
 * list_empty() - Create an empty list.
 *
 * Returns: The list.
 */
list *list_empty(void);

/**
 * list_isempty() - Check if a list is empty.
 * @l: A list.
 *
 * Returns: True if the list is empty, else false. Returns -1 on failure.
 */
bool list_isempty(list *l);

/**
 * list_isend() - Checks if a position in a list is the last position of
 * the list.
 * @l: A list.
 * @p: The position to check
 *
 * Returns: True if p is the last position of l, otherwise false. Returns -1
 * on failure.
 */
bool list_isend(list *l, pos p);

/**
 * list_first() - Returns the first element of the list.
 * @l: A list.
 *
 * Returns: First element of l. Returns NULL if list doesn't exist.
 */
pos list_first(list *l);

/**
 * list_next() - Returns the next position after p, in list l.
 * @l: A list
 * @p: A position in the list.
 *
 * Returns: The next position after p. Returns NULL if list doesn't exist.
 */
pos list_next(list *l, pos p);

/**
 * list_insert() - Inserts a new element in the list after position p.
 * @l: A list.
 * @p: Position to be inserted at.
 * @v: Value to be inserted.
 *
 * Returns: The position after the inserted element. Returns NULL if list
 * doesn't exist.
 */
pos list_insert(list *l, pos p, void *v);

/**
 * list_inspect() - Inspects the value at pos p in list l.
 * @l: A list.
 * @p: Position to inspect at.
 *
 * Returns: The value of the element at pos p. Returns NULL if list
 * doesn't exist.
 */
void *list_inspect(list *l, pos p);

/**
 * list_remove - Removes an element from list l at pos p.
 * @l: A list.
 * @p: Position to remove element at.
 *
 * Returns: The position of the element after the removed element. Returns
 * NULL if list doesn't exist.
 */
pos list_remove(list *l, pos p);

/**
 * list_swap() - Swaps the values of two elements in a list.
 * @n1: First node element to be swapped.
 * @n2: Second node element to be swapped.
 *
 * Returns: Nothing.
 */
void list_swap(node *n1, node *n2);

/**
 * list_print() - Prints the contents of a list to stdout, starting at
 * the first element in the list.
 * @l: A list.
 *
 * Returns: Nothing.
 */
void list_print(list *l);

/**
 * list_kill() - Frees the memory used by a list.
 * @l: List to kill.
 *
 * Returns: Nothing.
 */
void list_kill(list *l);

#endif
