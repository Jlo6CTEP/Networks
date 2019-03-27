
#include <stdio.h>
#include "array_list/alist.h"
#include <netdb.h>
#include <memory.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ffi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>


int main() {

    network_node * nn2 = (network_node *)malloc(sizeof(network_node));
    strcpy(nn2->node, "Alpha:127.0.0.1:8080");
    array_list_add_file(nn2, "ffghf");
    array_list_add_file(nn2, "khhhkk");
    array_list_add_file(nn2, "ffghf");
    array_list_add_file(nn2, "khhhkk");
    array_list_add_file(nn2, "ffghf");
    array_list_add_file(nn2, "khhhkk");

    network_node * nn3 = (network_node *)malloc(sizeof(network_node));
    strcpy(nn3->node, "Beta:127.0.0.1:8080");
    array_list_add_file(nn3, "fffgh");
    array_list_add_file(nn3, "kfghkk");

    network_node * nn4 = (network_node *)malloc(sizeof(network_node));
    strcpy(nn4->node, "Gamma:127.0.0.1:8080");
    array_list_add_file(nn4, "ffggg");
    array_list_add_file(nn4, "kkfghk");

    network_node * nn5 = (network_node *)malloc(sizeof(network_node));
    strcpy(nn5->node, "Tango:127.0.0.1:8080");
    array_list_add_file(nn5, "ffgfgh");
    array_list_add_file(nn5, "kkghghk");

    network_node * nn6 = (network_node *)malloc(sizeof(network_node));
    strcpy(nn6->node, "Charlie:127.0.0.1:8080");
    array_list_add_file(nn6, "ffgfgh");
    array_list_add_file(nn6, "kkghghk");

    network_node * nn7 = (network_node *)malloc(sizeof(network_node));
    strcpy(nn7->node, "Delta:127.0.0.1:8080");
    array_list_add_file(nn7, "ffgfgh");
    array_list_add_file(nn7, "kkghghk");

    p_array_list alist = create_array_list();

    array_list_add(alist, nn2);
    array_list_add(alist, nn3);
    array_list_add(alist, nn4);
    array_list_add(alist, nn5);
    array_list_add(alist, nn6);
    array_list_add(alist, nn7);

    printf("size: %ld, count: %ld\n", alist->size, alist->count);
    int is_shtf = 1;
    size_t length;


    size_t iter = array_list_iter(alist, &is_shtf);
    while(is_shtf >= 0) {
        printf("%s \n", array_list_get(alist, iter, &is_shtf)->files);
        iter = array_list_next(alist, iter, &is_shtf);
    }
    printf("kek\n");

    delete_array_list(alist);

    return 0;
}
