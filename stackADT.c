/*
 * A memory-efficient Stack data structure that handles all data and memory operations
 * without memory leaks.
 * 
 * Adapted from a school project.
 * 
 * Eric Wang
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef IMP
#else
#include "stackADT.h"
#endif

struct node {
    void * data;
    struct node *next;
};

struct stack_type {
    struct node *top;
};

static void terminate(const char *message){
    printf("%s\n", message);
    exit(EXIT_FAILURE);
}

Stack Stack_create(void){
    Stack s = malloc(sizeof(struct stack_type));
    if(s == NULL){
        terminate("Create error: Stack could not be created.");
    }
    s->top = NULL;
    return s;
}

void Stack_destroy(Stack s){
    Stack_make_empty(s);
    free(s);
}

void Stack_make_empty(Stack s){ // FREE THE DATA HERE!
    while(!Stack_is_empty(s)){
        void * s_ptr = Stack_pop(s);
        free(s_ptr);
    }
}

bool Stack_is_empty(Stack s){
    return s->top == NULL;
}

bool Stack_is_full(Stack s){ // ?
    return false;
}

void Stack_push(Stack s, void * i){
    struct node *new_node = malloc(sizeof(struct node));
    if(new_node == NULL){
        terminate("Push error: Stack is full.");
    }
    new_node->data = i;
    new_node->next = s->top;
    s->top = new_node;
}

void * Stack_pop(Stack s){
    struct node *old_top;
    void * i;

    if(Stack_is_empty(s)){
        return NULL;
    }

    old_top = s->top;
    i = old_top->data;
    s->top = old_top->next;
    free(old_top);
    return i;
}

void * Stack_peek(Stack s){
    void * i;

    if(Stack_is_empty(s)){
        terminate("Peek error: Stack is empty.");
    }

    i = s->top->data;
    return i;
}

// BELOW FUNCTIONS COME FROM DISCUSSION 9
void print_char_back(struct node *n_ptr)
{
    if (n_ptr == NULL)
    {
        printf("NULL");
        return;
    }

    print_char_back(n_ptr->next);
    printf("<-%c", *((char *)n_ptr->data));
}

void Stack_char_print(Stack s)
{
    print_char_back(s->top);
    printf("<-top\n");
}

// for dataStack
void print_int_back(struct node *n_ptr)
{
    if (n_ptr == NULL)
    {
        printf("NULL");
        return;
    }

    print_int_back(n_ptr->next);
    printf("<-%d", *((int *)n_ptr->data));
}

void Stack_int_print(Stack s)
{
    print_int_back(s->top);
    printf("<-top\n");
}
