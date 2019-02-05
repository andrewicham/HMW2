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
#define HIST_COUNT		10
char historyArray[HIST_COUNT][MAX_LINE];

//this function checks if the command entered is exit
int checkExit(char *inputBuff){
	if(strcmp(inputBuff, "exit")==0){ //compare shell input with "exit"
		printf("returning 1(exit)");		
		return 1;
	}
	else{
		return 0;
	}
}
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
char* stringCompare(int historyCount, char *inputBuff){
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
			return historyArray[historyCount];
		}
		else if(temp[i] == '!' && asciiPlusOne > 47 && asciiPlusOne < 57 && slen<3){//all ascii int values are between 48 and 57
			return historyArray[(int)asciiPlusOne];
		}
	return inputBuff; //will return 0 if the string does not contain !! or !N
	}
}

void history(int historyCount, char *inputBuff)
{
	if(strcmp(inputBuff, "history")==0){ //compare shell input with "history"
		int i = 0;			
		while(i < historyCount){
			printf("%d %s\n",(i+1),historyArray[i]);
			i++;
		}
	}	
	
}

int addToHistory(int historyCount, char *inputBuff)
{	
	int slen;
	printf("%s",inputBuff);
	if(historyCount < HIST_COUNT){
		printf("about to copy\n");
		slen = strlen(inputBuff);
		printf("slen of inputBuff: %d\n", slen);
		strcpy(historyArray[historyCount], inputBuff);
		printf("historyCount: %d\n", historyCount);
		historyCount++;
	}	
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
	int historyCount = 0; //	
	int tokenCount; //to see how many tokens are in args array

    while(should_run){   
        printf("\nosh>");
        fflush(stdout); //this allows the buffer to be flushed so you do not need a new line

	fgets(inputBuff, MAX_LINE, stdin); //get user input
	
	size_t length = strlen(inputBuff); //length of input
	if(inputBuff[length-1] == '\n'){ //replaces endline char with null terminating 0
		inputBuff[length-1] = '\0';
	}
	if(checkExit(inputBuff) == 1){ //check if input is command is "exit"
		printf(" exiting\n");
		exit(0);
		should_run = 0;
	}
	history(historyCount, inputBuff);
	historyCount = addToHistory(historyCount, inputBuff);
	strcpy(inputBuff, stringCompare(historyCount, inputBuff));

	
	//returns an int, the value of which depends on characters in inputBuff

		

		
		
		
		int j;
		for(j=0; j<historyCount; j++){	
			printf("History array index: %d\n", j);
			printf("history array values: %s\n", historyArray[j]);


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
