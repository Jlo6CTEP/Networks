#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

PHANDLE read_end;
PHANDLE write_end;



#define SIZE 10

int* array_stack;
int pointer = 0;
int size = SIZE;



int peek() {
	return(array_stack[pointer]);
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
	printf("%d", pointer);
}


DWORD WINAPI server(void* data) {
	
}

DWORD WINAPI client() {
	
}
/*
	int peek()  - return the upper element in stack (without poping it) 
	void push(int data) - push new element into the stack 
	void pop() - pop upper element from the stack int 
	int empty() - check if the stack is empty 
	void display() - print the stack 
	void create() - create a new empty stack 
	void stack_size() - print stack size
*/

int parse_stuff(char* command) {
	if (strcmp(command, "peek()") == 0)
		return peek();
	if (strcmp(command, "pop()") == 0)
		pop();
	if (strcmp(command, "empty()") == 0)
		return empty();
	if (strcmp(command, "display()") == 0)
		display();
	if (strcmp(command, "create()") == 0)
		create();
	if (strcmp(command, "stack_size()") == 0)
		stack_size();
			
}

int main() {
	if(!CreatePipe(read_end, write_end, NULL, 0))
		printf("Shit happens\n");
		
	HANDLE thread_server = CreateThread(NULL, 0, server, (void*) NULL, 0, NULL);
	HANDLE thread_client = CreateThread(NULL, 0, client, (void*) NULL, 0, NULL);
		
	return 0;
}