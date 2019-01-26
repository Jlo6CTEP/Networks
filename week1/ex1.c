#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <sys/time.h>

HANDLE read_end;
HANDLE write_end;



#define SIZE 10

int* array_stack;
int pointer = 0;
int size = SIZE;



int peek() {
	return(array_stack[pointer-1]);
}

void push(int data) {
	if(pointer >= size) 
		array_stack = (int*)realloc(array_stack, size * sizeof(int) * 2);
	size*=2;
	array_stack[pointer++] = data;
}

void pop() {
	pointer = pointer - 1 >= 0 ? pointer - 1 : pointer ;
}

int empty() {
	return (pointer == 0);
}

void display() {
	for(int i = 0; i < pointer; i++)
		printf("%d", array_stack[i]);
	printf("\n");
}

void create() {
	array_stack = (int*)malloc(SIZE);
	pointer = 0;
}

void stack_size() {
	printf("%d\n", pointer);
}


DWORD WINAPI server() {
	int parsed_command[2];
	while(1) {
		long unsigned int bytes_written;
		ReadFile(read_end, parsed_command, 2*sizeof(int), &bytes_written, NULL);
		switch(parsed_command[0]) {
			case 0:
				printf("%d\n", peek());
				break;
			case 2:
				pop();
				break;
			case 3:
				printf("%d\n", empty());
				break;
			case 4:
				display();
				break;
			case 5:
				create();
				break;
			case 6:
				stack_size();
				break;
			case 1:
				push(parsed_command[1]);
		}	
	}
}

/*
	code 0: int peek()  - return the upper element in stack (without poping it) 
	code 1:void push(int data) - push new element into the stack 
	code 2:	void pop() - pop upper element from the stack int 
	code 3:int empty() - check if the stack is empty 
	code 4:	void display() - print the stack 
	code 5:void create() - create a new empty stack 
	code 6:void stack_size() - print stack size
*/

int* parse_stuff(char* command, int return_array[]) {
	return_array[1] = 0;
	if (strcmp(command, "peek()") == 0)
		return_array[0] = 0;
	else if (strcmp(command, "pop()") == 0)
		return_array[0] = 2;
	else if (strcmp(command, "empty()") == 0)
		return_array[0] = 3;
	else if (strcmp(command, "display()") == 0)
		return_array[0] = 4;
	else if (strcmp(command, "create()") == 0)
		return_array[0] = 5;
	else if (strcmp(command, "stack_size()") == 0) 
		return_array[0] = 6;
	else if (strstr(command, "push(") != NULL && strchr(command, ')') != 0) {
		int in_len =(strchr(command, ')') - command - 5);
		char* input = malloc(in_len + 1);
		input[in_len] = '\0';
		memcpy((void*) input, (void*)(command + 5), in_len);
		return_array[0] = 1;
		return_array[1] = atoi(input);
	} else
		return_array[0] = -1;			
	return return_array;
}

int main() { //also this guy are gonna be a client
	HANDLE thread_server = CreateThread(NULL, 0, server, (void*) NULL, 0, NULL);
	if(!CreatePipe(&read_end, &write_end, NULL, 0))
		printf("Shit happens\n");

	int next = 1;

	while(next != -1) {
		char command[64];
		scanf("%s", command);
		int parsed_command[2];
		parse_stuff(command, parsed_command);
		next = parsed_command[0];
		long unsigned int bytes_written;
		WriteFile(write_end, parsed_command, 2*sizeof(int),&bytes_written, NULL);
	}
	free(array_stack);			
	CloseHandle(thread_server);
	return 0;
}