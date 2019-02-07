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
#define HIST_COUNT		10 /* 10 commands max in history*/

char historyArray[HIST_COUNT][MAX_LINE]; //global array of command history
int historyCount; //tracks amount of commands executed
int arrayLength; //tracks size of the historyArray[]


//this function splits the inputBuff into tokens and stores into args array
void splitToken(char* inputBuff, char** args)
{
	char *token;
	int i = 0;
	int tokenCount = 0;
	token = strtok(inputBuff," \n"); //splits tokens at new line
	while(token != NULL) //stops at terminating \0
	{
		args[i++] = token; //inserts argument split by space into an index in args[]
		token = strtok(NULL, " \n");
		tokenCount++;
	}
	args[i] = NULL; //last argument is NULL termination
}
//this function checks to see if each character in an arg is &, and if it is, returns 1
int checkHasAmpAndModify(char **args, size_t length)
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
	int slen = strlen(inputBuff); //find length of inputBuff
	int i = 0;
	int asciiArray[slen]; //array for ascii values of history feature command
	memset(asciiArray, 0, slen*sizeof(int));
	int asciiValue = inputBuff[i]; //is the first character a (!), if not we can save time and return null
	if(asciiValue == 33 && slen > 1){	
		for(i = 0; i < slen; i++){ //loop to fill asciiArray with values from command
			asciiValue = (int)inputBuff[i]; //cast char to int(ascii)
			if(asciiValue == 33 || (asciiValue > 48 && asciiValue < 58)){
				asciiArray[i] = asciiValue;
			}
			else{ 
				printf("Invalid history command\n");
				exit(0);			
			}
		}
		if(asciiArray[0] == 33 && asciiArray[1] == 33 && slen<3){ // (!!) command
			if(historyCount > 0){			
				printf("osh>%s\n", historyArray[arrayLength-1]);
				return historyArray[arrayLength-1];
			}
			else{
				printf("No commands in history\n");
				exit(0);
			}
		}
		else{ //(!N) command
			memmove(inputBuff, inputBuff+1,strlen(inputBuff)); //remove first !
			int x = atoi(inputBuff); //cast char number to int
			if(x <= historyCount && x >= (historyCount-9) && x > 0 && historyCount > 0){
				if (historyCount > 10){
					int index = (x-(historyCount - 9)); //finds index when more than 10 commands have been entered
					printf("osh>%s\n", historyArray[index]);
					return historyArray[index];
				}
				else{
					int index = x-1;
					printf("osh>%s\n", historyArray[index]);
					return historyArray[index];
				}
			}
			else{
				printf("No such command in history\n");
				exit(0);
			}
		}
		
	}
	
	return NULL; //will return NULL if the string does not contain !! or !N
}

//this function checks if the command is a custom supported command and returns a status
int checkCustomCmd(char *inputBuff)
{
	if(strcmp(inputBuff, "history")==0){ //compare shell input with "history"
		return 1;		
	}
	else if(strcmp(inputBuff, "exit")==0){ //compare shell input with "exit"		
		return 2;
	}
	else{
		return 0;
	}
}

//this function adds the current command to history and manages historyArray[]
void addToHistory(char *inputBuff)
{	
	historyCount++; //increase historyCount every time function is called
	if(historyCount <= HIST_COUNT){
		strncpy(historyArray[historyCount-1], inputBuff, MAX_LINE); //copy current command into historyArray[]
		arrayLength++; //while historyCount < 10, increments the array length

	}
	else if(historyCount > HIST_COUNT){ //if more than 10 commands have been entered...
		int i;
		for(i = 1; i < HIST_COUNT; i++){
			strncpy(historyArray[i-1], historyArray[i], MAX_LINE); //copies all array positions down to make room for new array index 9
		}
		strncpy(historyArray[9], inputBuff, MAX_LINE); //copy current command into max historyArray index 9
	}
}

//this function executes a custom supported command
void execCustomCmd(char* inputBuff, int* should_run, int commandStatus){
	pid_t pid = fork();
	if(pid < 0){
		printf("fork error occured\n");
		exit(0);
	}
	else if(pid == 0){ //successful fork		
		if (commandStatus == 1){ //custom "history" command
			int i = 0;
			int j;
			if (historyCount > 10){
				j = (historyCount - 9);
			}else{ j = 1; }

			while(i < HIST_COUNT){
				printf("%d %s\n",(j),historyArray[i]); //print most recent 10 commands from hsitoryArray[] 
				j++;
				i++;
			}
		}
		else if(commandStatus == 2){ //custom "exit" command
			printf(" exiting\n");
			should_run = 0;
		}
		exit(0);
	}
	else{	
		wait(NULL); //child invoke wait
	}
}

//this function executes a normal non-pipe command
void execArgs(char** args, int flag, char* inputBuff){
	pid_t pid = fork();
	if(pid < 0){
		printf("fork error occured\n");
		exit(0);
	}
	else if(pid == 0){ //successful fork
		splitToken(inputBuff, args); //fills args[] array with commands from inputBuff		
		if (execvp(args[0], args) < 0){ //executes command(s) and checks for return error 
			printf("command error occured\n");
		} 
		exit(0);
	}
	else{
		if(flag == 0){			
			wait(NULL); //child invoke wait
		}
	}
}

//this function checks if inputBuff contains a pipe
int checkPipe(char* inputBuff, char** strPiped){    
	int i; 
    	for (i = 0; i < 2; i++) { 
        	strPiped[i] = strsep(&inputBuff, "|"); //seperates 2 strings by pipe |
        	if (strPiped[i] == NULL) 
           	break; 
    	} 
    	if (strPiped[1] == NULL){
        	return 0; // returns 0 if no pipe is found
	}
    	else { 
        	return 1; // returns 1 if pipe is found
    	}
}

//this function executes a pipe command
void execArgsPiped(char** args, char** argsPiped, char* inputBuff, char** strPiped) 
{
    	int pipefd[2]; //read and write  
    	pid_t pid1;
	pid_t pid2;
  
    	if (pipe(pipefd) < 0){ //create proccess pipe and check for return error
        	printf("pipe construction error\n"); 
        return; 
    	} 
    	pid1 = fork(); 
    	if (pid1 < 0){ 
        	printf("fork error occured\n"); 
        	return; 
    	} 
	//child 1 executes here
   	if (pid1 == 0){ //successful fork 
		splitToken(strPiped[0], args); //fills args[] array with commands from left side of pipe
       		dup2(pipefd[1], STDOUT_FILENO); //duplicate output side pipe
		close(pipefd[0]); //close read
        
        if (execvp(args[0], args) < 0){ //executes command(s) and checks for return error 
            printf("command 1 execution error\n"); 
            exit(0); 
        } 
    	} else {

        pid2 = fork(); 
        if (pid2 < 0){ 
            printf("fork error occured\n"); 
            return; 
        } 
	//child 2 executes here 
        if (pid2 == 0){ //successful fork
		splitToken(strPiped[1], argsPiped); //fills argsPiped[] array with commands from right side of pipe
            	dup2(pipefd[0], STDIN_FILENO); //duplicate input side pipe
		close(pipefd[1]); //close write

           	if (execvp(argsPiped[0], argsPiped) < 0){ //executes command(s) and checks for return error 
                	printf("command 2 execution error\n"); 
                	exit(0); 
            }
        }  
		close(pipefd[0]); //close read
		close(pipefd[1]); //close write
            	wait(NULL);
            	wait(NULL); 
    } 
}

int main(void)
{
	
	char inputBuff[MAX_LINE]; 
	char *args[MAX_LINE/2 + 1] = {0};
	char *argsPiped[MAX_LINE/2 + 1];
    int should_run = 1;
	int flag = 0;
	int stringCompareInt;
	int tokenCount; //to see how many tokens are in args array
	char* strPiped[2];
	int pipeStatus;
	int commandStatus;
	strncpy(historyArray[HIST_COUNT-1], "", MAX_LINE);
	historyCount = 0;
	arrayLength = 0;

    while(should_run){   
        printf("osh>");
        fflush(stdout); //this allows the buffer to be flushed so you do not need a new line

		fgets(inputBuff, MAX_LINE, stdin); //get user input	

		size_t length = strlen(inputBuff);
		if(inputBuff[length-1] == '\n'){ //replaces endline char with null terminating 0
			inputBuff[length-1] = '\0';
		}
		char* historyCmd = historyExeCmd(historyCount, inputBuff); //is input a !! or !N
		if(historyCmd != NULL){
			strncpy(inputBuff, historyCmd, MAX_LINE); //copy the history command into inputBuff
		}
		addToHistory(inputBuff); //add the current inputBuff to historyArray[]
		pipeStatus = checkPipe(inputBuff,strPiped); //0 is no pipe, 1 has pipe
		commandStatus = checkCustomCmd(inputBuff); //0 default command; 1 history; 2 exit
		if(commandStatus == 1 || commandStatus == 2){ //execute custom command			
			execCustomCmd(inputBuff, &should_run, commandStatus);
		}
		else if(commandStatus == 0){ //command is not custom
			if(pipeStatus == 0){ //standard command(no pipe)
				//flag = checkHasAmpAndModify((&args[tokenCount-1]), length);
				execArgs(args, flag, inputBuff); //standard execution
			}
			else if(pipeStatus == 1){ //piped command
				//flag = checkHasAmpAndModify((&args[tokenCount-1]), length);
				execArgsPiped(args, argsPiped, inputBuff, strPiped); //piped execution
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
