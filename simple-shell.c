/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Ninth Edition
 * Copyright John Wiley & Sons - 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


#define MAX_LINE		80 /* 80 chars per line, per command */

//this function splits the inputBuff into tokens and stores into args array
int splitToken(char* inputBuff, char** args)
{
	char *token;
	int i = 0; //used for placing args into args array
	int tokenCount = 0;
	printf("splitting string %s into tokens:\n", inputBuff);
	token = strtok(inputBuff," \t\r\a\n"); //splits tokens at space, tab, carriage return, new line
	while(token != NULL) //stops at terminating \0
	{
		args[i++] = token;
		token = strtok(NULL, " \t\r\a\n");
		tokenCount++;
	}
	int j;
	for(j = 0; j<tokenCount; j++){
		printf("token: %s\n", args[j]);	
	}
	return tokenCount;
}

void checkHasAmp(char* arg, int *flag){
	if(strcmp(arg,"&") == 0) // will equal 0 if the two strings are equal
	{
		*flag = *flag + 1;
	}
}

void historyFeature(){
	
}

int main(void)
{
	char inputBuff[MAX_LINE]; //buffers are used to move data within processes of a computer
	char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
    	int should_run = 1;
	int length;
	//int flag; // solves the problem if the command is followed by a & by setting it equal to 1
	pid_t pid;
	
	
    while (should_run){
	//flag = 0;   
        printf("osh>");
        fflush(stdout); //this allows the buffer to be flushed so you do not need a new line

	length = read(STDIN_FILENO, inputBuff, MAX_LINE);//reads input into inputBuff array

	int tokenCount; //an easy way to see how many tokens are in args array
	tokenCount = splitToken(inputBuff, args);

	int arrayIt;
	arrayIt = 0;
	
	int flag;
	flag = 0;
	while(arrayIt < tokenCount && args[arrayIt] != NULL)//this calls checkHasAmp to see if string has &
	{
		if(flag != 1){ //flag becomes 1 in checkHasAmp if string has a &
			checkHasAmp(args[arrayIt], &flag);
		}
		arrayIt++;
	}
	pid = fork();
	if(pid < 0){
		printf("error occured");
		exit(1);
	}
	else if(pid == 0){
		printf("fork success\n");		
		execvp(args[0], args);
	}
	else{
		if(flag == 1){
			wait(NULL);
			printf("child complete");
		}
	}
        /**
         * After reading user input, the steps are:
         * (1) fork a child process
         * (2) the child process will invoke execvp()  
         * (3) if command included &, parent will invoke wait()
         */
	/*
	this means that you need to first fork a child process and add conditional statements that will execute
	if the pid meets certain conditions. 
	*/
    }
    
	return 0;
}
