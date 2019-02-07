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
int historyCount;
int arrayLength; //this keeps track of how long the array is so that you do not go out of bounds


//this function splits the inputBuff into tokens and stores into args array
void splitToken(char* inputBuff, char** args)
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
char* historyExeCmd(int historyCount, char *inputBuff){	
	int slen = strlen(inputBuff);
	int i; 
	for(i = 0; i < slen; i++){
		int asciiOne = inputBuff[i];//cast the array member into an ascii int
		int asciiTwo = inputBuff[i+1];
		int asciiThree = inputBuff[i+2];
		
		if(asciiOne == 33 && asciiTwo == 33 && slen<3)//ascii for ! is 33
		{	
			printf("osh>%s\n", historyArray[historyCount-1]);		
			return historyArray[historyCount-1];
		}
		else if(asciiOne == 33 && asciiTwo > 48 && asciiTwo < 58 && slen<3){//all ascii int values are between 48 and 57
			int index = asciiTwo - 49;
			printf("osh>%s\n", historyArray[index]);
			return historyArray[index];
		}
		else if(asciiOne == 33 && asciiTwo == 49 && asciiThree == 48 && slen<4){ //special case for !10; 1 is ascii of 49, 0 is 48;
			int index = 9;
			printf("osh>%s\n", historyArray[index]);
			return historyArray[index];
		}
	
	}
	return NULL; //will return NULL if the string does not contain !! or !N
}

int checkCustomCmd(char *inputBuff)
{
	if(strcmp(inputBuff, "history")==0){ //compare shell input with "history"
		return 1;		
	}

	else if(strcmp(inputBuff, "exit")==0){ //compare shell input with "exit"
		printf("returning 2(exit)");		
		return 2;
	}
	else{
		return 0;
	}
	
}

int addToHistory(int historyCount, char *inputBuff)
{	
	
	if(historyCount < HIST_COUNT-1){
		historyCount = (historyCount % 10); //makes this a circular array, and so that historyCount is always between 0 and 9
		strncpy(historyArray[historyCount], inputBuff, MAX_LINE);
		printf("DEBUG: HISTORYCOUNT %d\n", historyCount);
		historyCount++;
		arrayLength++;//while historyCount < 9, increments the array length

	}
	else if(arrayLength == HIST_COUNT-1){ //will copy to array if arraylength equals to 9
		strncpy(historyArray[historyCount], inputBuff, MAX_LINE);
		arrayLength++; 
	}
	else if(arrayLength == HIST_COUNT){ //the final case for when arraylength equals 10, which it will for the rest of execution
		int i;
		for(i = 1; i < HIST_COUNT; i++){
			strncpy(historyArray[i-1], historyArray[i], MAX_LINE);//copies all array positions left to make room for new array postion 10
		}
	strncpy(historyArray[historyCount], inputBuff, MAX_LINE);
	}
	return historyCount;
}

void execArgs(char** args, int flag, char* inputBuff){
	pid_t pid = fork();
	if(pid < 0){
		printf("fork error occured\n");
		exit(0);
	}
	else if(pid == 0){
		printf("fork success\n");		
		if (execvp(args[0], args) < 0){
			printf("command error occured\n");
		} else{
			historyCount = addToHistory(historyCount, inputBuff);
		}
		exit(0);
	}
	else{
		if(flag == 0){
			printf("waiting child\n");			
			wait(NULL);
			printf("child complete\n");
		}
	}
}

int checkPipe(char* inputBuff, char** strPiped){
	    
	int i; 
    for (i = 0; i < 2; i++) { 
        strPiped[i] = strsep(&inputBuff, "|"); //seperates 2 strings by pipe |
        if (strPiped[i] == NULL) 
            break; 
    } 
  
    if (strPiped[1] == NULL) 
        return 0; // returns 0 if no pipe is found
    else { 
        return 1; // returns 1 if pipe is found
    }
}

void execArgsPiped(char** args, char** argsPiped, char* inputBuff) 
{
    int pipefd[2];  
    pid_t p1, p2; 
  
    if (pipe(pipefd) < 0){ 
        printf("pipe construction error\n"); 
        return; 
    } 
    p1 = fork(); 
    if (p1 < 0){ 
        printf("fork error occured\n"); 
        return; 
    } 
    if (p1 == 0){ //fork is complete
        //child 1 executes here 
        close(pipefd[0]); //close read
        dup2(pipefd[1], STDOUT_FILENO); //duplicate output side pipe
        close(pipefd[1]); //close write
        if (execvp(args[0], args) < 0){ 
            printf("command 1 execution error\n"); 
            exit(0); 
        } 
    } else {

        p2 = fork(); 
        if (p2 < 0){ 
            printf("fork error occured\n"); 
            return; 
        } 
		//child 2 executes here 
        if (p2 == 0){ //fork is completed
            close(pipefd[1]); //close write
            dup2(pipefd[0], STDIN_FILENO); //duplicate input side pipe
            close(pipefd[0]); //close read
            if (execvp(argsPiped[0], argsPiped) < 0){ 
                printf("command 2 execution error\n"); 
                exit(0); 
            } else{
				historyCount = addToHistory(historyCount, inputBuff);
			}
        } else {  
			printf("Waiting child 1\n");
            wait(NULL);
			printf("Child 1 complete\n");
			printf("Waiting child 2\n");
            wait(NULL); 
			printf("Child 2 complete\n");
        } 
    } 
}

int main(void)
{
	
	char inputBuff[MAX_LINE]; 
	char *args[MAX_LINE/2 + 1];
	char *argsPiped[MAX_LINE/2 + 1];
    int should_run = 1;
	int flag = 0;
	int stringCompareInt;
	int tokenCount; //to see how many tokens are in args array
	char* strPiped[2];
	strncpy(historyArray[HIST_COUNT-1], "", MAX_LINE);
	historyCount = 0;
	arrayLength = 0;

    while(should_run){   
        printf("osh>");
        fflush(stdout); //this allows the buffer to be flushed so you do not need a new line

		fgets(inputBuff, MAX_LINE, stdin); //get user input

		size_t length = strlen(inputBuff); //length of input
		if(inputBuff[length-1] == '\n'){ //replaces endline char with null terminating 0
			inputBuff[length-1] = '\0';
		}

		char* historyCmd = historyExeCmd(historyCount, inputBuff);
		if(historyCmd != NULL){
			strncpy(inputBuff, historyCmd, MAX_LINE);//will copy the old history command into inputBuff
		}
		int pipeStatus = checkPipe(inputBuff,strPiped); //0 is no pipe, 1 has pipe
		int commandStatus = checkCustomCmd(inputBuff); // 0 default command; 1 history; 2 exit
	
		if(commandStatus == 2){ //"exit" command
			printf(" exiting\n");
			should_run = 0;
			exit(0);
		}
		else if(commandStatus == 1){ //"history" command
			historyCount = addToHistory(historyCount, inputBuff);	
			int i = 0;
			while(i < HIST_COUNT){
				printf("%d %s\n",(i+1),historyArray[i]);
				i++;
			}
		}
		else if(commandStatus == 0){ //command is not custom
			if(pipeStatus == 0){ //standard command(no pipe)
				splitToken(inputBuff, args);
				//flag = checkHasAmpAndModify((&args[tokenCount-1]));
				//printf("flag: %d\n", flag);
				execArgs(args, flag, inputBuff); //standard execution
			}
			else if(pipeStatus == 1){ //piped command
				splitToken(strPiped[0], args);
				splitToken(strPiped[1], argsPiped);
				//flag = checkHasAmpAndModify((&args[tokenCount-1]));
				//printf("flag: %d\n", flag);
				execArgsPiped(args, argsPiped, inputBuff); //piped execution
			}
		}

		
		
	//returns an int, the value of which depends on characters in inputBuff
		
	
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
