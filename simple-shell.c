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

char historyArray[HIST_COUNT][MAX_LINE];
int historyCount;

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
char* convertExToCmd(int historyCount, char *inputBuff){
	int slen;	
	slen = strlen(inputBuff);
	int i; 
	char *temp = inputBuff;
	for(i = 0; i < slen; i++){
		int ascii = temp[i];//cast the array member into an ascii int
		int asciiPlusOne = temp[i+1];
		int asciiPlusTwo = temp[i+2];
		
		if(ascii == 33 && asciiPlusOne == 33 && slen<3)//ascii for ! is 33
		{	
			return historyArray[historyCount];
		}
		else if(temp[i] == '!' && asciiPlusOne > 48 && asciiPlusOne < 58 && slen<3){//all ascii int values are between 48 and 57
			int index = asciiPlusOne - 49;
			return historyArray[index];
		}
		else if(temp[i] == '!' && asciiPlusOne == 49 && asciiPlusTwo == 48 && slen<4){ //special case for !10; 1 is ascii of 49, 0 is 48;
			int index = 9;
			return historyArray[index];
		}
	
	}
	return NULL; //will return NULL if the string does not contain !! or !N
}

void history(int historyCount, char *inputBuff)
{
	if(strcmp(inputBuff, "history")==0){ //compare shell input with "history"
		int i = 0;			
		while(i < HIST_COUNT){
			printf("%d %s\n",(i+1),historyArray[i]);
			i++;
		}
	}
}

int addToHistory(int historyCount, char *inputBuff)
{	
	historyCount = (historyCount) % HIST_COUNT; //makes this a circular array
	strncpy(historyArray[historyCount], inputBuff, MAX_LINE);
	historyCount++;
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
	int tokenCount; //to see how many tokens are in args array
	historyCount = 0;

    while(should_run){   
        printf("osh>");
        fflush(stdout); //this allows the buffer to be flushed so you do not need a new line

		fgets(inputBuff, MAX_LINE, stdin); //get user input

		size_t length = strlen(inputBuff); //length of input
		if(inputBuff[length-1] == '\n'){ //replaces endline char with null terminating 0
			inputBuff[length-1] = '\0';
		}

		if(checkExit(inputBuff) == 1){ //check if inputcommand is "exit"
			printf(" exiting\n");
			exit(0);
			should_run = 0;
		}
		
		char* tmp = convertExToCmd(historyCount, inputBuff);//calls function to see if input has !! or !N
		if(tmp != NULL){
			strncpy(inputBuff, tmp, MAX_LINE);//will copy the old history command into inputBuff
		}
		historyCount = addToHistory(historyCount, inputBuff);
		history(historyCount, inputBuff);

	//returns an int, the value of which depends on characters in inputBuff
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
