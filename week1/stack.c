#include <stdio.h>

#define SIZE 128

int array_stack[SIZE];

int pointer = 0;



int peek() {
	return(array_stack[pointer]);
}

void push(int data) {
	if(pointer > SIZE) 
		printf("Error stack is full");
	else
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
	display();
	pop();
	pop();
	display();
	pop();
	pop();
	display();
}
