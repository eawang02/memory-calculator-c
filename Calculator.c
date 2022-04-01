/*
 * A memory-safe calculator that utilizes Stack data structures in order to
 * accurately and efficiently parse a text equation before returning the result.
 * This program handles all necessary memory transactions without leaks.
 * 
 * Adapted from a school project.
 * 
 * Eric Wang
 * 
 */

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#ifdef IMP
#else
#include "stackADT.h"
#endif

// maximum size of a single command 
#define MAX_INPUT_SIZE 4096
// maximum size of a operation
#define MAX_OP_SIZE 64


void print_command_help();
int process(char *input, Stack dataStack, Stack opStack);
bool is_int(char *);
int runOperation(char *op, Stack dataStack);
int runCloseParen(Stack dataStack, Stack opStack);
bool higherPriority(char *op1, char *op2);

void error_msg_extraData(char *cmd) {
  printf("ERROR: %s: found data left over!\n", cmd);
}

void error_msg_badCommand(char *cmd) {
  printf("ERROR: bad command!\n");
}

void error_msg_opStackNotEmpty(char *cmd) {
  printf("ERROR: %s: not able to process all operations\n", cmd);
}

void error_msg_missingResult(char *cmd) {
  printf("ERROR: %s: no result!\n", cmd);
}

void error_msg_opMissingArgs(char *op) {
  printf("ERROR: op %s: empty stack need two arguments: found none.\n", op);
}

void error_msg_divByZero(void) {
  printf("Error: Division by zero!\n");
}

void error_msg_badOp(char *op) {
  printf("Error: Unrecognized operator: %s!\n", op);
}

int main(int argc, char *argv[]) 
{
  Stack dataStack;
  Stack opStack;
  char *command = NULL;
  int max_input = MAX_INPUT_SIZE;
  int result;

  // Read command input
  // Create dataStack and opStack
  
  dataStack = Stack_create();
  opStack = Stack_create();

  while(true){
    int rc = 0; // Run condition: Status of the current command
    command = (char *) malloc(sizeof(char) * max_input);
    char *str = fgets(command, max_input, stdin);

    if (!str || *str == '\n') break;

    printf("%s", str);
    // While command != NULL, process the input
    // If process is successful...
    if(process(command, dataStack, opStack) == 0){
      // While opStack has items, runOperation through stack
      // Check runOperation for input errors
      char *op;

      //Stack_int_print(dataStack);
      //Stack_char_print(opStack);

      while(!Stack_is_empty(opStack)){
        op = Stack_pop(opStack);
        if(runOperation(op, dataStack) != 0){
          // If runOperation fails, break
          rc = -1;
          free(op);
          op = NULL;
          break;
        }

        free(op);
        op = NULL;
      }

      if(rc == 0){
        int *result_ptr;
        if(!Stack_is_empty(opStack)){
          error_msg_opStackNotEmpty(command);
        } else if (Stack_is_empty(dataStack)){
          error_msg_missingResult(command);
        } else {
          result_ptr = Stack_pop(dataStack);
          result = *result_ptr;
          if(!Stack_is_empty(dataStack)){
            error_msg_extraData(command);
          } else {
            printf("= %d\n", result);
          }
          free(result_ptr);
          result_ptr = NULL;
        }
      }
    } else {
      error_msg_badCommand(command);
    }
    // Clear stacks, take another input
    Stack_make_empty(dataStack);
    Stack_make_empty(opStack);
    free(command);
    command = NULL;

    // command = (char *) malloc(sizeof(char) * max_input);
    //str = fgets(command, max_input, stdin);
  }

  // Free memory and destroy data before returning
  free(command);
  command = NULL;
  Stack_destroy(dataStack);
  Stack_destroy(opStack);
  return 0;
}

/***********************************************************************
 This is the main skeleton for processing a command string:
***********************************************************************/
int
process(char *command, Stack dataStack, Stack opStack){
  char delim[] = " ";
  int *data = ((void *)0);
  char *operation = ((void *)0);
  int rc = 0;
  char *token_ptr;

  token_ptr = strtok(command, delim);
  while (token_ptr != ((void *)0)) {

    data = malloc(sizeof(int));
    if (sscanf(token_ptr, "%d", data) == 1){ // Sets *data if token is an int
      // If token is a data term, push it onto dataStack

      Stack_push(dataStack, data);
      data = NULL;
      //Stack_int_print(dataStack);
    } else {
      free(data); // Not using data, so free it.
      data = NULL;

      operation = malloc(sizeof(char));
      *operation = *token_ptr;
      *(operation+1) = '\0';

      if(!strcmp(operation, ")")){
        // If token is ')', call runCloseParen()
        free(operation); // Never using ")", so free the token immediately.
        operation = NULL;

        if(runCloseParen(dataStack, opStack) != 0){
          rc = -1;
          break;
        }
      } else if(!strcmp(token_ptr, "(")){
        // If token is '(', add to operator stack

        //printf("Pushing %c to opStack...\n", *operation);
        Stack_push(opStack, operation);
        operation = NULL;
        //Stack_char_print(opStack);
      } else {
        // Token is guaranteed to be an operator
        while(true){
          if(Stack_is_empty(opStack)){break;}

          if(higherPriority(Stack_peek(opStack), operation)){
            // If opStack->top has higher priority than token, runOperation() with prev operation
            // Else break from while loop
            char* prevOperation = Stack_pop(opStack);
            if(runOperation(prevOperation, dataStack) != 0){
              rc = -1;
              free(prevOperation);
              prevOperation = NULL;
              free(operation);
              operation = NULL;
              return rc;
            }
            free(prevOperation);
            prevOperation = NULL;

          } else {
            break;
          }
        }
        // Push new operator onto opStack
        //printf("Pushing %c to opStack...\n", *operation);
        Stack_push(opStack, operation);
        operation = NULL;
        //Stack_char_print(opStack);
      }
    }
    // if (data) {free(data);
    // data = NULL;}
    // if (operation) {free(operation);
    // operation = NULL;}
    token_ptr = strtok(((void *)0), delim); // Advances and stores the next "token" in the command
  }

  // FREE MEMORY
  // if(data != NULL){
  //   free(data);
  //   data = NULL;
  // }
  // if(operation != NULL){
  //   free(operation);
  //   operation = NULL;
  // }
  return rc;
}

// 3
int
runCloseParen(Stack dataStack, Stack opStack) {
  int rc = -2; // -2 means opStack is empty
  char *op = ((void *)0);

  while(!Stack_is_empty(opStack)){
    op = Stack_pop(opStack); // Grab the top item in opStack.
    if(!strcmp(op, "(")){
      rc = 0; // 0 means successful
      free(op);
      op = NULL;
      break;
    } else if(runOperation(op, dataStack) == -1){
      rc = -1; // -1 means runOperation failed
      free(op);
      op = NULL;
      break;
    }

    free(op); // Free memory associated with the operator we just used
    op = NULL;
  }

  return rc;
}

int
getPriority(char* op)
{
  if(!strcmp(op,"*") || !strcmp(op, "/")) return 2;
  if(!strcmp(op,"+") || !strcmp(op, "-")) return 1;
  return 0;
}

_Bool
higherPriority(char *oldOp, char *newOp)
{
  //printf("%c: %d, %c: %d", *oldOp, getPriority(oldOp), *newOp, getPriority(newOp));
  return getPriority(oldOp) >= getPriority(newOp);
}

// This function executes the specified operation 
//  It's arguments are the first two values on the data stack
int
runOperation(char *op, Stack dataStack)
{
  int data1;
  int data2;
  int result;
  int *data_ptr;

  // FREE DATA AFTER EACH POP
  // Check to make sure Stack is not empty before popping
  if(Stack_is_empty(dataStack)){
    error_msg_opMissingArgs(op);
    return -1;
  }
  data_ptr = Stack_pop(dataStack);
  if(data_ptr == NULL){
    // Error checking
    return -1;
  }

  data1 = *data_ptr;
  //printf("%d\n", data1);
  free(data_ptr);

  if(Stack_is_empty(dataStack)){
    error_msg_opMissingArgs(op);
    return -1;
  }
  data_ptr = Stack_pop(dataStack);
  if(data_ptr == NULL){
    // Error checking
    return -1;
  }
  data2 = *data_ptr;
  //printf("%d\n", data2);
  free(data_ptr);
  data_ptr = NULL;

  //printf("Performing runOperation with %d %c %d\n", data2, *op, data1);

  if(!strcmp(op, "+")){
    result = data2 + data1;
  } else if(!strcmp(op, "*")){
    result = data2 * data1;
  } else if(!strcmp(op, "/")){
    if(data1 == 0){
      error_msg_divByZero();
      return -1;
    }
    result = data2 / data1;
  } else if(!strcmp(op, "-")){
    result = data2 - data1;
  } else {
    // If operator is not recognized, print error message and return -1
    error_msg_badOp(op);
    return -1;
  }

  // MALLOC DATA BEFORE PUSH
  int* result_ptr = malloc(sizeof(int));
  *result_ptr = result;
  Stack_push(dataStack, result_ptr);
  //printf("Pushing %d to opStack...\n", *result_ptr);
  result_ptr = NULL;
  return 0;
}

