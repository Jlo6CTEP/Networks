#include <windows.h>
#include <stdio.h>



PHANDLE read_end;
PHANDLE write_end;

DWORD WINAPI server(void* data) {
	
}

DWORD WINAPI client() {
	
}

int main() {
	if(!CreatePipe(read_end, write_end, NULL, 0))
		printf("Shit happens\n");
		
	HANDLE thread_server = CreateThread(NULL, 0, server, (void*) NULL, 0, NULL);
	HANDLE thread_client = CreateThread(NULL, 0, client, (void*) NULL, 0, NULL);
		
	return 0;
}