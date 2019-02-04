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
#define HIST_COUNT		100


//this function splits the inputBuff into tokens and stores into args array
int splitToken(char* inputBuff, char** args)
{
	char *token;
	int i = 0; //used for placing args into args array
	int tokenCount = 0;
	token = strtok(inputBuff," \n"); //splits tokens at space, tab, carriage return, new line
	while(token != NULL) //stops at terminating \0
	{
		args[i++] = token;
		token = strtok(NULL, " \n");
		tokenCount++;
	}
	args[i] = NULL;
	return tokenCount;
}
//this function checks to see if each character in an arg is &, and if it is, returns 1
int checkHasAmpAndModify(char **args)
{
	int slen;
	slen = strlen(*args);
	char *str = *args;
	if(str[slen-1] == '&'){ //checks to see if the final value, before terminating character, is '&'
		args[slen-1] = 0;// if it is, removes &
		slen--;
		if(slen == 0)
		{
			*args = NULL;
		}
		return 1;//returns 1 if last character is &
	}
	return 0;
}

//this function checks to see if inputBuff has !! or !N to decide on history feature
int stringCompare(char *inputBuff){
	int slen;	
	slen = strlen(inputBuff);
	printf("slen: %d\n",slen);
	int i; 
	char *temp = inputBuff;
	for(i = 0; i < slen; i++){
		int ascii = temp[i];//cast the array member into an ascii int
		int asciiPlusOne = temp[i+1];
		printf("ascii: %d\n", ascii);
		printf("ascii + 1: %d\n", asciiPlusOne);
		if(ascii == 33 && asciiPlusOne == 33 && slen<3)//ascii for ! is 33
		{
			return 2;
		}
		else if(temp[i] == '!' && asciiPlusOne > 47 && asciiPlusOne < 57 && slen<3){//all ascii int values are between 48 and 57
			return 1;
		}
	return 0; //will return 0 if the string does not contain !! or !N
	}
}

void history()
{
	printf("history: \n");
	
}

int addToHistory(char **historyArray, int historyCount, char *inputBuff)
{	
	int slen;
	int* temp;
	temp = NULL;
	temp = (char *)malloc(sizeof(temp));
	strcpy(temp, inputBuff);
	
	if(historyCount < HIST_COUNT){
		printf("about to copy\n");
		slen = strlen(inputBuff);
		printf("slen of inputBuff: %d\n", slen);
		strcpy(historyArray[historyCount], temp);
		
		printf("historyCount: %d\n", historyCount);
		historyCount++;
	}	
	printf("made it here");
	int j;
	for(j=0; j<historyCount; j++)
	{	
		printf("History array index: %d\n", historyCount);
		printf("history array values: %s\n", historyArray[j]);
	}
	free(temp);
	return historyCount;
}


int main(void)
{
	
	char inputBuff[MAX_LINE]; 
	char *args[MAX_LINE/2 + 1] = {0}; //declares it on the stack
    	int should_run = 1;
	pid_t pid;
	int flag;
	int stringCompareInt;
	char *historyArray[HIST_COUNT] = {0};
	int historyCount = 0; //	
	int tokenCount; //to see how many tokens are in args array

    while(should_run){   
        printf("\nosh>");
        fflush(stdout); //this allows the buffer to be flushed so you do not need a new line

	fgets(inputBuff, MAX_LINE, stdin);//get user input
	
	stringCompareInt = stringCompare(inputBuff);
	
	//returns an int, the value of which depends on characters in inputBuff
	printf("stringCompareInt: %d\n", stringCompareInt);
	if(stringCompareInt == 1 || stringCompareInt == 2){ //if inputBuff contains !! or !N
		history(stringCompareInt);
	}
	
	else if(stringCompareInt == 0){ //if inputBuff contains a command
		
		historyCount = addToHistory(&historyArray[HIST_COUNT-1], historyCount, inputBuff);
		
		tokenCount = splitToken(inputBuff, args);
	
		flag = checkHasAmpAndModify((&args[tokenCount-1]));
		
		printf("flag: %d\n", flag);
		pid = fork();
		if(pid < 0){
			printf("error occured\n");
			exit(1);
		}
		else if(pid == 0){
			printf("fork success\n");		
			execvp(args[0], args);
		}
		else{
			if(flag == 0){
				printf("waiting child\n");			
				wait(NULL);
				printf("child complete\n");
			}
		}
	}
	
	
        /**
         * After reading user input, the steps are:
         * (1) fork a child process
         * (2) the child process will invoke execvp()  
         * (3) if command included &, parent will invoke wait()
         */
	/*
	*/
    }
    
	return 0;
}
