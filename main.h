/**************************************************************************************************************
In this exercise, we will execute a simple C language shell under the Linux operating system,
 the shell will show PROMPT to the user, read the commands and send them to the operating system for execution.
 **************************************************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include "pipe.h"

#define INPUT_SIZE 510 //The length of the maximum string for the user
#define CUTTING_WORD " \n"//For dividing a string into single words (using in strtok)
#define ENDING_WORD "exit"//Program end word
#define RESET 0
#define NAME_SHELL "davixie_shell"

#define true 1
#define false 0

#define pipe_redirection "|"
#define pipe_stdout ">"
# define pipe_stdin "<"

 /*****************************Private Function declaration******************************/
void  DisplayPrompt();//Display Prompt : user@currect dir>
char** getArgv(char *input,char **argv,int *sizeOfArray,int *cmdLength, int count);  //Preparation of a receiver input as an expense
void garbageCollector(char** argv,int size); //Memory Release
int *countInput(char *input,int *cmdLength);
void executeCommand(char **argv,int sizeOfArray);
void executePipeRedirect(char *input, char **argv, int sizeOfArray, int *exist_token);

 /****************************/
static int *numOfCmd;
static int *cmdLength;
/****************************/
