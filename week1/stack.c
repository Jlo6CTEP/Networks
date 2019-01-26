#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>


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


int main() {
	create();
	push(1);
	push(2);
	push(3);
	display();
	push(4);
	push(5);
	push(6);
	push(7);

	display();
	pop();
	pop();
	display();
	pop();
	pop();
	display();
	free(array_stack);
	return 0;
}
